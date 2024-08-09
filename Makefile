GCCFLAGS = 
SOURCES = cxpq.c xml-parser.c util.c string.c parsers/*.c

debug: GCCFLAGS += -o debug/cxpq -Og -g -fsanitize=address -fno-omit-frame-pointer -DDEBUG
debug: cxpq

release: GCCFLAGS += -O3 -o release/cxpq
release: cxpq

cxpq:
	$(CC) $(GCCFLAGS) $(SOURCES)

clean:
	rm -f release/cxpq debug/cxpq
