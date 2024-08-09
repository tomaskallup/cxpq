**C** **X**ML **P**arser and **Q**uery

A simple C program for parsing XML files (with partial validation - see below) and querying them.

This is still very much work in progress, parsing is still not complete & work on querying has not even started.

## Setup

### Nix
For [Nix](https://github.com/NixOS/nix) users, there is a flake.nix for setting up a dev-shell.

A simple `nix develop` should probably just work, or just use [direnv](https://direnv.net/).

### Others

Having GNU Make and a C compiler (clang, gcc ..., make sure $CC is set) should be enough.
If you use clang, make sure to set `ASAN_SYMBOLIZER_PATH` to `which llvm-symbolizer` (it needs to be installed - usually via `libllvm`).

## Building

### Debug

```shell
$ make debug
$ ./debug/cxpq_debug ./examples/bookstore.xml
```

### Release

```shell
$ make realease
$ ./release/cxpq ../examples/bookstore.xml
```

## Features
 - [ ] Parsing
   - [x] Prolog (attributes get skipped)
   - [x] DTD (content is just skipped)
   - [x] \(Root\) Node
     - [x] Attributes
     - [x] Namespaces
     - [x] Content
     - [x] CDATA (parsed as text content, with a flag)
   - [x] Comments (currently get skipped)
   - [x] Processing Instructions
 - [ ] Validation
   - [ ] Valid file (no unclosed tags, comments, etc.)
   - [ ] Valid Prolog
   - [x] Valid Root Node name
   - [ ] Valid Node name (partially, missing `xml` "ban")
   - [ ] Namespaces (currenly they just get included in node name)
 - [ ] Querying
   - [ ] XPath?
   - [ ] Custom query language (CSS selector like)
