default: s

c:
	cd clients && make && cd ..

s:
	cd server && make && cd ..

a:
	cd auth && make && cd ..

simple:
	gcc -g -o main main.c
	./main

MESSAGE ?= "Update - automatic makefile message"
save: 
	@make clean
	git add . 
	git commit -m "$(MESSAGE)"
	git push -u

clean:
	cd clients && make clean && cd ..
	cd server && make clean && cd ..
