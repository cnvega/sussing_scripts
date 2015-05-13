all: pipelines

.PHONY: clean

pipelines:
	$(MAKE) -C src
	mv src/select_forests .
	mv src/extract_forests .
	mv src/split_forests .

clean:
	rm -f select_forests extract_forests split_forests src/*.o

