#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CHUNK_SIZE 4

void reverse_array(uint16_t arr[], short n)
{
    int aux[n];
 
    for (int i = 0; i < n; i++) {
        aux[n - 1 - i] = arr[i];
    }
 
    for (int i = 0; i < n; i++) {
        arr[i] = aux[i];
    }
}

uint16_t* str_to_bigint(char *number, short* len) {
    /* Unsigned 16 bit number range: 0 to 65535 
    divide into 4 digits chunks (max value = 9999) 
    */
    uint16_t *int_array;
	  char chunk[CHUNK_SIZE];
    short chunk_num = 0;
    printf("number = |%s|\n", number);

    while(number[*len] == '0') { len++; } /*skip zeros*/

    strncpy(number, number + *len, strlen(number)); /*get clean number (without leading zeroes)*/

    *len = strlen(number);

    int_array = malloc(*len * sizeof(uint16_t)); /*allocate memory for array*/

    short i;
    for(i = *len - CHUNK_SIZE; i >= 0; i-=CHUNK_SIZE) { /*loop trough number in reverse, divide it into CHUNK_SIZE chunks, convert to int and append to array*/
        strncpy(chunk, number + i, CHUNK_SIZE);
        uint16_t int_chunk = atoi(chunk);
        int_array[chunk_num] = int_chunk;
        chunk_num++;
    }
    if (i < 0) { /* if the last chunk is smaller then CHUNK_SIZE*/
      memset(chunk, 0, CHUNK_SIZE);
      strncpy(chunk, number, i + CHUNK_SIZE);
      uint16_t int_chunk = atoi(chunk);
      int_array[chunk_num] = int_chunk;
      chunk_num++; 
    }
    
    reverse_array(int_array, chunk_num); /*reverse the number*/

    *len = chunk_num; /*update length*/

    return int_array;
}

uint16_t* add_bigints(uint16_t*bigint1, short* len1, uint16_t*bigint2, short* len2, short* len3) {
    short i, carry = 0, sum = 0, out_len = 0;
    short len = *len1 > *len2 ? *len1 : *len2; /*get max length*/
    short b1 = 0, b2 = 0, bi1 = 0, bi2 = 0;
    char chr_num[len + 1], chr_chunk[CHUNK_SIZE];

    for(i = len - 1; i >= 0; i--) { /*loop trough array and add numbers*/
      bi1 = i - (len - *len1); /*find the beggining of the array by calc. the diff. betweem max and this->len*/
      bi2 = i - (len - *len2);
      b1 = bigint1[bi1], b2 = bigint2[bi2];
      b1 = bi1 < 0 || b1 < 0 ? 0 : bigint1[bi1];
      b2 = bi2 < 0 || b2 < 0 ? 0 : bigint2[bi2]; 
      
      sum = b1 + b2 + carry;

      if(sum > 9999) { /*if sum is bigger than 9999 - add carry*/
          sum -= 10000;
          carry = 1;
      } else {
          carry = 0;
      }
      itoa(sum, chr_chunk, 10); /*convert sum to string*/
      strcat(chr_num, strrev(chr_chunk));
      out_len += strlen(chr_chunk);
    }

    if (carry == 1) strcat(chr_num, "1"); /*add carry if needed*/

    char out_num[out_len];
    strncpy(out_num, strrev(chr_num), out_len);
    out_num[out_len] = '\0';

    uint16_t* bigint3 = str_to_bigint(out_num, len3);
    return bigint3;
}

void print_bigint(uint16_t *bigint, short* len) {
    /*print bigtint array chunks*/
    for (int i = 0; i < *len; i++) printf("%hu,", (uint16_t)bigint[i]);
		printf("\n");
}


int main()
{
    /*Number 1*/
		/*char number1[] = "367451236"  "115792089237316195423570985008687907853269984665640564039457584007913129639936";*/
		char number1[] = "87907853269984665640564039457584007913129639936";

		printf("String number1 %s\n", number1);
    short len1 = 0;

		uint16_t* bigint1 = str_to_bigint(number1, &len1);

		printf("bigint array1 ");
		print_bigint(bigint1, &len1);

    /*Number 2*/
    /*char number2[] = "8153786" "912415373636592080147267228649611544136934419016527019426904852909558630064154";*/
    char number2[] = "1544136934419016527019426904852909558630064154";

		printf("String number2 %s\n", number2);
    short len2 = 0;

		uint16_t* bigint2 = str_to_bigint(number2, &len2);

		printf("bigint array2 ");
		print_bigint(bigint2, &len2);

    /*Add bigints*/
    short len3 = 0;
    uint16_t* result_addition = add_bigints(bigint1, &len1, bigint2, &len2, &len3);
    printf("bigint1 + bigint2= ");
		print_bigint(result_addition, &len3);
    return 0;
}
