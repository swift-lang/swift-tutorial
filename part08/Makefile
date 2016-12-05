CC?=mpicc

CFLAGS = -Wall -g

SRC = $(wildcard *.c)

DEPS = 
OBJS = $(SRC:.c=.o)

EXEC = hipi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC) : $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXEC) *~
