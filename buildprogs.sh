#!/bin/bash

echo "#include \"../inc/elf_helpers.h\""
echo

for FILE in `ls progs`
do
    echo "extern uint32_t _binary_progs__${FILE}_start[];"
    echo "extern uint32_t _binary_progs__${FILE}_size[];"
done

echo
echo "program_t programs[] = {"

for FILE in `ls progs`
do
    echo "    {"
    echo "        .file = _binary_progs__${FILE}_start,"
    echo "        .size = (uint32_t) _binary_progs__${FILE}_size,"
    echo "        .name = \"${FILE}\","
    echo '    },'
done

echo "};"
echo
echo "long programs_size = sizeof(programs)/sizeof(program_t);"
