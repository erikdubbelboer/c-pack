
#include <stdio.h>   /* printf() */
#include <stdlib.h>  /* rand()   */
#include <endian.h>  /* htob..() */

#include "pack.h"


/* Macro to define integer pack and unpack functions. */
#define PACK(type, conv)                    \
void pack_##type(void* pa, type i) {        \
  packed_t* p = (packed_t*)pa;              \
  i = conv(i);                              \
  if ((p->length + sizeof(type)) > p->_s) { \
    ++p->error;                             \
  } else {                                  \
    memcpy(p->_p, &i, sizeof(type));        \
    p->_p     += sizeof(type);              \
    p->length += sizeof(type);              \
  }                                         \
}                                           \
                                            \
type unpack_##type(packed_t* p) {           \
  type d = 0;                               \
  if ((p->_s + sizeof(type)) > p->length) { \
    ++p->error;                             \
  } else {                                  \
    memcpy(&d, p->_p, sizeof(type));        \
    p->_s += sizeof(type);                  \
    p->_p += sizeof(type);                  \
  }                                         \
  return conv(d);                           \
}


static inline int8_t int8_t_nop (int8_t  i) { return i; }
static inline int8_t uint8_t_nop(uint8_t i) { return i; }

PACK(int8_t  , int8_t_nop )
PACK(uint8_t , uint8_t_nop)
PACK(int16_t , htobe16)
PACK(uint16_t, htobe16)
PACK(int32_t , htobe32)
PACK(uint32_t, htobe32)
PACK(int64_t , htobe64)
PACK(uint64_t, htobe64)

#undef PACK


void pack_string(void* pa, const char* s) {
  packed_t* p = (packed_t*)pa;

  size_t length = strlen(s);

  if ((p->length + length + 1) >  p->_s) {
    ++p->error;
  } else {
    memcpy(p->_p, s, length);
    p->_p[length] = 0;
    ++length;  /* Count the 0 byte. */
    p->_p     += length;
    p->length += length;
  }
}

char* unpack_string(packed_t* p) {
  size_t length = strlen(p->_p);
  char*  s      = p->_p;
  ++length;  /* Skip the 0 byte. */
  p->_s += length;
  p->_p += length;
  return s;
}


void pack_buffer(void* pa, const void* buffer, uint32_t length) {
  packed_t* p = (packed_t*)pa;

  if ((p->length + length + 1) >  p->_s) {
    ++p->error;
  } else {
    pack_uint32_t(p, length);
    memcpy(p->_p, buffer, length);
    p->_p     += length;
    p->length += length;
  }
}

void* unpack_buffer(packed_t* p, uint32_t* length) {
  *length = unpack_uint32_t(p);

  if (p->error) {
    return 0;
  }

  if ((p->_s + *length) > p->length) {
    ++p->error;
    return 0;
  }

  void* buffer = (void*)(p->_p);

  p->_s += *length;
  p->_p += *length;

  return buffer;
}


/* Write the length to the start of the data. */
void pack_finish(void* pa) {
  packed_t* p      = (packed_t*)pa;
  uint32_t  length = htobe32(p->length);

  memcpy(p->_p - p->length, &length, sizeof(length));
}


void unpack_reset(void* pa) {
  packed_t* p = (packed_t*)pa;

  /* Reset back to after the size we read. */
  p->_p -= (p->_s - 4);
  p->_s  = 4;
}

