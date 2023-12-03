env GOOS=linux GOARCH=riscv64 go build -o ebm-rpi_riscv64
sshpass -p domluis810 scp ebm-rpi_riskv64 bdl@192.168.1.196:/home/bdl/ebusmaster
