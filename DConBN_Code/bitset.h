#ifndef BITSET_H
#define	BITSET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BITS_PER_CHAR 8
#define BITS_PER_CHAR_FLOAT 8.0f

typedef struct BitSetStruct BitSet;

struct BitSetStruct {
    char *_mblock; // a block of memmery
    int _len; //the number of bits specified by users
    int _len2; //the number of bytes in the _mblock 
    int _count; //the number of bits that is set to 1 in the _mblock
};

int bitset_isset(BitSet &bs, int pos);
void bitset_reset(BitSet &bs, int pos);


static const unsigned char mask1[8] = {0x01, /* 00000001 */
    0x02, /* 00000010 */
    0x04, /* 00000100 */
    0x08, /* 00001000 */
    0x10, /* 00010000 */
    0x20, /* 00100000 */
    0x40, /* 01000000 */
    0x80 /* 10000000 */};

static const unsigned char mask2[8] = {
    0xFE, /* 11111110 */
    0xFD, /* 11111101 */
    0xFB, /* 11111011 */
    0xF7, /* 11110111 */
    0xEF, /* 11101111 */
    0xDF, /* 11011111 */
    0xBF, /* 10111111 */
    0x7F /* 01111111 */
};

inline BitSet bitset_new(int bitsCount) {
    BitSet bs;// = (BitSet *) malloc(sizeof (BitSet));
    bs._count = 0;
    bs._len = bitsCount;
    // the real length of _mblock is (bitsCount / BITS_PER_CHAR + 1) in Byte
    bs._len2 = bs._len / BITS_PER_CHAR + 1;
    bs._mblock = (char *) malloc(sizeof (char) * bs._len2);
    memset(bs._mblock, 0, sizeof (char) * bs._len2);

    return bs;
}

inline BitSet bitset_new2(const char *bits) {
    int i, len;
    BitSet bs;
    len = strlen(bits);
    bs = bitset_new(len);
    for (i = 0; i < len; ++i)
        if (bits[i] == '1')
            bitset_reset(bs, i);
    return bs;
}

inline void bitset_free(BitSet *bs) {
    if (bs) {
        free(bs->_mblock);
        free(bs);
    }
}

inline void bitset_free(BitSet &bs) {
    free(bs._mblock);
}

inline int bitset_size(BitSet &bs) {
    return bs._len;
}

inline int bitset_count(BitSet &bs) {
    return bs._count;
}

inline void bitset_reset_all(BitSet &bs) {
    memset(bs._mblock, 0x00, bs._len2);
    bs._count = 0;
}

inline void bitset_reset(BitSet &bs, int pos) {
    int i, j;

    if (pos >= bs._len) {
        fprintf(stderr, "bit postion :%d is invalid!\n", pos);
        return;
    }

    i = pos / BITS_PER_CHAR;
    j = pos - i * BITS_PER_CHAR;
    if (bitset_isset(bs, pos)) {
        bs._mblock[i] &= mask2[j];
        bs._count--;
    }

}

inline void bitset_set_all(BitSet &bs) {
    memset(bs._mblock, 0xFF, bs._len2);
    bs._count = bs._len;
}

inline void bitset_set(BitSet &bs, int pos) {
    int i, j;

    if (pos >= bs._len) {
        fprintf(stderr, "bit postion :%d is invalid!\n", pos);
        return;
    }

    i = pos / BITS_PER_CHAR;
    j = pos - i * BITS_PER_CHAR;
    if (!bitset_isset(bs, pos)) {
        bs._count++;
        bs._mblock[i] |= mask1[j];
    }
}

inline int bitset_isset(BitSet &bs, int pos) {
    int i, j;

    if (pos >= bs._len) {
        fprintf(stderr, "bit postion :%d is invalid!\n", pos);
        return 0;
    }

    i = pos / BITS_PER_CHAR;
    j = pos - i * BITS_PER_CHAR;
    if (bs._mblock[i] & mask1[j])
        return 1;
    return 0;
}

/**
 * Added by Brandon Malone (bmmalone@gmail.com)
 * @param bs
 */
inline void bitset_flip_all(BitSet &bs) {
    for (int i = 0; i < bs._len2; i++) {
        bs._mblock[i] = ~bs._mblock[i];
    }
}

/**
 * Added by Brandon Malone (bmmalone@gmail.com)
 * @param bs
 * @param pos
 * @return 
 */
inline void bitset_flip(BitSet &bs, int pos) {
    int i, j;

    if (pos >= bs._len) {
        fprintf(stderr, "bit postion :%d is invalid!\n", pos);
        return;
    }

    i = pos / BITS_PER_CHAR;
    j = pos - i * BITS_PER_CHAR;
    bs._mblock[i] = !bs._mblock[i];
}

/**
 *  Added by Brandon Malone (bmmalone@gmail.com)
 */
inline int bitset_first_set_bit(BitSet &bs) {
    // mapping from a char to its first set bit
    static const int lookup_table[] = {0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,};

    /**
     * This can be programmatically generated with this code:
     * 
//    int lookup_table[256];
//    for(unsigned int i = 0; i < 256; i++) {
//        lookup_table[i] = 0;
//        for(int fsb = 7; fsb >= 0; fsb--) {
//            if(i & (1<<fsb)) {
//                lookup_table[i] = fsb;
//            }
//        }
//    }
     */

    int block = 0;
    int pos = 0;

    while (bs._mblock[block] == 0 && pos < bs._len) {
        block++;
        pos += BITS_PER_CHAR;
    }

    if (pos < bs._len) {
        return pos + lookup_table[(unsigned char) (bs._mblock[block])];
    }

    return -1;
}

/**
 *  Added by Brandon Malone (bmmalone@gmail.com)
 */
inline void bitset_and(BitSet &bs, BitSet &other) {
    for (int i = 0; i < bs._len2; ++i) {
//        if (bs._mblock[i] == 0) {
////            first++;
//            continue;
//        } 
//        else 
//        if (other._mblock[i] == 0) {
//            bs._mblock[i] = 0;
////            second++;
//            continue;
//        }
        bs._mblock[i] &= other._mblock[i];
    }
}

inline void bitset_print(BitSet &bs) {
    int i;
    for (i = 0; i < bs._len; ++i)
        printf("%c", (bitset_isset(bs, i) ? '1' : '0'));
    printf("\n");
}

inline char *bitset_to_str(BitSet &bs) {
    int i;
    char *str = (char *) malloc(sizeof (char) * (bs._len + 1));
    memset(str, 0, sizeof (char) * (bs._len + 1));
    for (i = 0; i < bs._len; ++i)
        str[i] = (bitset_isset(bs, i) ? '1' : '0');
    return str;
}

#endif	/* BITSET_H */

