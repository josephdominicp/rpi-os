#ifndef TYPES_H
#define TYPES_H

/*
 * Fixed-width integer types
 * (Equivalent to stdint.h, but self-contained)
 */

typedef unsigned char       uint8_t;
typedef signed char         int8_t;

typedef unsigned short      uint16_t;
typedef signed short        int16_t;

typedef unsigned int        uint32_t;
typedef signed int          int32_t;

typedef unsigned long long  uint64_t;
typedef signed long long    int64_t;

/*
 * Architecture-sized types
 */
typedef uint64_t            size_t;
typedef int64_t             ssize_t;



/*
 * Pointer-sized integer
 */
typedef uint64_t            uintptr_t;

#endif /* TYPES_H */