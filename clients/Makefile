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

debug:
	make clean
	make build
	cd out/clients && gdb clients && cd ..

valgrind:
	make clean
	make build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./out/clients/clients


