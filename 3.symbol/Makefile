INC				:= -Iinc
SRC				:= src/main.c
SRC				+= src/qarr.c
SRC				+= src/qerr.c
SRC				+= src/qstr.c
SRC				+= src/qstack.c
SRC				+= src/symbol.c
SRC				+= src/lexer.c
SRC				+= src/parser.c

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
	./$(TARGET) example/and.c
	./$(TARGET) example/hello.c
	./$(TARGET) example/nest.c
	./$(TARGET) example/pair.c

dump:
	$(OBJDUMP) -dV $(TARGET) > elf_log

clean:
	rm -rf $(TARGET) elf_log

.PHONY: all check clean
