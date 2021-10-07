# Config
override CFLAGS  += -Iinclude
override LDFLAGS += -lraylib -lm -lzip

OUT := hive
#

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

all: ${BLD_DIR} ${OUT}

%.o: %.c
	${CC} -o $@ -c $< ${CFLAGS}

${OUT}: ${OBJS}
	${CC} -o $@ ${OBJS} ${LDFLAGS}

debug:
	${MAKE} -B CFLAGS="-g -O0"

clean:
	rm -f ${OBJS} ${OUT}

.PHONY: clean debug
