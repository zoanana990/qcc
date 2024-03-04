INC				:= -Iinc
SRC				:= src/qcc.c src/qarr.c src/qerr.c src/qstr.c src/qlex.c src/parser.c

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

dump:
	$(OBJDUMP) -dV $(TARGET) > elf_log

clean:
	rm -rf $(TARGET) elf_log

.PHONY: all check clean
