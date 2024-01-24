/* Header file for the homebrewn crypto used in assignment 2
 *
 * Copyright 2018 TUM
 * Created: 2018-11-15 Florian Wilde <florian.wilde@tum.de>
 **************************************************************************************************/

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>

#define CRYPTO_NONCE_BYTES ((size_t) (192/8))

uint8_t encrypt(uint8_t *ciphertext[],
                const uint8_t *plaintext, const size_t plaintext_len,
                const uint8_t *nonce, const uint8_t *key );

size_t getCiphertextLength(const size_t plaintext_len);

#endif/*CRYPTO_H*/
