REPO	:= $(shell pwd)
SRC 	:= $(REPO)/src
INC 	:= $(REPO)/inc

INCLUDE	:= -I$(INC)
SOURCES	:= $(wildcard $(SRC)/*.c)

TARGET	:= qcc

CC		:= gcc
CFLAGS	:= $(INCLUDE) -g -Wall -Werror


all: $(TARGET) force

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

clean: force
	rm -rf $(TARGET)

check: force
	./$(TARGET) example/1
	./$(TARGET) example/2
	./$(TARGET) example/3

force:

.PHONY: all clean force