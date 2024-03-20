default: s
	
c:
	cd clients && make && cd ..

s:
	cd server && make && cd ..

simple:
	gcc -g -o main main.c
	./main

clean:
	cd clients && make clean && cd ..
	cd server && make clean && cd ..

save: MESSAGE
	git add . && git commit $(MESSAGE) git push -u

save: 
	clean
	save
	
