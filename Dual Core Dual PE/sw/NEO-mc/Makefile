all : main.c bootstrap.S
	riscv32-unknown-elf-gcc main.c bootstrap.S -o main -nostartfiles -march=rv32ima -mabi=ilp32
	
sim: all
	$(EE6470)/riscv-vp/vp/build/bin/riscv-vp-acc  --intercept-syscalls main

dump-elf: all
	riscv32-unknown-elf-readelf -a main
	
dump-code: all
	riscv32-unknown-elf-objdump -D main
	
dump-comment: all
	objdump -s --section .comment main
	
clean:
	rm -f main
