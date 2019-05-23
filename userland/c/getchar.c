/* Get on character from stdin, and then print it back out.
 *
 * Same as getc(stdin).
 *
 * You have to press enter for the character to go through:
 * https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar
 *
 * Used at:
 * https://stackoverflow.com/questions/556405/what-do-real-user-and-sys-mean-in-the-output-of-time1/53937376#53937376
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char c;
    printf("enter a character: ");
    c = getchar();
    printf("you entered: %c\n", c);
    return EXIT_SUCCESS;
}
