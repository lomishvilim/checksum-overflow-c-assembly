#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

extern void sanitizeInAsm(int *array, int count); //also in adderInC file, as it is a small function and shouldn't have its own file
extern void sanitizeInC(int *array, int count);
extern int  simpleSum(int *numbers, int count);

extern int *adderInC(int *numbers, int count);
extern int  adderInAsm(int *numbers, int count,int *sum_lo_out, int *sum_hi_out);

void printArray(const char *label, int *arr, int count) {
    printf("%s:", label);
    for (int i = 0; i < count; i++) printf(" %d", arr[i]);
    printf("\n");
}

void runTest(const char *label, int *arr, int count) {
    //sanitize array, show results
    printArray("Original array", arr, count);
    sanitizeInAsm(arr, count);
    printArray("Sanitized (ASM)", arr, count);
    sanitizeInC(arr, count);
    printArray("Sanitized (C)  ", arr, count);
    printf("\n");

    //normal checksum, for comparison
    int sumSimple = simpleSum(arr, count);
    printf("simpleSum = %d\n", sumSimple);

    //C version to compare with ASM version of the code
    int *resC = adderInC(arr, count);
    int statusC = resC[0];
    int loC = resC[1];
    int hiC = resC[2];
    uint32_t uloC = (uint32_t)loC;
    int uhiC = (uint32_t)hiC;
    uint64_t fullC = ((uint64_t)uhiC << 32) | uloC;
    printf("C (overflow-aware) version: status=%d lo=%u hi=%u full=%" PRIu64 "\n", statusC, uloC, uhiC, fullC);

    int lo_s, hi_s, status = adderInAsm(arr, count, &lo_s, &hi_s);
    uint32_t lo = (uint32_t)lo_s;
    int hi = (uint32_t)hi_s;
    uint64_t full = ((uint64_t)hi << 32) | lo;

    if (status == 2) {
        printf("Error: irrecoverable overflow (over 64 bits)\n");
    } else if (status == 1) {
        printf("ASM wide mode: lo=%u hi=%u full=%" PRIu64 "\n", lo, hi, full);
    } else {
        printf("ASM fast mode: lo=%u hi=%u full=%" PRIu64 "\n", lo, hi, full);
    }
    printf("\n");
}

int main(void) {
    int arrNeg[6]   = { 5, -3, -7, 16, 10, -1 };

    int arrFast[10] = { 1,2,3,4,5,6,7,8,9,10 };

    int arrWide[2]  = { 2000000000, 2000000000 };
    //int arrError[20] = {};

    runTest("Negatives + sanitize", arrNeg, 6);
    runTest("Fast-mode array",      arrFast, 10);
    runTest("Wide-mode array",      arrWide,  2);
    //runTest("Error-mode array",     arrError, 20);

    /* The irrecoverable 64-bit overflow case is difficult to trigger in practice
without extremely large inputs, so it is not exercised here. */



    return 0;
}
