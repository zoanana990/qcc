INC				:= -Iinc
SRC				:= $(wildcard src/*.c)

CC				:= gcc
OBJDUMP			:= otool
CFLAGS			:= $(INC)
TARGET			:= qcc

all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

check:
	./$(TARGET) example/basic.c
	./$(TARGET) example/add.c
	./$(TARGET) example/hello.c
	./$(TARGET) example/and.c

disasem:
	$(OBJDUMP) -dV $(TARGET) > elf_log

clean:
	rm -rf $(TARGET) elf_log

.PHONY: all check clean
