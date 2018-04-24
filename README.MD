# Performance Statistics Collector. Set-UID program that collects information from the /proc filesystem on UNIX computers.

build: gcc -o getData main.c processInfo.c processInfo.h

run:
sudo chown root getData
sudo chmod u+s getData
./getData > proc_data.txt