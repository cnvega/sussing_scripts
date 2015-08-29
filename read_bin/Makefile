CC = gcc
CCFLAGS = -O2 -std=c99 -Wno-unused-result -lm
#CCFLAGS += -g

all: rbin_catalog rbin_forests rbin_mtree

.PHONY: clean

%.o: %.c
	$(CC) -c $< -o $@ $(CCFLAGS)

rbin_catalog: rbin_catalog.o forests.o
	$(CC) forests.o rbin_catalog.o -o rbin_catalog $(CCFLAGS)

rbin_forests: rbin_forests.o forests.o
	$(CC) forests.o rbin_forests.o -o rbin_forests $(CCFLAGS)

rbin_mtree: rbin_mtree.o forests.o
	$(CC) forests.o rbin_mtree.o -o rbin_mtree $(CCFLAGS)

clean:
	rm -f *.o rbin_catalog rbin_forests rbin_mtree

