default: clean build run
	
build:
	mkdir -p out
	cd out && cmake .. && make clients
	cd ..

clean:
	rm -rf out

run:
	cd out/clients && ./clients && cd ..

simple:
	gcc -g -o main main.c
	./main

