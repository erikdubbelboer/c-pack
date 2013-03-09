
#ifndef SRC_PACK_H_
#define SRC_PACK_H_


#include <stdint.h>  // uint64_t
#include <string.h>  // memcpy()


#define PACK_FIELDS \
  uint32_t _s;      \
  char*    _p;      \
  uint32_t length;  \
  int      error;


typedef struct packed_s {
  PACK_FIELDS
} packed_t;


// Define a structure used to pack data.
// The union is only there to force the struct to have the
// same layout as packed_t.
#define PACKER(name, size)                                \
  struct _ ## name ## _s {                                \
    PACK_FIELDS                                           \
    char     data[size];                                  \
  } name;                                                 \
  union _ ## name ## _align {                             \
    packed_t a;                                           \
    struct _ ## name ## _s b;                             \
  };                                                      \
  if (size < 4) {                                         \
    name._s     = 0;                                      \
    name.length = 0;                                      \
    name.error  = 1;                                      \
  } else {                                                \
    name.length = sizeof(name.length);                    \
    memcpy(name.data, &name.length, sizeof(name.length)); \
    name._s     = size;                                   \
    name._p     = name.data + sizeof(name.length);        \
    name.error  = 0;                                      \
  }


// Define a structure to unpack data.
// Also check if the length matches what was
// packed into the data.
#define PACKED(name, data, len)           \
  packed_t name;                          \
  name._s     = 0;                        \
  name._p     = data;                     \
  name.length = len;                      \
  if (len < 4) {                          \
    name.error = 1;                       \
  } else {                                \
    name.error  = 0;                      \
    name.length = unpack_uint32_t(&name); \
    if (name.length > len) {              \
      name.length = len;                  \
      ++name.error;                       \
    }                                     \
  }


void       pack_int8_t  (void* pa, int8_t   i);
int8_t   unpack_int8_t  (packed_t* p);
void       pack_uint8_t (void* pa, uint8_t  i);
uint8_t  unpack_uint8_t (packed_t* p);
void       pack_int16_t (void* pa, int16_t  i);
int16_t  unpack_int16_t (packed_t* p);
void       pack_uint16_t(void* pa, uint16_t i);
uint16_t unpack_uint16_t(packed_t* p);
void       pack_int32_t (void* pa, int32_t  i);
int32_t  unpack_int32_t (packed_t* p);
void       pack_uint32_t(void* pa, uint32_t i);
uint32_t unpack_uint32_t(packed_t* p);
void       pack_int64_t (void* pa, int64_t  i);
int64_t  unpack_int64_t (packed_t* p);
void       pack_uint64_t(void* pa, uint64_t i);
uint64_t unpack_uint64_t(packed_t* p);


void pack_string(void* pa, const char* s);

// This function returns a pointer inside the data (not a copy).
char* unpack_string(packed_t* p);

void pack_buffer(void* pa, const void* buffer, uint32_t length);

// This function returns a pointer inside the data (not a copy).
void* unpack_buffer(packed_t* p, uint32_t* length);


// Finish packing. This function should always be called before using the packed data.
void pack_finish(void* pa);


#if 0

// This macro can be used to pack data based on the automatic type of it's argument.
// It is not adviced to use this function seeing as you need to know the
// explicit type when unpacking.

#define pack(p, d)                                                                                                            \
  do {                                                                                                                        \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), int8_t  ), pack_int8_t  (p, d), (void)0);               \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), uint8_t ), pack_uint8_t (p, d), (void)0);               \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), int16_t ), pack_int16_t (p, d), (void)0);               \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), uint16_t), pack_uint16_t(p, d), (void)0);               \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), int32_t ), pack_int32_t (p, d), (void)0);               \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), uint32_t), pack_uint32_t(p, d), (void)0);               \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), int64_t ), pack_int64_t (p, d), (void)0);               \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), uint64_t), pack_uint64_t(p, d), (void)0);               \
    __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(d), const char*), pack_string(p, (const char*)d), (void)0); \
  } while (0);

#endif


#endif // SRC_PACK_H

