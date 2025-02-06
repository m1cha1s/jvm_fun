CC := cc
CFLAGS := -O0 -g
LDFLAGS := -fsanitize=address -g

EXE := jvm_fun
SRC := main.c
OBJ := $(SRC:.c=.o)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)

.PHONY: test clean

test: $(EXE)
	make -j -C test
	./$(EXE) test/Hello.class

clean:
	rm -rf $(OBJ)
	make -C test clean
