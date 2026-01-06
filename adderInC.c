#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

//sanitizer - replaces negative elements with 0
void sanitizeInC(int *array, int count) {
    for (int i = 0; i < count; i++) {
        if (array[i] < 0) array[i] = 0;
    }
}

// simple reference sum (no overflow handling)

int simpleSum(int *numbers, int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += numbers[i];
    }
    return sum;
}

//adder that performs addition, detects overflow in 32 bits (status=0 case)
//in case of overflow, starts implementing checksum in 64-bits (status=1 case)
//in case of overflow in 64-bits, returns error (rare, hard to get) (status=2 case)

int *adderInC(int *numbers, int count) {
    static int result[3];// result = {status, low32, high32}
    int32_t sum_lo = 0;
    int    status = 0;

    // 1) fast (32-bit) loop
    for (int i = 0; i < count; i++) {
        int32_t x   = numbers[i];
        int32_t old = sum_lo;
        sum_lo     += x;
        if ((x > 0 && sum_lo <  old) || (x < 0 && sum_lo >  old)) {
            status = 1;
            break;
        }
    }
    if (status == 0) {
        result[0] = 0; // fast version status
        result[1] = sum_lo;
        result[2] = 0;
        return result;
    }

    // 2) Wide (64-bit) loop
    uint64_t wide = 0;
    for (int i = 0; i < count; i++) {
        wide += (uint32_t)numbers[i];
    }
    uint32_t lo = (uint32_t) wide;
    uint32_t hi = (uint32_t)(wide >> 32);


    // 3) Double‐overflow check (if it overflows in 64 bits aswell)
    if (hi > (uint32_t)INT32_MAX) {
        result[0] = 2; // error
        result[1] = 0;
        result[2] = 0;
    } else {
        result[0] = 1; // wide version
        result[1] = lo;
        result[2] = hi;
    }
    return result;
}
