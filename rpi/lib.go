package main

import (
	"fmt"
	"log"
	"net"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"syscall"
	"time"

	_ "github.com/dpapathanasiou/go-modbus"
	mqtt "github.com/eclipse/paho.mqtt.golang"
)

const (
	YYYYMMDD = "2006-01-02"
)

var (
	PRODUCT_NAME = "E-BUSMASTER"
	VERSION      = "V1.1 B"
)

func statTimes(name string) (atime, mtime, ctime time.Time, err error) {
	fi, err := os.Stat(name)
	if err != nil {
		return
	}
	mtime = fi.ModTime()
	stat := fi.Sys().(*syscall.Stat_t)
	atime = time.Unix(int64(stat.Atim.Sec), int64(stat.Atim.Nsec))
	ctime = time.Unix(int64(stat.Ctim.Sec), int64(stat.Ctim.Nsec))
	return
}

func setupLog() {
	logFile := "log/ebusmaster" + time.Now().Format(YYYYMMDD) + ".log"
	f, err := os.OpenFile(logFile, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0666)
	if err != nil {
		log.Fatalf("error opening file: %v", err)
	}

	log.SetOutput(f)
	log.Println("Loogin in " + logFile)

	_, _, ctime, err := statTimes(os.Args[0])
	if err != nil {
		log.Println(err)
	}
	VERSION = VERSION + " " + strconv.Itoa(ctime.Nanosecond())
	PRODUCT_NAME = PRODUCT_NAME + " " + VERSION
}

func LogFatal(v ...any) {
	log.Println(v)
	os.Exit(2)
}

func LogPrint(v ...any) {
	fmt.Println(v)
	log.Println(v)
}

func RaspBerryID() string {
	cmd := "cat /proc/cpuinfo | grep Serial"
	out, err := exec.Command("bash", "-c", cmd).Output()
	if err != nil {
		LogFatal(err)
	}
	fields := strings.Split(string(out), ":")
	return strings.Trim(fields[1], " ")[0:16]
}

// Get preferred outbound ip of this machine
func GetOutboundIP() string {
	conn, err := net.Dial("udp", "8.8.8.8:80")
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()

	localAddr := conn.LocalAddr().(*net.UDPAddr)

	return localAddr.IP.String()
}

// this callback triggers when a message is received, it then prints the message (in the payload) and topic
var messagePubHandler mqtt.MessageHandler = func(client mqtt.Client, msg mqtt.Message) {
	fmt.Printf("Received message: %s from topic: %s\n", msg.Payload(), msg.Topic())
}

// upon connection to the client, this is called
var connectHandler mqtt.OnConnectHandler = func(client mqtt.Client) {
	fmt.Println("Connected")
}

// this is called when the connection to the client is lost, it prints "Connection lost" and the corresponding error
var connectLostHandler mqtt.ConnectionLostHandler = func(client mqtt.Client, err error) {
	fmt.Printf("Connection lost: %v", err)
}

func SetupMQTT() {
	var broker = "83734c7eda514823bc0bfaa4a35fa596.s2.eu.hivemq.cloud" // find the host name in the Overview of your cluster (see readme)
	var port = 8883                                                    // find the port right under the host name, standard is 8883
	opts := mqtt.NewClientOptions()
	opts.AddBroker(fmt.Sprintf("tls://%s:%d", broker, port))
	opts.SetClientID(RaspBerryID()) // set a name as you desire
	opts.SetUsername("<username>")  // these are the credentials that you declare for your cluster (see readme)
	opts.SetPassword("<password>")
	// (optionally) configure callback handlers that get called on certain events
	opts.SetDefaultPublishHandler(messagePubHandler)
	opts.OnConnect = connectHandler
	opts.OnConnectionLost = connectLostHandler
	// create the client using the options above
	client := mqtt.NewClient(opts)
	// throw an error if the connection isn't successfull
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		LogFatal(token.Error())
	}

	subscribe(client)
	publish(client)

}

func subscribe(client mqtt.Client) {
	// subscribe to the same topic, that was published to, to receive the messages
	topic := "topic/test"
	token := client.Subscribe(topic, 1, nil)
	token.Wait()
	// Check for errors during subscribe (More on error reporting https://pkg.go.dev/github.com/eclipse/paho.mqtt.golang#readme-error-handling)
	if token.Error() != nil {
		fmt.Printf("Failed to subscribe to topic")
		panic(token.Error())
	}
	fmt.Printf("Subscribed to topic: %s", topic)
}

func publish(client mqtt.Client) {
	// publish the message "Message" to the topic "topic/test" 10 times in a for loop
	num := 10
	for i := 0; i < num; i++ {
		text := fmt.Sprintf("Message %d", i)
		token := client.Publish("topic/test", 0, false, text)
		token.Wait()
		// Check for errors during publishing (More on error reporting https://pkg.go.dev/github.com/eclipse/paho.mqtt.golang#readme-error-handling)
		if token.Error() != nil {
			fmt.Printf("Failed to publish to topic")
			panic(token.Error())
		}
		time.Sleep(time.Second)
	}
}
