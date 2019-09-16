CC=gcc
bbcp: bbcp.c
	$(CC) bbcp.c -o bbcp -ansi -g -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init -Wlogical-op -Wpedantic -Wshadow
clean:
	rm bbcp