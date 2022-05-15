#include <stdio.h>

int main()
 {
     char op;
     double num1, num2;

     while (1)
     {

         printf("\nEnter number1:");
         scanf("%lf", &num1);

         printf("Enter number2:");
         scanf("%lf", &num2);

         printf("Enter operator:");
         scanf("\n%c", &op);

         switch(op)
         {
           case '+':
             printf("%lf", num1+num2);
             break;
           case '-':
             printf("%lf", num1-num2);
             break;
           case '*':
             printf("%lf", num1*num2);
             break;
           case '/':
             printf("%lf", num1/num2);
             break;
           case 'E':
             return 0;
             break;
           default:
             printf("Error! Invalid Operator.");
         }

     }

     return 0;
 }