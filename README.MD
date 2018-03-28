build: gcc -o getData main.c processInfo.c processInfo.h

run:
sudo chown root getData
sudo chmod u+s getData
for i in {1..4}; do ./getData >> samples.txt; sleep 15; done