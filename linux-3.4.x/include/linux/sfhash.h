#ifndef _LINUX_SFHASH_H
#define _LINUX_SFHASH_H

/* sfhash.h: SuperFastHash support.
 *
 * Copyright (C) 2004 by Paul Hsieh
 *
 * http://www.azillionmonkeys.com/qed/hash.html
 *
 */

#define get16bits(d) (*((const u16 *) (d)))

/* The most generic version, hashes an arbitrary sequence
 * of bytes.  No alignment or length assumptions are made about
 * the input key.
 */
static inline u32 __sfhash_nwords(const void *key, u32 len, u32 initval)
{
	const char * data = key;
	u32 hash = len + initval, tmp;
	int rem;

	if (len <= 0 || data == NULL)
		return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--) {
		/* Mix 32bit chunk of the data */
		hash += get16bits(data);
		tmp   = (get16bits(data+2) << 11) ^ hash;
		hash  = (hash << 16) ^ tmp;
		data += 2*sizeof(u16);
		hash += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
	case 3:	hash += *((u16 *)data);
		hash ^= hash << 16;
		hash ^= data[sizeof(u16)] << 18;
		hash += hash >> 11;
		break;
	case 2:	hash += *((u16 *)data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1: hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 2;
	hash += hash >> 15;
	hash ^= hash << 10;

	return hash;
}

/* The generic version
 */
static inline u32 sfhash(const void *key, u32 length, u32 initval)
{
	return __sfhash_nwords(key, length, initval);
}

/* Special versions for hashing exactly 3 words.
 */
static inline u32 sfhash_3words(u32 a, u32 b, u32 c, u32 initval)
{
	u32 data[3] = {a,b,c};

	return __sfhash_nwords(data, 12, initval);
}

/* Special versions for hashing exactly 2 words.
 */
static inline u32 sfhash_2words(u32 a, u32 b, u32 initval)
{
	return sfhash_3words(a, b, 0, initval);
}

/* Special versions for hashing exactly 1 words.
 */
static inline u32 sfhash_1word(u32 a, u32 initval)
{
	return sfhash_3words(a, 0, 0, initval);
}

/* replace jhash to sfhash */
#undef jhash_3words
#undef jhash_2words
#undef jhash_1words
#undef jhash

#define jhash_3words(a,b,c,i)	sfhash_3words(a,b,c,i)
#define jhash_2words(a,b,i)	sfhash_2words(a,b,i)
#define jhash_1word(a,i)	sfhash_1word(a,i)
#define jhash(k,l,i)		sfhash(k,l,i)

#endif
