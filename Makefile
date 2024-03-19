default: clients server
	
clients:
	cd clients
	make clean
	make build
	make run
	cd ..

server:
	cd server
	make clean
	make build
	make run
	cd ..

simple:
	gcc -g -o main main.c
	./main
