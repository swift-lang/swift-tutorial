CC=mpicc

CFLAGS = -Wall -g

SRC = $(wildcard *.c)

DEPS = mandelbrot.h
OBJS = $(SRC:.c=.o)

EXEC=mandelbrot

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC) : $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXEC) *~ *out *err
