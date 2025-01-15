# YAL

Usage: yal [OPTIONS]

A brief description of what myapp does.

## Options:

-i, --input <file>     Specify the input file to process.

-l, --logs             Write bytecode in yal_bytecode.txt and show logs

-t, --time             Show execution time.

-O2                    Add optimization

-h, --help             Display this help message

## Examples:
yal --input program.yal

yal -O2 --time --logs --input program.yal