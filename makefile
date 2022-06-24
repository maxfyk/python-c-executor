all:
	gcc -o bigint c_code/bigint/main.c
arm:
	arm-none-eabi-gcc -c -mthumb  -g -Wall -mcpu=cortex-m4 -o F:/bigint c_code/bigint/main.c