/* Source file for the homebrewn crypto used in assignment 2
 *
 * Copyright 2018 TUM
 * Created: 2018-11-15 Florian Wilde <florian.wilde@tum.de>
 **************************************************************************************************/

#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include "crypto.h"

uint8_t encrypt(uint8_t *ciphertext[],
                const uint8_t *plaintext, const size_t plaintext_len,
                const uint8_t *nonce, const uint8_t *key ) {
  /* Encrypt like the most popular app with over 30 million users worldwide did
     cf. https://www.ssl.com/article/nq-vault-app-not-as-secure-as-advertised/  */
  uint8_t buf[128];
  memcpy(buf, plaintext, plaintext_len);
  for(uint8_t i=0; i<128; i++) buf[i] ^= key[nonce[0] % 8];
  *ciphertext = malloc(plaintext_len);
  memcpy(*ciphertext, buf, plaintext_len);
  return 0;
}

size_t getCiphertextLength(const size_t plaintext_len) {
  return plaintext_len;
}
