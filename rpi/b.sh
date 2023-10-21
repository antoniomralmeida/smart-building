env GOOS=linux GOARCH=arm64 go build -o ebm-rpi_arm64
sshpass -p domluis810 scp ebm-rpi_arm64 bdl@192.168.1.196:/home/bdl/ebusmaster
