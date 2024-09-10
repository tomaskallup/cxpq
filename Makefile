CCFLAGS = -std=c99 -pedantic -Wall
SOURCES = xml-parser.c query-executor.c query.c node-collection.c util.c string.c parsers/*.c
TEST_SOURCES = $(SOURCES) ./lib/libtap/tap.c

debug: CCFLAGS += -o debug/cxpq -Og -g -fsanitize=address -fno-omit-frame-pointer -DDEBUG
debug: cxpq

release: CCFLAGS += -O3 -o release/cxpq
release: cxpq

cxpq: cxpq.c cli-parser.c $(SOURCES)
	$(CC) $(CCFLAGS) cxpq.c cli-parser.c $(SOURCES)

clean:
	rm -f release/cxpq debug/cxpq tests/run-tests

test: CCFLAGS += -o tests/run-tests -Og -g -fsanitize=address -fno-omit-frame-pointer -DDEBUG
test: $(TEST_SOURCES) tests/**/*.c
	$(CC) $(CCFLAGS) tests/tests.c $(TEST_SOURCES)
	./tests/run-tests
