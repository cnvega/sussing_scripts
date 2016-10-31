all: pipelines

.PHONY: clean

pipelines:
	$(MAKE) -C src
	mv src/select_forests .
	mv src/extract_forests .
	mv src/extract_forests_block .
	mv src/split_forests .
	mv src/catalog_check .

clean:
	rm -f select_forests extract_forests extract_forests_block split_forests catalog_check src/*.o

