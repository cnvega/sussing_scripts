CC = gcc
CCFLAGS = -std=c99 -Wno-unused-result -lm
CCFLAGS += -O2 
#CCFLAGS += -g

all: select_forests extract_forests extract_forests_block split_forests split_forests_nf catalog_check

.PHONY: clean

%.o: %.c
	$(CC) -c $< -o $@ $(CCFLAGS)

select_forests: select_forests.o
	$(CC) select_forests.o -o select_forests $(CCFLAGS)

extract_forests: extract_forests.o forests.o
	$(CC) forests.o extract_forests.o -o extract_forests $(CCFLAGS)

extract_forests_block: extract_forests_block.o forests.o
	$(CC) forests.o extract_forests_block.o -o extract_forests_block $(CCFLAGS)

split_forests: split_forests.o forests.o
	$(CC) forests.o split_forests.o -o split_forests $(CCFLAGS)

split_forests_nf: split_forests_nf.o forests.o
	$(CC) forests.o split_forests_nf.o -o split_forests_nf $(CCFLAGS)

catalog_check: catalog_check.o forests.o
	$(CC) forests.o catalog_check.o -o catalog_check $(CCFLAGS)

clean:
	rm -f select_forests extract_forests extract_forests_block split_forests split_forests_nf catalog_check *.o

