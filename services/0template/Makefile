default: clean build run

clean:
	rm -rf out

build:
	mkdir -p out
	cd out && cmake .. && make server
	cd ..

run:
	cd out/server && ./server && cd ..

