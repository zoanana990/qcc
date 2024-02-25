INC				:= -Iinc
SRC				:= $(wildcard src/*.c)

CC				:= gcc
CFLAGS			:= $(INC)
TARGET			:= qcc

all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET)

.PHONY: all clean
