/** 
   Encoding of LDPC codes is done by multiplying vector of message (m) with generating matrix (G) using modulo two aritmetics to get codeword (c)

   Generating matrix is obtained from parity check matrix (H) (more about it later) as [I_m | P], where I_m is identity matrix of dimension m

   Codeword composition:

   ********* 
   *message*   
   *********  

     /¯\          
   /¯   ¯\       
   x x x x p p p p
           \_   _/
             \_/

        **************
        *parity check*
        **************

   Example encoding:

                              / 1 0 0 0 0 1 0 1 \           
      c = m * G = (0 1 0 1)   | 0 1 0 0 1 1 0 1 | = (0 1 0 1 0 1 1 0)
                              | 0 0 1 0 0 1 1 0 |    
                              \ 0 0 0 1 1 0 1 1 /
                              
   Decoding is done by multiplying recieved codeword with transformed parity check matrix (H) in form [-P^T | I_(c - m)] to get syndrom (s) of the codeword
   If s = 0, then message is acquired as first m bits of codeword, otherwise is done error corection using bit flip method
                              
   Example decoding:

                                    / 0 1 0 1 \
                                    | 1 1 0 1 |
                                    | 0 1 1 0 |
   s = c * H^T = (0 1 0 1 0 1 1 0)  | 1 0 1 1 | = (0 0 0 0) => m = (0 1 0 1)
                                    | 1 0 0 0 !
                                    | 0 1 0 0 |
                                    | 0 0 1 0 |
                                    \ 0 0 0 1 /

   Example error correction:

   c´ = (0 1 1 1 0 1 1 0)

   s = c´ * H^T = (0 1 1 0) => error present

   Add point to every bit coded by parity bits that detected error and bitflip the most likely culprit (now we don´t use modulo two aritmetics):

   r = (1 1 1 0 0 1 0 0) + (0 0 1 1 0 0 1 0) = (1 1 2 1 0 1 1 0) => error on 3rd bit

   c´´ = (0 1 0 1 0 1 1 0)

   s = c´´ * H^T = (0 0 0 0) => m = (0 1 0 1)

   If only one parity bit detected error it means that this parity bit itself got corrupted

   
   In order for error corection to work consistently it is necessary to follow these rules:

   1) each bit of message is coded by at least two parity bits

   2) no parity bit codes only bits, that are already coded by other parity bit (coded bits are not subset of bits coded by any other parity bit)

   3) number of parity bits is at least 1/3 of codeword length (for short codewords it should be close to 1/2 or even more)
**/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../ldpc.h"
#include "ldpc_matrixes.c"

/**Settings**/

#define max_bitflips 300

/**Helper functions and macros**/

/*
  Gets parity of row_width bits of message specified by matrix_row 

  Returns parity of those bits
*/
bool get_parity_bit(const uint8_t matrix_row[], const uint8_t row_width, uint8_t* message) {
  uint8_t parity_num = 0;

  for (uint8_t i = 0; i < row_width; i++) {
    uint8_t position_in_byte = matrix_row[i] % 8;
    uint8_t byte_position = (matrix_row[i] - position_in_byte) / 8;

    if ((message[byte_position] & (1 << (7 - position_in_byte)))) parity_num++;
  }

  if (parity_num % 2) return true;
  
  return false;
}

/*
  Encodes message of length message_len with parity_bits parity bits using matrix with number of ones in row defined by matrix_row_widths and puts result in codeword_buffer
*/
#define encode_routine(message, message_bits, parity_bits, codeword_buffer, matrix, matrix_row_widths) do {                                               \
  memcpy(codeword_buffer, message, message_bits / 8);                                                                                                     \
                                                                                                                                                          \
  for (uint8_t i = 0; i < parity_bits; i++) {                                                                                                             \
    codeword[(message_bits + (i - i % 8)) / 8] += (get_parity_bit(matrix[i], matrix_row_widths[i] - 1, message) << (7 - i % 8)); /*setting parity bits*/  \
  } \
} while (0)

/*
  Gets syndrom of codeword with number of parity bits defined by parity_bits using matrix with number of ones in row defined by matrix_row_widths and puts result in syndrom_buffer
*/
#define get_syndrom_routine(codeword, parity_bits, syndrom_buffer, matrix, matrix_row_widths) for (uint8_t i = 0; i < parity_bits; i++) {                                 \
  syndrom_buffer[(i - i % 8) / 8] = syndrom_buffer[(i - i % 8) / 8]  | get_parity_bit(matrix[i], matrix_row_widths[i], codeword) << (7 - i % 8); /*setting syndrom bits*/ \
}

