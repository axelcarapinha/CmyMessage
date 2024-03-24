default: clean build run
	
clean:
	rm -rf out

build:
	mkdir -p out
	cd out && cmake .. && make auth
	cd ..

run:
	cd out/auth && ./auth && cd ..

simple:
	gcc -g -o main main.c
	./main

