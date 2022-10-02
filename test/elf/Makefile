elfdump: elfdump.c
	gcc -o elfdump elfdump.c

# Some test files used to run the benchmark
test.obj: elfdump.c
	gcc -O0 -m32 -c -o test.obj elfdump.c
test.elf: elfdump.c
	gcc -O0 -m32 -o test.elf elfdump.c

# Runs the benchmarks
run.obj: elfdump test.obj
	./elfdump test.obj
run.elf: elfdump test.elf
	./elfdump test.elf

# Debug the benchmarks
debug.obj: elfdump test.obj
	gdb ./elfdump test.obj
debug.elf: elfdump test.elf
	gdb ./elfdump test.elf

# readelf the benchmarks
read.obj: test.obj
	readelf -e -s test.obj --wide | tr -s ' ' | sed 's/^ //; s/\[\s*/\[/g'
read.elf: test.elf
	readelf -e -s test.elf --wide | tr -s ' ' | sed 's/^ //; s/\[\s*/\[/g'
