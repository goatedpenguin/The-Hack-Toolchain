The Hack VM translator in C, for nand2tetris projects 7 and 8.

Build:

```sh
gcc -std=c11 -O2 -Wall -Wextra src/*.c -o hackvm
```

Usage:

```sh
./hackvm path/to/File.vm
./hackvm path/to/Program
```

A file produces `File.asm` beside the input. A directory produces
`Program/Program.asm` from its `.vm` files, in filename order.
Directory output starts with `SP = 256` and a call to `Sys.init`, so the
program needs a `Sys.init` function. Single files have no bootstrap code,
for the project 7 and early project 8 tests.

Run the local checks with `bash tests/run_tests.sh`. To also run the supplied
project 7/8 tests and the CPU regression tests, pass the path to the
[nand2tetris software suite](https://www.nand2tetris.org/software):

```sh
bash tests/run_tests.sh /path/to/nand2tetris
```

Specification: [project 7](https://www.nand2tetris.org/project07) and
[project 8](https://www.nand2tetris.org/project08).
