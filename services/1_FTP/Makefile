default: clean build run

clean:
	rm -rf out

build:
	mkdir -p out
	cd out && cmake .. && make ftp_service
	cd ..

run:
	cd out/ftp_service && ./ftp_service && cd ..

test:
	bash many_clients.sh

debug:
	make clean
	make build
	cd out/ftp_service && gdb ftp_service && cd ..

valgrind:
	make clean
	make build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./out/ftp_service/ftp_service

