OBJS	= main.o
SOURCE	= main.c
HEADER	= 
OUT	= main.out
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c -lcrypto


clean:
	rm -f $(OBJS) $(OUT)