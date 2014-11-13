/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*
 * EAP server/peer: EAP-PSK shared routines
 * Copyright (c) 2004-2006, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "aes_wrap.h"
#include "eap_defs.h"
#include "eap_psk_common.h"

#define aes_block_size 16


void eap_psk_key_setup(const u8 *psk, u8 *ak, u8 *kdk)
{
	os_memset(ak, 0, aes_block_size);
	aes_128_encrypt_block(psk, ak, ak);
	os_memcpy(kdk, ak, aes_block_size);
	ak[aes_block_size - 1] ^= 0x01;
	kdk[aes_block_size - 1] ^= 0x02;
	aes_128_encrypt_block(psk, ak, ak);
	aes_128_encrypt_block(psk, kdk, kdk);
}


void eap_psk_derive_keys(const u8 *kdk, const u8 *rand_p, u8 *tek, u8 *msk,
			 u8 *emsk)
{
	u8 hash[aes_block_size];
	u8 counter = 1;
	int i;

	aes_128_encrypt_block(kdk, rand_p, hash);

	hash[aes_block_size - 1] ^= counter;
	aes_128_encrypt_block(kdk, hash, tek);
	hash[aes_block_size - 1] ^= counter;
	counter++;

	for (i = 0; i < EAP_MSK_LEN / aes_block_size; i++) {
		hash[aes_block_size - 1] ^= counter;
		aes_128_encrypt_block(kdk, hash, &msk[i * aes_block_size]);
		hash[aes_block_size - 1] ^= counter;
		counter++;
	}

	for (i = 0; i < EAP_EMSK_LEN / aes_block_size; i++) {
		hash[aes_block_size - 1] ^= counter;
		aes_128_encrypt_block(kdk, hash, &emsk[i * aes_block_size]);
		hash[aes_block_size - 1] ^= counter;
		counter++;
	}
}
