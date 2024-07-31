**C** **X**ML **P**arser and **Q**uery

A simple C program for parsing XML files (with partial validation - see below) and querying them.

This is still very much work in progress, parsing is still not complete & work on querying has not even started.

## Setup

### Nix
For [Nix](https://github.com/NixOS/nix) users, there is a flake.nix for setting up a dev-shell.

A simple `nix develop` should probably just work, or just use [direnv](https://direnv.net/).

### Others

Having CMake & gcc should be enough.

## Building

### Debug

`-DCMAKE_EXPORT_COMPILE_COMMANDS=YES` is optional, but it enables usage of CCLS for developing.

```shell
$ cd debug
$ cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ..
$ cmake --build .
$ ./cxpq_debug ../examples/bookstore.xml
```

### Release

```shell
$ cd release
$ cmake ..
$ cmake --build .
$ ./cxpq ../examples/bookstore.xml
```

## Features
 - [ ] Parsing
   - [x] Prolog (currently it gets skipped)
   - [ ] DTD
   - [x] \(Root\) Node
     - [x] Attributes
     - [x] Namespaces
     - [x] Content
     - [ ] CDATA (planned to be just included in content)
   - [x] Comments (currently get skipped)
   - [ ] Processing Instructions
 - [ ] Validation
   - [ ] Valid file (no unclosed tags, comments, etc.)
   - [ ] Valid Prolog
   - [x] Valid Root Node name
   - [ ] Valid Node name (partially, missing `xml` "ban")
   - [ ] Namespaces (currenly they just get included in node name)
 - [ ] Querying
   - [ ] XPath?
   - [ ] Custom query language (CSS selector like)
