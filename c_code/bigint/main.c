#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CHUNK_SIZE 4
#define IN_NUMBER_LEN 39 //128 bit
#define IN_NUMBER_HEX_LEN 78 //128 bit


#define NUMBER_SIZE IN_NUMBER_LEN //16 * CHUNK_SIZE
#define OUT_NUMBER_LEN IN_NUMBER_LEN / CHUNK_SIZE + 1

char EMPTY_CHUNK[CHUNK_SIZE];
const short CARRY_THRESHOLD = 9999;//pow(10, CHUNK_SIZE) - 1; // calculate the carry threshold (chunk size = 4 -> 9999)

void reverse_array(uint16_t arr[], short n);
void print_bigint(uint16_t* bigint, short* len);

void str_to_bigint(char* number, uint16_t* int_array) {
    /* Unsigned 16 bit number range: 0 to 65535
    divide into 4 digits chunks (max value = 9999)
    */
    short chunk_num = 0, len = strlen(number);

    char chunk[CHUNK_SIZE];// number[strlen(number) - len];

  // get clean number, without leading zeros
  //memmove(number, number, strlen(number));


    short i;
    for (i = len - CHUNK_SIZE; i >= 0; i -= CHUNK_SIZE) { //loop trough number in reverse, divide it into CHUNK_SIZE chunks, convert to int and append to array
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
    reverse_array(int_array, OUT_NUMBER_LEN); // reverse the number
}

uint16_t* add_bigints(uint16_t bigint1[], uint16_t bigint2[], short* len3) {
    short b1 = 0, b2 = 0;
    short i, carry = 0, sum = 0, out_len = 1, chr_chunk_len = 0;

    char chr_num[NUMBER_SIZE + 1] = "";

    {
        char chr_chunk[CHUNK_SIZE];
        for (i = OUT_NUMBER_LEN - 1; i >= 0; i--) { // loop trough array and add numbers
            b1 = bigint1[i], b2 = bigint2[i];
            b1 = b1 < 0 ? 0 : b1; // if we are out of chunks -> set value to 0
            b2 = b2 < 0 ? 0 : b2;

            sum = b1 + b2 + carry;

            if (sum > CARRY_THRESHOLD) { // if sum is bigger than 9999 - add carry
                sum -= CARRY_THRESHOLD + 1;
                carry = 1;
            }
            else {
                carry = 0;
            }
            _itoa(sum, chr_chunk, 10); // convert sum to string
            chr_chunk_len = strlen(chr_chunk);

            strcat(chr_num, _strrev(chr_chunk)); // append to number

            // for (short j = 0; j < chr_chunk_len; j++) chr_num[j + out_len] = chr_chunk[j];

            // strncpy(chr_num + out_len, strrev(chr_chunk), CHUNK_SIZE); // append
            if (chr_chunk_len < CHUNK_SIZE) {
                // append zeros to chr_num using for loop
                for (short j = chr_chunk_len; j < CHUNK_SIZE; j++) strcat(chr_num, "0");
            }

            out_len += CHUNK_SIZE;
        }
    }
    if (carry == 1) { strcat(chr_num, "1"); out_len++; } // add carry if needed
    out_len--;

    uint16_t* bigint3[NUMBER_SIZE + 1];
    str_to_bigint(_strrev(chr_num), &bigint3);
    printf("bigint1 + bigint2 result = ");
    print_bigint(bigint3, NUMBER_SIZE + 1);

    *len3 = NUMBER_SIZE + 1;
  
    return bigint3; // return the result
}

void print_bigint(uint16_t* bigint, short* len) {
    /*print bigtint array and if item length is less than chunk_size then add leading zeros to print*/
    printf("%hu", bigint[0]);
    for (int i = 1; i < OUT_NUMBER_LEN; i++) printf("%0*hu", CHUNK_SIZE, bigint[i]);
    printf("\n");
}

void reverse_array(uint16_t arr[], short n) {
    uint16_t* aux = (uint16_t*)calloc(n, sizeof(uint16_t));
    for (int i = 0; i < n; i++) aux[n - 1 - i] = arr[i];
    for (int i = 0; i < n; i++) arr[i] = aux[i];
}

char hex_to_dec(char hex[], char text[]){
    int i = 0, j = 0;
    while(hex[i]){
        int up = '0' <= hex[i] && hex[i] <= '9' ? hex[i] - '0' : hex[i] - 'a' + 10;//lowcase
        if(hex[++i] == '\0'){
            return -1;
        }
        int low = '0' <= hex[i] && hex[i] <= '9' ? hex[i] - '0' : hex[i] - 'a' + 10;//lowcase
        text[j++] = up * 16 + low;
        ++i;
    }
    text[j] = 0;
    return text;
}

int main()
{
    snprintf(EMPTY_CHUNK, sizeof(EMPTY_CHUNK), "%0*i", CHUNK_SIZE, 0);

    /*Number 1*/
    // char number1[] = "340282366920938463463374607431768211455"; // 340282366920938463463374607431768211455 // FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
    char number1_hex[IN_NUMBER_HEX_LEN + 1];
    char number1[NUMBER_SIZE + 1];

    printf("Enter number1: ");
    gets(number1_hex);
    hex_to_dec(number1_hex, number1);

    printf("String number1 %s\n", number1); // or just number1 if reading from console

    uint16_t* bigint1[NUMBER_SIZE];
    str_to_bigint(number1, &bigint1);

    printf("bigint array1 ");
    print_bigint(bigint1, OUT_NUMBER_LEN);

    /*Number 2*/
    // char number2[] = "250026244553818629560840097906942367043"; // 250026244553818629560840097906942367043 // BC194D99B948CB2AA6DF6B892BB95543
    char number2_hex[IN_NUMBER_HEX_LEN + 1];
    char number2[NUMBER_SIZE + 1];

    printf("Enter number2: ");
    gets(number2_hex);
    hex_to_dec(number2_hex, number2);
    printf("String number2 %s\n", number2);

    uint16_t bigint2[NUMBER_SIZE];
    str_to_bigint(number2, &bigint2);
    printf("bigint array2 ");
    print_bigint(bigint2, NUMBER_SIZE);

    /*Add bigints*/
    short len_out = 0;
    uint16_t* result_addition;
    result_addition = add_bigints(bigint1, bigint2, &len_out);
    // printf("bigint1 + bigint2 result = ");
    // print_bigint(result_addition, NUMBER_SIZE + 1);
    return 0;
}
