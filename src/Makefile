CC = gcc
CCFLAGS = -O2 -std=c99 -Wno-unused-result -lm
#CCFLAGS += -g

all: select_forests extract_forests split_forests 

.PHONY: clean

%.o: %.c
	$(CC) -c $< -o $@ $(CCFLAGS)

select_forests: select_forests.o
	$(CC) select_forests.o -o select_forests $(CCFLAGS)

extract_forests: extract_forests.o forests.o
	$(CC) forests.o extract_forests.o -o extract_forests $(CCFLAGS)

split_forests: split_forests.o forests.o
	$(CC) forests.o split_forests.o -o split_forests $(CCFLAGS)

clean:
	rm -f select_forests extract_forests split_forests *.o

