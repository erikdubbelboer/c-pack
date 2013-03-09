
// Usage: ./example 1337 | ./example


#include <stdio.h>
#include <stdlib.h>  // atoi()

#include "pack.h"


int main(int argc, char* argv[]) {
  if (argc < 2) {
    // Reading.
    char buffer[32];
    int  n = 0;

    while (!feof(stdin)) {
      n += fread(&buffer[n], 1, 1, stdin);
    }

    printf("%d bytes read\n", n);

    // Unpack the data we just received.
    PACKED(p, buffer, n);

    int32_t num = unpack_int32_t(&p);

    if (p.error) {
      printf("error unpacking\n");
    } else {
      printf("the number was: %d\n", num);
    }
  } else {
    // Writing.
    int32_t num = (argc < 2) ? 42 : atoi(argv[1]);

    // Pack the number.
    PACKER(p, 32);

    pack_int32_t(&p, num);
    pack_finish(&p);

    fwrite(p.data, p.length, 1, stdout);
  }

  return 0;
}

