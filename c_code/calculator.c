#include <stdio.h>

int main()
 {
     char op;
     double num1, num2;

     printf("Enter number1:\n");
     scanf("%lf", &num1);

     printf("Enter number2:\n");
     scanf("%lf", &num2);

     printf("Enter operator:\n");
     scanf("\n%c", &op);

     switch(op)
     {
       case '+':
         printf("Result: %lf", num1+num2);
         break;
       case '-':
         printf("Result: %lf", num1-num2);
         break;
       case '*':
         printf("Result: %lf", num1*num2);
         break;
       case '/':
         printf("Result: %lf", num1/num2);
         break;
       default:
         printf("Error! Invalid Operator.");
     }

     return 0;
 }