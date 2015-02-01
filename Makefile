# names and paths
TARGET_LIB = libhilg.so
SRC_DIR = src
OBJS = $(SRC_DIR)/hilg.o
BIN_DIR = bin
EXAMPLES = $(BIN_DIR)/box_mover $(BIN_DIR)/snake

# tools and flags
CC = gcc
CFLAGS = -I ./include -fPIC -Wall -Wextra -O2
LDFLAGS = -L . -Wl,-rpath=. -lncurses -lhilg
RM = rm -f

# rules
$(BIN_DIR)/%: examples/%.c
	mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

.PHONY: all
all: ${TARGET_LIB} ${EXAMPLES}

$(TARGET_LIB): $(OBJS)
	$(CC) -shared -o $@ $^

.PHONY: clean
clean:
	${RM} ${TARGET_LIB} ${OBJS} ${EXAMPLES}
	${RM} -r $(BIN_DIR)
