/* 
 * File:   typedefs.h
 * Author: malone
 *
 * Created on October 22, 2012, 10:33 PM
 */

#ifndef TYPEDEFS_H
#define	TYPEDEFS_H

#include <stdint.h>

#include <string>
#include <sstream>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// declare the BYTE data type
typedef uint8_t byte;

// convenience method to easily cast (some) things to string
#include <boost/lexical_cast.hpp>
#define TO_STRING(s) (boost::lexical_cast<std::string>(s))

/**
 *  Decide how to represent variable sets.
 *  Please note that the macro VARSET_NEW is quite sensitive.  Caveat emptor.
 *  Similarly, VARSET_COPY is very sensitive.
 */
//#define BOOST_VARSET
#define NATIVE_VARSET

/**
 *  Decide how to handle dynamic bitsets (i.e., for sparse parent graphs).
 */
#define BOOST_BITSET
//#define BITMAGIC_BITSET
//#define CUSTOM_BITSET

/**
 *  Decide how to handle unordered maps (i.e., for duplicate detection).
 */
#define BOOST_MAP
//#define GOOGLE_SPARSE_MAP
//#define GOOGLE_DENSE_MAP
//#define STD_MAP




/**
 *  Macros related to variable sets.
 */
#ifdef BOOST_VARSET
#include <boost/dynamic_bitset.hpp>
#include "dynamic_bitset_hash.h"
typedef boost::dynamic_bitset<> varset;

#define VARSET_NEW(varname, size) varset varname(size)
#define VARSET_NEW_INIT(varname, size, value) varset varname(size, value)
#define VARSET_SET(vs, index) (vs.set(index))
#define VARSET_SET_ALL(vs, maxIndex) (vs.set())
#define VARSET_CLEAR(vs, index) (vs.set(index, false))
#define VARSET_GET(vs, index) (vs.test(index))
#define VARSET_LESS_THAN(vs, other) (vs < other)
#define VARSET_IS_SUBSET_OF(vs, other) (vs.is_subset_of(other))
#define VARSET_FIND_FIRST_SET(vs) (vs.find_first())
#define VARSET_NOT(vs) (varset(vs).flip())
#define VARSET_AND(vs, other) (vs & other)
#define VARSET_COPY(vs, other) varset other(vs)

inline byte cardinality(varset &vs) {
    return vs.count();
}

inline varset nextPermutation(varset &vs) {
    unsigned long variables = vs.to_ulong();
    unsigned long temp = (variables | (variables - 1)) + 1;
    unsigned long nextVariables(temp | ((((temp & -temp) / (variables & -variables)) >> 1) - 1));
    return varset(vs.size(), nextVariables);
}

inline varset varsetClearCopy(const varset &vs, int index) {
    varset cp(vs);
    cp.set(index, false);
    return cp;
}

#elif defined NATIVE_VARSET
#include <string.h>
typedef uint64_t varset;
#define VARSET_NEW(varname, size) varset varname(0)
#define VARSET(size) varset(0)
#define VARSET_NEW_INIT(varname, size, value) varset varname(value)
#define VARSET_SET(vs, index) (vs |= (1L << index))
#define VARSET_SET_VALUE(vs, value) (vs = varset(value))
#define VARSET_SET_ALL(vs, maxIndex) (vs |= (uint64_t)pow(2, maxIndex) - 1)
#define VARSET_CLEAR(vs, index) (vs ^= (1L << index))
#define VARSET_CLEAR_ALL(vs) (vs = 0)
#define VARSET_GET(vs, index) (vs & (1L << index))
#define VARSET_LESS_THAN(vs, other) (vs < other)
#define VARSET_IS_SUBSET_OF(vs, other) ((vs & other) == vs)
#define VARSET_FIND_FIRST_SET(vs) (ffsl(vs) - 1) // -1 because, if the first bit is set, ffsl returns 1 (expecting 0)
#define VARSET_FIND_NEXT_SET(vs, index) (index + ffsl(vs << index))
#define VARSET_NOT(vs) (~vs)
#define VARSET_AND(vs, other) (vs & other)
#define VARSET_COPY(vs, other) varset other(vs)

/**
 * Taken from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan.
 * 
 * @param v the variable set to count
 * @return 
 **/
byte inline cardinality(varset v) {
    byte layer = 0;
    for (layer = 0; v; layer++) {
        v &= v - 1; // clear the least significant bit set
    }
    return layer;
}

inline varset nextPermutation(varset &vs) {
    varset nextVariables; // next permutation of bits
    varset temp = (vs | (vs - 1)) + 1;
    nextVariables = temp | ((((temp & -temp) / (vs & -vs)) >> 1) - 1);
    return nextVariables;
}

inline varset varsetClearCopy(const varset &vs, int index) {
    return vs ^ (1L << index);
}

inline std::string varsetToString(const varset &vs) {
    std::ostringstream os;
    os << "{";
    for (int i = 0; i < 64; i++) {
        if (vs & (1L << i)) {
            os << i << ", ";
        }
    }
    os << "}";
    return os.str();
}

