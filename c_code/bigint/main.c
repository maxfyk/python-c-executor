#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CHUNK_SIZE 4

uint16_t* str_to_bigint(char *number, short* len) {
    /* Unsigned 16 bit number range: 0 to 65535 
    divide into 4 digits chunks (max value = 9999) 
    */
    uint16_t *int_array;
	  char chunk[4];
    short chunk_num = 0;
    
    while(number[*len] == '0') { len++; } /*skip zeros*/

    strncpy(number, number + *len, strlen(number)); /*get clean number (without leading zeroes)*/

    *len = strlen(number);

    int_array = malloc(*len * sizeof(uint16_t)); /*allocate memory for array*/

    short i;
    for(i = 0; i < *len; i+=CHUNK_SIZE) { /*loop trough number, divide it into CHUNK_SIZE chunks, convert to int and append to array*/
        strncpy(chunk, number + i, CHUNK_SIZE);
        int_array[chunk_num] = atoi(chunk);
        chunk_num++;
    }

    *len = chunk_num; /*update length*/

    return int_array;
}

void print_bigint(uint16_t *bigint, short* len) {
    /*print bigtint array chunks*/
    for (int i = 0; i < *len; i++) printf("%hu,", (uint16_t)bigint[i]);
		printf("\n");
}


int main()
{
		char number[] = "115792089237316195423570985008687907853269984665640564039457584007913129639936";

		printf("String number1 %s\n", number);
    short len1 = 0;

		uint16_t* bigint1 = str_to_bigint(number, &len1);

		printf("bigint array1 ");
		print_bigint(bigint1, &len1);
    return 0;
}
