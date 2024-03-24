default: clean build run

clean:
	rm -rf out

build:
	mkdir -p out
	cd out && cmake .. && make broadcastChat
	cd ..

run:
	cd out/broadcastChat && ./broadcastChat && cd ..

