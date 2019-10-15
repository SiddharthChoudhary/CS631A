CC=gcc
ls: ls.c
	$(CC) ls.c -o ls -g -lm -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init -Wshadow 
debug:
	cc -g -lm ls.c -o ls
clean:
	rm ls a.out