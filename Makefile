all: pipelines

.PHONY: clean

pipelines:
	$(MAKE) -C src
	mv src/select_forests .
	mv src/extract_forests .
	mv src/split_forests .
	mv src/catalog_check .

clean:
	rm -f select_forests extract_forests split_forests catalog_check src/*.o

