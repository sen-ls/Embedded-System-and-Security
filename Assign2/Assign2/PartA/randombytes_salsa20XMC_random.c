/*
 * ISC License
 *
 * Copyright (c) 2018 TUM
 * Author: Florian Wilde <florian.wilde@tum.de>
 * Based on previous work published with libsodium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <sys/types.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
# include <unistd.h>
#endif

#include <sodium/crypto_core_salsa20.h> // "sodium/include/sodium/crypto_core_salsa20.h"
#include <sodium/crypto_generichash.h> //"sodium/include/sodium/crypto_generichash.h"
#include <sodium/crypto_stream_salsa20.h> //"sodium/include/sodium/crypto_stream_salsa20.h"
#include <sodium/randombytes.h> //"sodium/include/sodium/randombytes.h"
#include "randombytes_salsa20XMC_random.h"
#include <sodium/utils.h> //"sodium/include/sodium/utils.h"

#define SALSA20_RANDOM_BLOCK_SIZE crypto_core_salsa20_OUTPUTBYTES
#define HASH_BLOCK_SIZE 128U
#define COMPILER_ASSERT(X) (void) sizeof(char[(X) ? 1 : -1])

#ifndef SSIZE_MAX
# define SSIZE_MAX (SIZE_MAX / 2 - 1)
#endif

typedef struct Salsa20Random_ {
    size_t        rnd32_outleft;
    int           random_data_source_fd;
    int           initialized;
    int           getrandom_available;
    unsigned char key[crypto_stream_salsa20_KEYBYTES];
    unsigned char rnd32[16U * SALSA20_RANDOM_BLOCK_SIZE];
    uint64_t      nonce;
#ifdef HAVE_GETPID
    pid_t         pid;
#endif
} Salsa20Random;

static Salsa20Random stream = {
    SODIUM_C99(.rnd32_outleft =) (size_t) 0U,
    SODIUM_C99(.random_data_source_fd =) -1,
    SODIUM_C99(.initialized =) 0,
    SODIUM_C99(.getrandom_available =) 0
};

static void
randombytes_salsa20XMC_random_init(void)
{
    /* In lack of real time clock or /dev/urandom,
     * use a hash of DSRAM1 as entropy source */
    unsigned char hash[crypto_generichash_BYTES];
    crypto_generichash(hash, sizeof hash, RANDOMBYTES_SALSA20XMC_PSRAM_START,\
                       RANDOMBYTES_SALSA20XMC_PSRAM_SIZE, NULL, 0);
    memcpy(&stream.nonce, &hash, sizeof(stream.nonce) < sizeof(hash) ? \
                                 sizeof(stream.nonce) : sizeof(hash));

    assert(stream.nonce != (uint64_t) 0U);
}

static void
randombytes_salsa20XMC_random_rekey(const unsigned char * const mix)
{
    unsigned char *key = stream.key;
    size_t         i;

    for (i = (size_t) 0U; i < sizeof stream.key; i++) {
        key[i] ^= mix[i];
    }
}

static void
randombytes_salsa20XMC_random_stir(void)
{
    /* constant to personalize the hash function */
    const unsigned char hsigma[crypto_generichash_KEYBYTES] = {
        0x54, 0x68, 0x69, 0x73, 0x49, 0x73, 0x4a, 0x75,
        0x73, 0x74, 0x41, 0x54, 0x68, 0x69, 0x72, 0x74,
        0x79, 0x54, 0x77, 0x6f, 0x42, 0x79, 0x74, 0x65,
        0x73, 0x53, 0x65, 0x65, 0x64, 0x2e, 0x2e, 0x2e
    };
    unsigned char  m0[crypto_stream_salsa20_KEYBYTES + HASH_BLOCK_SIZE];
    unsigned char *k0 = m0 + crypto_stream_salsa20_KEYBYTES;
    size_t         sizeof_k0 = sizeof m0 - crypto_stream_salsa20_KEYBYTES;

    memset(stream.rnd32, 0, sizeof stream.rnd32);
    stream.rnd32_outleft = (size_t) 0U;
    if (stream.initialized == 0) {
        randombytes_salsa20XMC_random_init();
        stream.initialized = 1;
    }
    if (crypto_generichash(stream.key, sizeof stream.key, k0, sizeof_k0,
                           hsigma, sizeof hsigma) != 0) {
        abort(); /* LCOV_EXCL_LINE */
    }
    COMPILER_ASSERT(sizeof stream.key <= sizeof m0);
    randombytes_salsa20XMC_random_rekey(m0);
    sodium_memzero(m0, sizeof m0);
#ifdef HAVE_GETPID
    stream.pid = getpid();
#endif
}

