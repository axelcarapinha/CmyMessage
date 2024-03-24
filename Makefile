default: s

c:
	cd clients && make && cd ..

s:
	cd services/1clientHandler && make && cd ..

a:
	cd auth && make && cd ..

simple:
	gcc -g -o main main.c
	./main

MSG ?= "Update - automatic makefile message"
save: 
	@make clean
	git add . 
	git commit -m "$(MSG)"
	git push -u

clean:
	cd clients && make clean && cd ..
	find services/*/ -mindepth 1 -maxdepth 1 -type d -exec make clean -C {} \;
