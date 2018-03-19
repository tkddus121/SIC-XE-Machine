CC = gcc
CFLAGS = -g -c -Wall

OBJECTS = 20141549.o CommandRead.o Commands.o
SRCS = 20141549.c CommandRead.c Commands.c

TARGET = 20141549.out

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

20141549.o : 20141549.h 20141549.c
	$(CC) $(CFLAGS) -c 20141549.c
CommandRead.o : 20141549.h CommandRead.c
	$(CC) $(CFLAGS) -c CommandRead.c
Commands.o : 20141549.h Commands.c
	$(CC) $(CFLAGS) -c Commands.c

clean :
	rm -rf $(OBJECTS) $(TARGET)
