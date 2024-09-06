CCFLAGS = 
SOURCES = xml-parser.c query-executor.c query.c node-collection.c util.c string.c parsers/*.c
TEST_SOURCES = $(SOURCES) ./lib/libtap/tap.c

debug: CCFLAGS += -o debug/cxpq -g -fsanitize=address -fno-omit-frame-pointer -DDEBUG -Wall
debug: cxpq

release: CCFLAGS += -O3 -o release/cxpq -Wall
release: cxpq

cxpq: cxpq.c cli-parser.c $(SOURCES)
	$(CC) $(CCFLAGS) cxpq.c cli-parser.c $(SOURCES)

clean:
	rm -f release/cxpq debug/cxpq tests/run-tests

test: CCFLAGS += -o tests/run-tests -g -fsanitize=address -fno-omit-frame-pointer -DDEBUG -Wall
test: $(TEST_SOURCES) tests/**/*.c
	$(CC) $(CCFLAGS) tests/tests.c $(TEST_SOURCES)
	./tests/run-tests
