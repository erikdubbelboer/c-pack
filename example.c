
/* Usage: ./example 1337 | ./example
 */


#include <stdio.h>
#include <stdlib.h>  /* atoi() */

#include "pack.h"


int main(int argc, char* argv[]) {
  if (argc < 2) {
    /* Reading. */
    char buffer[10];
    int  n = 0;

    while (!feof(stdin) && (n < 10)) {
      n += fread(&buffer[n], 1, 1, stdin);
    }

    printf("%d bytes read\n", n);

    /* Create an unpacker for the data we just read. */
    PACKED(p, buffer, n);

    /* Unpack the number. */
    int32_t num = unpack_int32_t(&p);

    if (p.error) {
      printf("error unpacking\n");
    } else {
      printf("the number was: %d\n", num);
    }
  } else {
    /* Writing. */
    int32_t num = atoi(argv[1]);

    /* Create a buffer of 10 bytes. */
    PACKER(p, 10);

    /* Pack the number. */
    pack_int32_t(&p, num);
    pack_finish(&p);

    fwrite(p.data, p.length, 1, stdout);
  }

  return 0;
}

