#!/bin/sh

for x in *.c
do
    exe=${x%%.c}
    rm -f "$exe" "$exe.o"

    gcc -nostdlib -nostdinc -nostartfiles -nodefaultlibs -c "$exe.c"
    ld -e main "$exe.o" -o "$exe"
done