static void
randombytes_salsa20XMC_random_stir_if_needed(void)
{
#ifdef HAVE_GETPID
    if (stream.initialized == 0) {
        randombytes_salsa20XMC_random_stir();
    } else if (stream.pid != getpid()) {
        abort();
    }
#else
    if (stream.initialized == 0) {
        randombytes_salsa20XMC_random_stir();
    }
#endif
}

static void
randombytes_salsa20XMC_random_buf(void * const buf, const size_t size)
{
    size_t i;
    int    ret;

    randombytes_salsa20XMC_random_stir_if_needed();
    COMPILER_ASSERT(sizeof stream.nonce == crypto_stream_salsa20_NONCEBYTES);
#ifdef ULONG_LONG_MAX
    /* coverity[result_independent_of_operands] */
    assert(size <= ULONG_LONG_MAX);
#endif
    ret = crypto_stream_salsa20((unsigned char *) buf, (unsigned long long) size,
                                (unsigned char *) &stream.nonce, stream.key);
    assert(ret == 0);
    for (i = 0U; i < sizeof size; i++) {
        stream.key[i] ^= ((const unsigned char *) (const void *) &size)[i];
    }
    stream.nonce++;
    crypto_stream_salsa20_xor(stream.key, stream.key, sizeof stream.key,
                              (unsigned char *) &stream.nonce, stream.key);
}

static uint32_t
randombytes_salsa20XMC_random_getword(void)
{
    uint32_t val;
    int      ret;

    COMPILER_ASSERT(sizeof stream.rnd32 >= (sizeof stream.key) + (sizeof val));
    COMPILER_ASSERT(((sizeof stream.rnd32) - (sizeof stream.key))
                    % sizeof val == (size_t) 0U);
    if (stream.rnd32_outleft <= (size_t) 0U) {
        randombytes_salsa20XMC_random_stir_if_needed();
        COMPILER_ASSERT(sizeof stream.nonce == crypto_stream_salsa20_NONCEBYTES);
        ret = crypto_stream_salsa20((unsigned char *) stream.rnd32,
                                    (unsigned long long) sizeof stream.rnd32,
                                    (unsigned char *) &stream.nonce,
                                    stream.key);
        assert(ret == 0);
        stream.rnd32_outleft = (sizeof stream.rnd32) - (sizeof stream.key);
        randombytes_salsa20XMC_random_rekey(&stream.rnd32[stream.rnd32_outleft]);
        stream.nonce++;
    }
    stream.rnd32_outleft -= sizeof val;
    memcpy(&val, &stream.rnd32[stream.rnd32_outleft], sizeof val);
    memset(&stream.rnd32[stream.rnd32_outleft], 0, sizeof val);

    return val;
}

static uint32_t
randombytes_salsa20XMC_random(void)
{
    return randombytes_salsa20XMC_random_getword();
}

static const char *
randombytes_salsa20XMC_implementation_name(void)
{
    return "salsa20XMC";
}

struct randombytes_implementation randombytes_salsa20XMC_implementation = {
    SODIUM_C99(.implementation_name =) randombytes_salsa20XMC_implementation_name,
    SODIUM_C99(.random =) randombytes_salsa20XMC_random,
    SODIUM_C99(.stir =) randombytes_salsa20XMC_random_stir,
    SODIUM_C99(.uniform =) NULL,
    SODIUM_C99(.buf =) randombytes_salsa20XMC_random_buf,
    SODIUM_C99(.close =) NULL
};
