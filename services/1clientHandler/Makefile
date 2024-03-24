default: clean build run

clean:
	rm -rf out

build:
	mkdir -p out
	cd out && cmake .. && make 1clientHandler
	cd ..

run:
	cd out/1clientHandler && ./1clientHandler && cd ..

