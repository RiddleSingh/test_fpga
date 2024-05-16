#sudo apt-get install libatlas-base-dev 
#sudo apt-get install libgsl-dev
cpu: main.c
	gcc -Wall -O3 -I/usr/local/include -c main.c -o main.o
	gcc -L/usr/local/lib -L/opt/lib -O3 main.o -lm -lgsl -lcblas -o app
	