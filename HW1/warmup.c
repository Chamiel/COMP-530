/* 
 * Michael Huang
 * COMP 530
 * Homework 1
 * UNC Honor Pledge: I certify that no unauthorized assistance has been received or
 * given in the completion of this work.
 */
#include <stdio.h>

void main()
{
  #define TRUE 1
  #define FALSE 0
  #define CHAR_LINE 80
  int i = 0;
  int ast = FALSE;
  char first = -1;

  do {
    char input[82];
    input[80] = '\n';
    input[81] = '\0';
    i = 0;

    if (first != -1)
    {
      input[0] = first;
      first = -1;
      i++;
    }

    for (i; i<CHAR_LINE; i++)
    {

      if (scanf("%c", &input[i]) == EOF) /* end program at EOF char */
        return;

      if (input[i] == '\n') /* replace new line with space */
        input[i] = ' ';

      if (input[i] == '*')
      {
        if (ast == TRUE)
        {
          input[i-1] = '^'; /* if double asterisk found then move back one char, replace with ^ */
          i--;
          ast = FALSE;
        } else
          ast = TRUE;
      } else
        ast = FALSE;
    }

    if (ast == TRUE) { /* if last character is asterisk, check if next character makes a double asterisk */
      if (scanf("%c", &first) == EOF)
        return;
      if (first == '*')
      {
        input[79] = '^';
        first = -1;
      }
      ast == FALSE;
    }

    printf("%s", input);
  } while(TRUE); /* loop infinitely until EOF char */
}
