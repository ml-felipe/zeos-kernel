CC ?= gcc
.PHONY: test clean
test:
	$(CC) -std=gnu11 -g -O1 -ffunction-sections -fdata-sections -I../include -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast test_unblock.c ../sys.c ../list.c -Wl,--gc-sections -o test_unblock
	./test_unblock
clean:
	rm -f test_unblock
