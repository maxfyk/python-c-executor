#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define CHUNK_SIZE 4
char EMPTY_CHUNK[CHUNK_SIZE];
const short CARRY_THRESHOLD = pow(10, CHUNK_SIZE)  -1; // calculate the carry threshold (chunk size = 4 -> 9999)

void reverse_array(uint16_t arr[], short n);


uint16_t* str_to_bigint(char *in_num, short* len) {
    /* Unsigned 16 bit number range: 0 to 65535 
    divide into 4 digits chunks (max value = 9999) 
    */
    while(in_num[*len] == '0') *len+=1; //skip leading zeros

    uint16_t *int_array;
    short chunk_num = 0;
	  char number[strlen(in_num) - *len], chunk[CHUNK_SIZE];
    number[strlen(in_num) - *len] = '\0';
    
    // get clean number, without leading zeros
    memmove(number, in_num + *len, strlen(in_num) - *len);

    *len = strlen(number);

    int_array = malloc(*len * sizeof(uint16_t)); //allocate memory for array

    short i;
    for(i = *len - CHUNK_SIZE; i >= 0; i-=CHUNK_SIZE) { //loop trough number in reverse, divide it into CHUNK_SIZE chunks, convert to int and append to array
        memset(chunk, 0, CHUNK_SIZE); // clear old chunk
        strncpy(chunk, number + i, CHUNK_SIZE); // copy new one
        uint16_t int_chunk = chunk == EMPTY_CHUNK ? 0 : atoi(chunk); // convert chars to int (if char consists of only zeros = 0)
        int_array[chunk_num] = int_chunk; // add to bigint array
        chunk_num++; 
    }
    if (i < 0) { // if the last chunk is smaller then CHUNK_SIZE and was skipeed
      memset(chunk, 0, CHUNK_SIZE);
      strncpy(chunk, number, i + CHUNK_SIZE);
      uint16_t int_chunk = chunk == EMPTY_CHUNK ? 0 : atoi(chunk); // convert chars to int (if char consists of only zeros = 0)
      int_array[chunk_num] = int_chunk;
      chunk_num++; 
    }
    
    reverse_array(int_array, chunk_num); // reverse the number

    *len = chunk_num; // update length

    return int_array;
}

uint16_t* add_bigints(uint16_t*bigint1, short* len1, uint16_t*bigint2, short* len2, short* len3) {
    short b1 = 0, b2 = 0, bi1 = 0, bi2 = 0;
    short i, carry = 0, sum = 0, out_len = 0;

    short len = *len1 > *len2 ? *len1 : *len2; //get max length

    char chr_num[len * CHUNK_SIZE + 1], chr_chunk[CHUNK_SIZE];
    chr_num[0] = '\0';

    for(i = len - 1; i >= 0; i--) { // loop trough array and add numbers
      bi1 = i - (len - *len1); // find the beggining of the array by calc. the diff. betweem max len and this->len
      bi2 = i - (len - *len2);
      b1 = bigint1[bi1], b2 = bigint2[bi2];
      b1 = bi1 < 0 || b1 < 0 ? 0 : bigint1[bi1]; // if we are out of chunks -> set value to 0
      b2 = bi2 < 0 || b2 < 0 ? 0 : bigint2[bi2]; 
      
      sum = b1 + b2 + carry;

      if(sum > CARRY_THRESHOLD) { // if sum is bigger than 9999 - add carry
          sum -= CARRY_THRESHOLD + 1;
          carry = 1;
      } else {
          carry = 0;
      }
      itoa(sum, chr_chunk, 10); // convert sum to string
      strcat(chr_num, strrev(chr_chunk)); // append to number

      if(strlen(chr_chunk) < CHUNK_SIZE) {
        // append zeros to chr_num using for loop
        short chr_num_len = strlen(chr_num);
        for(short j = chr_num_len; j < chr_num_len + CHUNK_SIZE - strlen(chr_chunk); j++) strcat(chr_num, "0");
      }

      out_len += CHUNK_SIZE;
    }

    if (carry == 1) strcat(chr_num, "1"); out_len+=1; // add carry if needed
    
    strncpy(chr_num, strrev(chr_num), out_len); // copy only the needed part of the number (without random symbols)

    return str_to_bigint(chr_num, len3);
}

void print_bigint(uint16_t *bigint, short* len) {
    /*print bigtint array and if item length is less than chunk_size then add leading zeros to print*/
    printf("%hu,", bigint[0]);
    for(int i = 1; i < *len; i++) printf("%0*hu,", CHUNK_SIZE, bigint[i]);
		printf("\n");
}

void reverse_array(uint16_t arr[], short n) {
    int aux[n];
    for (int i = 0; i < n; i++) aux[n - 1 - i] = arr[i];
    for (int i = 0; i < n; i++) arr[i] = aux[i];
}

int main()
{
    snprintf(EMPTY_CHUNK, sizeof(EMPTY_CHUNK), "%0*i", CHUNK_SIZE, 0);
    /*Number 1*/
		char number1[] = "115792089237316195423570985008687907853269984665640564039457584007913129639936";
		// char number1[] = "5000000000000000000000000000000000000000000000";

		printf("String number1 %s\n", number1);
    short len1 = 0;

		uint16_t* bigint1 = str_to_bigint(number1, &len1);

		printf("bigint array1 ");
		print_bigint(bigint1, &len1);

    /*Number 2*/
    char number2[] = "912415373636592080147267228649611544136934419016527019426904852909";
    // char number2[] = "5000000000000000000000000000000000000000000000";

		printf("String number2 %s\n", number2);
    short len2 = 0;

		uint16_t* bigint2 = str_to_bigint(number2, &len2);

		printf("bigint array2 ");
		print_bigint(bigint2, &len2);

    /*Add bigints*/
    short len3 = 0;
    uint16_t* result_addition = add_bigints(bigint1, &len1, bigint2, &len2, &len3);
    printf("bigint1 + bigint2 = ");
		print_bigint(result_addition, &len3);
    return 0;
}
