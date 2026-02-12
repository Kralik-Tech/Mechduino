#ifndef LDPC_H
#define LDPC_H

#include <stdlib.h>
#include <stdint.h>

typedef enum {
  _16BIT_CODE, // 16 bit code with 8 bits of message
  _64BIT_CODE, // 64 bit code with 40 bits of message
  _256BIT_CODE // 256 bit code with 160 bits of message
} CODE_TYPE;

/*
Encodes message into code specified by type

Returns pointer to resulting codeword or NULL if type is invalid
*/
uint8_t* LDPC_encode(CODE_TYPE type, uint8_t* message);

/*
Decodes codeword of type specified by type

Returns pointer to decoded message NULL if type is invalid
*/
uint8_t* LDPC_decode(CODE_TYPE type, uint8_t* codeword);

#endif //LDPC_H
