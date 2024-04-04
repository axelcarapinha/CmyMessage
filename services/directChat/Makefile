default: clean build run

clean:
	rm -rf out

build:
	mkdir -p out
	cd out && cmake .. && make broadcastChat
	cd ..

run:
	cd out/broadcastChat && ./broadcastChat && cd ..

test:
	bash many_clients.sh

debug:
	make clean
	make build
	cd out/broadcastChat && gdb broadcastChat && cd ..

valgrind:
	make clean
	make build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./out/broadcastChat/broadcastChat

