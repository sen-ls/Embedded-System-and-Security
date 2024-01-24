/*
 * ISC License
 *
 * Copyright (c) 2018 TUM
 * Author: Florian Wilde <florian.wilde@tum.de>
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

#ifndef randombytes_salsa20_XMC_H
#define randombytes_salsa20_XMC_H

#include <sodium/export.h> //"sodium/include/sodium/export.h"
#include <sodium/randombytes.h> //"sodium/include/sodium/randombytes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RANDOMBYTES_SALSA20XMC_PSRAM_START ((void *) 0x10000000)
#define RANDOMBYTES_SALSA20XMC_PSRAM_SIZE ((size_t) 0x10000)

SODIUM_EXPORT
extern struct randombytes_implementation randombytes_salsa20XMC_implementation;

#ifdef __cplusplus
}
#endif

#endif
