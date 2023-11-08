env GOOS=linux GOARCH=arm64 go build -o ebm-rpi_arm64
sshpass -p domluis810 scp ebm-rpi_arm64 bdl@10.241.0.42:/home/bdl/ebusmaster
