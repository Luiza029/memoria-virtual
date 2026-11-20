vm: main.o vm.o
	gcc -pthread main.o vm.o -o vm

main.o: main.c vm.h
	gcc -Wall -Wextra -pthread -c main.c

vm.o: vm.c vm.h
	gcc -Wall -Wextra -pthread -c vm.c

run: vm
	./vm addresses.txt lru fifo

clean: 
	rm -f *.o vm correct.txt

.PHONY: run clean