/*
  Corrects one bit of code with length codeword_len and number of parity bits defined by parity_bits using syndrome and matrix with number of ones in row defined by matrix_row_widths
*/
#define bitflip_routine(codeword, codeword_len, parity_bits, syndrom, matrix, matrix_row_widths) do {                                                                     \
  uint8_t *bitflip_likelyhood_score = calloc(codeword_len, sizeof(uint8_t));                                                                                              \
                                                                                                                                                                          \
  for (uint8_t i = 0; i < parity_bits; i++) {                                                                                                                                                                \
    if (!((syndrom[(i - i % 8) / 8] >> (7 - i % 8)) & 1)) continue; /*checking syndrome bit*/                                                                             \
                                                                                                                                                                          \
    for (uint8_t j = 0; j < matrix_row_widths[i]; j++) {                                                                                                                  \
      bitflip_likelyhood_score[matrix[i][j]]++;                                                                                                                           \
    }                                                                                                                                                                     \
  }                                                                                                                                                                       \
                                                                                                                                                                          \
  uint8_t most_likely_bitflip = 0;                                                                                                                                        \
  uint8_t highest_bitflip_score = 0;                                                                                                                                      \
                                                                                                                                                                          \
  for (uint16_t i = 0; i < codeword_len; i++) {                                                                                                                           \
    if (bitflip_likelyhood_score[i] >= highest_bitflip_score) {                                                                                                           \
      most_likely_bitflip = i;                                                                                                                                            \
      highest_bitflip_score = bitflip_likelyhood_score[i];                                                                                                                \
    }                                                                                                                                                                     \
  }                                                                                                                                                                       \
                                                                                                                                                                          \
  /*Bitflipping itself*/                                                                                                                                                  \
  if (highest_bitflip_score) {                                                                                                                                            \
    codeword[(most_likely_bitflip - most_likely_bitflip % 8) / 8] = codeword[(most_likely_bitflip - most_likely_bitflip % 8) / 8] ^ (1 << (7 - most_likely_bitflip % 8)); \
  }                                                                                                                                                                       \
} while(0)


/**Library functions**/

uint8_t* LDPC_encode(CODE_TYPE type, uint8_t* message) {
  uint8_t* codeword;
  
  switch (type) {
    case _16BIT_CODE:
      codeword = calloc(2, sizeof(uint8_t));

      encode_routine(message, 8, 8, codeword, _16bit_matrix, _16bit_matrix_row_width);

      break;
    case _64BIT_CODE:
      codeword = calloc(8, sizeof(uint8_t));

      encode_routine(message, 40, 24, codeword, _64bit_matrix, _64bit_matrix_row_width);

      break;
    case _256BIT_CODE:
      codeword = calloc(32, sizeof(uint8_t));

      encode_routine(message, 160, 96, codeword, _256bit_matrix, _256bit_matrix_row_width);

      break;
    default:
      return NULL;
  }

  return codeword;
}

uint8_t* LDPC_decode(CODE_TYPE type, uint8_t* codeword) {
  uint8_t *syndrom, *message;

  uint16_t number_of_bitflips = 0;

  switch (type) {
    case _16BIT_CODE:
      syndrom = calloc(1, sizeof(uint8_t*));

      get_syndrom_routine(codeword, 8, syndrom, _16bit_matrix, _16bit_matrix_row_width);

      while (number_of_bitflips < max_bitflips) {

        bitflip_routine(codeword, 16, 8, syndrom, _16bit_matrix, _16bit_matrix_row_width);

        bool corrected = true;

        for (uint8_t i = 0; i < 2; i++) {
          if (syndrom[i]) {
            corrected = false;

            syndrom[i] = 0;
          }
        }

        if (corrected) {
          break;
        }

        get_syndrom_routine(codeword, 8, syndrom, _16bit_matrix, _16bit_matrix_row_width);

        number_of_bitflips++;
      }

      message = malloc(1 * sizeof(uint8_t));

      memcpy(message, codeword, 1);

      break;
    case _64BIT_CODE:
      syndrom = calloc(3, sizeof(uint8_t*));

      get_syndrom_routine(codeword, 24,  syndrom, _64bit_matrix, _64bit_matrix_row_width);

      while (number_of_bitflips < max_bitflips) {
        bitflip_routine(codeword, 64, 24, syndrom, _64bit_matrix, _64bit_matrix_row_width);

        bool corrected = true;

        for (uint8_t i = 0; i < 8; i++) {
          if (syndrom[i]) {
            corrected = false;

            syndrom[i] = 0;
          }
        }

        if (corrected) {
          break;
        }

        get_syndrom_routine(codeword, 24, syndrom, _64bit_matrix, _64bit_matrix_row_width);

        number_of_bitflips++;
      }

      message = malloc(5 * sizeof(uint8_t));

      memcpy(message, codeword, 5);

      break;
    case _256BIT_CODE:
      syndrom = calloc(12, sizeof(uint8_t*));

      get_syndrom_routine(codeword, 96, syndrom, _256bit_matrix, _256bit_matrix_row_width);

      while (number_of_bitflips < max_bitflips) {
        bitflip_routine(codeword, 256, 96, syndrom, _256bit_matrix, _256bit_matrix_row_width);
        bool corrected = true;

        for (uint8_t i = 0; i < 32; i++) {
          if (syndrom[i]) {
            corrected = false;

            syndrom[i] = 0;
          }
        }

        if (corrected) {
          break;
        }

        get_syndrom_routine(codeword, 96, syndrom, _16bit_matrix, _256bit_matrix_row_width);

        number_of_bitflips++;
      }

      message = malloc(20 * sizeof(uint8_t));

      memcpy(message, codeword, 20);

      break;
    default:
      return NULL;
  }

  return message;
}