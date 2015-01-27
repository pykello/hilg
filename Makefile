TARGET_LIB = libhilg.so

CC = gcc
CFLAGS = -I . -fPIC -Wall -Wextra -O2
LIB_LDFLAGS = -shared
LDFLAGS = -L . -Wl,-rpath=. -lncurses -lhilg
RM = rm -f

SRCS = hilg.c
OBJS = $(SRCS:.c=.o)

EXAMPLES = box_mover

.PHONY: all
all: ${TARGET_LIB} ${EXAMPLES}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LIB_LDFLAGS} -o $@ $^

.PHONY: clean
clean:
	${RM} ${TARGET_LIB} ${OBJS} ${EXAMPLES}
