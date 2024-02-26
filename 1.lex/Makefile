INC				:= -Iinc
SRC				:= $(wildcard src/*.c)

CC				:= gcc
CFLAGS			:= $(INC)
TARGET			:= qcc

all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

check:
	./$(TARGET) example/hello.c
# ./$(TARGET) example/add.c

clean:
	rm -rf $(TARGET)

.PHONY: all check clean
