LIBS=-lpthread
CC=gcc
CFLAGS=-Wall -g
INCLUDES=-I .
SRC=WheelTimer.o threadApi.o main.o LinkedListApi.o
TARGET:exe
%.o:%.c
	 ${CC} ${CFLAGS} -c ${INCLUDES} $<
exe:$(SRC)
	${CC} ${CFLAGS} $(SRC) -o exe $(LIBS)
clean:
	rm *.o
	rm exe