inline int previousSetBit(varset &vs, int index) {
    for (int i = index-1; i >= 0; i--) {
        if (VARSET_GET(vs, i)) {
            return i;
        }
    }
    return -1;
}

inline int lastSetBit(varset vs) {
    return previousSetBit(vs, 64);
}
#endif

/**
 *  Macros related to dynamic bitsets.
 */
#ifdef BOOST_BITSET
#include <boost/dynamic_bitset.hpp>
typedef boost::dynamic_bitset<> bitset;

#define BITSET_FREE(bs) (delete bs)
#define BITSET_CREATE(bs, size) boost::dynamic_bitset<> bs(size)
#define BITSET_NEW(size) (boost::dynamic_bitset<>(size))
#define BITSET_SET(bs, index) (bs.set(index))
#define BITSET_FLIP_ALL(bs) (bs.flip())
#define BITSET_SET_ALL(bs) (bs.set())
#define BITSET_AND(bs, other) (bs &= other)
#define BITSET_OR(bs, other) (bs |= other)
#define BITSET_FIRST_SET_BIT(bs) (bs.find_first())
#define BITSET_CLEAR(bs) (bs.reset())
#define BITSET_COUNT(bs) (bs.count())
#define BITSET_COPY(bs, other) boost::dynamic_bitset<> other(bs)


#elif defined BITMAGIC_BITSET
#include <bm/bm.h>
typedef bm::bvector<> bitset;

#define BITSET_FREE(bs) (delete bs)
#define BITSET_NEW(size) (bm::bvector<>(size))
#define BITSET_SET(bs, index) (bs.set_bit(index))
#define BITSET_FLIP_ALL(bs) (bs.flip())
#define BITSET_SET_ALL(bs) (bs.set())
#define BITSET_AND(bs, other) (bs &= other)
#define BITSET_FIRST_SET_BIT(bs) (bs.get_first())



#elif defined CUSTOM_BITSET
#include "bitset.h"
typedef struct BitSetStruct bitset;

#define BITSET_FREE(bs) (bitset_free(bs))
#define BITSET_NEW(size) (bitset_new(size))
#define BITSET_SET(bs, index) (bitset_set(bs, index))
#define BITSET_FLIP_ALL(bs) (bitset_flip_all(bs))
#define BITSET_SET_ALL(bs) (bitset_set_all(bs))
#define BITSET_AND(bs, other) (bitset_and(bs, other))
#define BITSET_FIRST_SET_BIT(bs) (bitset_first_set_bit(bs))
#endif

/**
 *  Macros related to unordered maps.
 */

class Node;
class DFSNode;

#ifdef BOOST_MAP
#include <boost/unordered_map.hpp>
typedef boost::unordered_map<varset, Node*> NodeMap;
typedef boost::unordered_map<varset, DFSNode*> DFSNodeMap;
typedef boost::unordered_map<varset, float> FloatMap;


#elif defined GOOGLE_SPARSE_MAP
#include <google/sparse_hash_map>
typedef google::sparse_hash_map<varset, Node*> NodeMap;
typedef google::sparse_hash_map<varset, DFSNode*> DFSNodeMap;
typdef google::sparse_hash_map<varset, float> FloatMap;


#elif defined GOOGLE_DENSE_MAP
#include <google/dense_hash_map>
typedef google::dense_hash_map<varset, Node*> NodeMap;
typedef google::dense_hash_map<varset, DFSNode*> DFSNodeMap;
typdef google::dense_hash_map<varset, float> FloatMap;


#elif defined STD_MAP
#include <unordered_map>
typedef std::unordered_map<varset, Node*> NodeMap;
typedef std::unordered_map<varset, DFSNode*> DFSNodeMap;
typedef std::unordered_map<varset, float> FloatMap;


#endif

inline void init_map(NodeMap &map) {
#if defined(GOOGLE_SPARSE_MAP) || defined(GOOGLE_DENSE_MAP)
    map.set_deleted_key(-2);
    map.set_empty_key(-1);
#endif
}

inline void init_map(DFSNodeMap &map) {
#if defined(GOOGLE_SPARSE_MAP) || defined(GOOGLE_DENSE_MAP)
    map.set_deleted_key(-2);
    map.set_empty_key(-1);
#endif
}

inline void init_map(FloatMap &map) {
#if defined(GOOGLE_SPARSE_MAP) || defined(GOOGLE_DENSE_MAP)
    map.set_deleted_key(-2);
    map.set_empty_key(-1);
#endif
}

inline void init_map(NodeMap *map) {
#if defined(GOOGLE_SPARSE_MAP) || defined(GOOGLE_DENSE_MAP)
    map.set_deleted_key(-2);
    map.set_empty_key(-1);
#endif
}

inline void init_map(DFSNodeMap *map) {
#if defined(GOOGLE_SPARSE_MAP) || defined(GOOGLE_DENSE_MAP)
    map.set_deleted_key(-2);
    map.set_empty_key(-1);
#endif
}

inline void init_map(FloatMap *map) {
#if defined(GOOGLE_SPARSE_MAP) || defined(GOOGLE_DENSE_MAP)
    map.set_deleted_key(-2);
    map.set_empty_key(-1);
#endif
}

#endif	/* TYPEDEFS_H */

