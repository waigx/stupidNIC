CC = gcc
CFLAGS = -Iinclude -Wall -Werror -g3

TARGETS = $(patsubst ./%.c, %, $(wildcard ./*.c))


.PHONY: all binary

all: $(TARGETS)

$(TARGETS): $(patsubst lib/%.c, obj/%.o, $(wildcard lib/*.c))
	@$(MAKE) --no-print-directory BIN=$@ binary

binary: $(BIN).c
	$(CC) $(CFLAGS) -o $(BIN) $< obj/*.o

obj/%.o: lib/%.c include/%.h
	@mkdir -p obj
	$(CC) -c $(CFLAGS) -o $@ $<


.PHONY: clean

clean:
	rm -rf $(TARGETS) obj

