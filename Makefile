GCCFLAGS = 
SOURCES = xml-parser.c query-executor.c query.c node-collection.c util.c string.c parsers/*.c
TEST_SOURCES = $(SOURCES) ./lib/libtap/tap.c

debug: GCCFLAGS += -o debug/cxpq -Og -g -fsanitize=address -fno-omit-frame-pointer -DDEBUG
debug: cxpq

release: GCCFLAGS += -O3 -o release/cxpq
release: cxpq

cxpq:
	$(CC) $(GCCFLAGS) cxpq.c $(SOURCES)

clean:
	rm -f release/cxpq debug/cxpq

test: GCCFLAGS += -o tests/run-tests -Og -g -fsanitize=address -fno-omit-frame-pointer -DDEBUG
test:
	$(CC) $(GCCFLAGS) tests/tests.c $(TEST_SOURCES)
	./tests/run-tests
