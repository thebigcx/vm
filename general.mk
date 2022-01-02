SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

CFLAGS = -Wall -Wextra -Werror=implicit-function-declaration -Wno-unused-function -g -I../include
LDFLAGS =

.PHONY: all clean

all: $(TARG)

$(TARG): $(OBJ)
	@mkdir -p $(shell dirname $(TARG))
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(TARG) $(OBJ)
	@rmdir $(shell dirname $(TARG))
