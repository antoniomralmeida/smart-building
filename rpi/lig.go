package main

import (
	"log"
	"os"
	"strconv"
	"syscall"
	"time"
)

const (
	YYYYMMDD = "2006-01-02"
)

var (
	PRODUCT_NAME = "E-BUSMASTER"
	VERSION      = "V1.0 B"
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
	defer f.Close()

	log.SetOutput(f)
	log.Println("Loogin in " + logFile)

	_, _, ctime, err := statTimes(os.Args[0])
	if err != nil {
		log.Println(err)
	}
	VERSION = VERSION + " " + strconv.Itoa(ctime.Nanosecond())
	PRODUCT_NAME = PRODUCT_NAME + " " + VERSION
}
