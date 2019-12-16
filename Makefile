EXEC = sish
SRC = sish.c execute.c
OBJS = sish.o execute.o
MAIN = main.c
CFLAGS = -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init -Wlogical-op -Wpedantic -Wshadow -lmagic 

all: ${EXEC}
	cc -o ${EXEC} ${OBJS} -g ${MAIN} ${CFLAGS}
debug: ${EXEC}
	cc -o ${EXEC} ${OBJS} ${MAIN} ${CFLAGS} -g3 -D SWSDEBUG
${EXEC}: ${OBJS}
	cc -c -g ${SRC} ${CFLAGS}
run: all
	./$(EXEC)
clean:
	rm -f *.o *.core
fclean:
	rm -f ${PROG} *.o *.core
