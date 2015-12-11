CC = gcc
CFLAGS = -Iinclude -Werror

TARGET = hello_postman


.PHONY: all binary clean

default: all


all: 
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c


clean:
	rm hello_postman
