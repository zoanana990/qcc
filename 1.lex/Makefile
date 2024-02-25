CC				:= gcc
CFLAGS			:= 
TARGET			:= qcc

INC				:= -Iinc
SRC				:= $(wildcard scr/*.c)

all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGET)

.PHONY: all clean