default: clean build run
	
clean:
	rm -rf out

build:
	mkdir -p out
	cd out && cmake .. && make clients
	cd ..

run:
	cd out/clients && ./clients && cd ..

simple:
	gcc -g -o main main.c
	./main

