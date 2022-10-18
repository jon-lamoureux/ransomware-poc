OBJS	= main.o
SOURCE	= main.c
HEADER	= 
OUT	= main.out
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 
LDLIBS	 = -lcrypto 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) $(LDLIBS)

main.o: main.c
	$(CC) $(FLAGS) main.c $(LDLIBS)


clean:
	rm -f $(OBJS) $(OUT)