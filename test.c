
#include <stdio.h>   /* printf() */
#include <stdlib.h>  /* rand()   */
#include <string.h>  /* memcpy() */
#include <endian.h>  /* htob..() */

#include "pack.h"


void test_empty() {
  PACKER(p, 32);

  pack_finish(&p);

  PACKED(up, p.data, p.length);

  if (p.error || up.error) {
    printf("error on empty pack\n");
  }
}


void test_string() {
  const char* test = "test";

  PACKER(p, 32);

  pack_string(&p, test);
  pack_finish(&p);

  if (p.error) {
    printf("error packing string\n");
  } else {
    PACKED(up, p.data, p.length);

    if (up.error) {
       printf("error in string pack\n");
    } else {
      char* s = unpack_string(&up);

      if (up.error) {
        printf("error unpacking string\n");
      } else if (strcmp(s, test) != 0) {
        printf("data mismatch for string\n");
      } else if (up._s != up.length) {
        printf("to much data for string\n");
      }
    }
  }
}


void test_buffer() {
  const uint8_t test[] = { 1, 2, 0, 4 };

  PACKER(p, 32);

  pack_buffer(&p, test, 4);
  pack_finish(&p);

  if (p.error) {
    printf("error packing buffer\n");
  } else {
    PACKED(up, p.data, p.length);

    if (up.error) {
       printf("error in buffer pack\n");
    } else {
      uint32_t length;
      void*    b = unpack_buffer(&up, &length);

      if (up.error) {
        printf("error unpacking buffer\n");
      } else if (length != 4) {
        printf("buffer size mismatch\n");
      } else if (memcmp(b, test, 4) != 0) {
        printf("data mismatch for buffer\n");
      } else if (up._s != up.length) {
        printf("to much data for buffer\n");
      }
    }
  }
}


void test_overflow() {
  PACKER(p1, 2);

  pack_int64_t(&p1, 1l);

  if (p1.error == 0) {
    printf("expected error when writing to too small buffer\n");
  }


  PACKER(p2, 3);

  pack_string(&p2, "abcd");

  if (p2.error == 0) {
    printf("expected error when writing to too small buffer\n");
  }
}


void test_new() {
  PACKER_NEW(p, 8);

  pack_int64_t(p, 1234);
  pack_finish(p);

  if (p->error) {
    printf("error packing with new\n");
  }

  PACKED_NEW(up, pack_data(p), p->length);

  if (up->error) {
    printf("error in new pack\n");
  }

  free(p);
  free(up);
}


void test_random() {
  enum {
    t_int8_t,
    t_uint8_t,
    t_int16_t,
    t_uint16_t,
    t_int32_t,
    t_uint32_t,
    t_int64_t,
    t_uint64_t,
    t_string_t,
    t_max
  };

  typedef union data_u {
    int8_t   t_int8_t;
    uint8_t  t_uint8_t;
    int16_t  t_int16_t;
    uint16_t t_uint16_t;
    int32_t  t_int32_t;
    uint32_t t_uint32_t;
    int64_t  t_int64_t;
    uint64_t t_uint64_t;
    char     t_string_t[32];
  } data_t;

  PACKER(p, 4096);
  int    num = rand() % 100;
  int    types[num];
  data_t data[num];

  for (int i = 0; i < num; ++i) {
    types[i] = rand() % t_max;

    switch (types[i]) {
      #define TYPE(type) \
      case t_##type: { \
        data[i].t_##type = rand(); \
        pack_##type(&p, data[i].t_##type); \
        if (p.error) { \
          printf("error packing random " #type "\n"); \
        } \
        break; \
      }

      TYPE(int8_t);
      TYPE(uint8_t);
      TYPE(int16_t);
      TYPE(uint16_t);
      TYPE(int32_t);
      TYPE(uint32_t);
      TYPE(int64_t);
      TYPE(uint64_t);

      #undef TYPE

      case t_string_t: {
        snprintf(data[i].t_string_t, 32, "%d", rand());
        pack_string(&p, data[i].t_string_t);
        break;
      }
    }
  }

  pack_finish(&p);

  PACKED(up, p.data, p.length);

  if (up.error) {
    printf("error in random pack\n");
  }

  for (int i = 0; i < num; ++i) {
    switch (types[i]) {
      #define TYPE(type) \
      case t_##type: { \
        if (unpack_##type(&up) != data[i].t_##type) { \
          printf("error for random type " #type "\n"); \
        } \
        if (up.error) { \
          printf("error unpacking random " #type "\n"); \
        } \
        break; \
      }

      TYPE(int8_t);
      TYPE(uint8_t);
      TYPE(int16_t);
      TYPE(uint16_t);
      TYPE(int32_t);
      TYPE(uint32_t);
      TYPE(int64_t);
      TYPE(uint64_t);

      #undef TYPE

      case t_string_t: {
        char* s = unpack_string(&up);
        if (strcmp(s, data[i].t_string_t) != 0) {
           printf("random error for type string\n");
        }
        break;
      }
    }
  }
}


int main() {
  printf("testing...\n");


  #define TEST(type)                                 \
    do {                                             \
      PACKER(p, 32);                                 \
      pack_##type(&p, 42);                           \
      pack_finish(&p);                               \
      if (p.error) {                                 \
        printf("error packing " #type "\n");         \
      } else {                                       \
        PACKED(up, p.data, p.length);                \
        if (up.error) {                              \
          printf("error in " #type " pack\n");       \
        } else {                                     \
          type i = unpack_##type(&up);               \
          if (up.error) {                            \
            printf("error unpacking " #type "\n");   \
          } else if (i != 42) {                      \
            printf("data mismatch for " #type "\n"); \
          } else if (up._s != up.length) {           \
            printf("to much data for " #type "\n");  \
          }                                          \
        }                                            \
      }                                              \
    } while (0);

  TEST(int8_t );
  TEST(uint8_t);

  TEST(int16_t );
  TEST(uint16_t);

  TEST(int32_t );
  TEST(uint32_t);

  TEST(int64_t );
  TEST(uint64_t);

  #undef TEST


  test_empty();
  test_string();
  test_buffer();
  test_overflow();
  test_new();


  srand(0);

  for (int i = 0; i < 100000; ++i) {
    test_random();
  }


  printf("done.\n");
  
  return 0;
}

