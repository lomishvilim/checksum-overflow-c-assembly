# Checksum with Overflow Handling (C & Assembly)

An implementation of an **integer checksum** written in **C and x86 assembly**,
designed to detect 32-bit overflow and fall back to a wider accumulation when needed.

The project compares C and assembly implementations side by side and focuses on
low-level correctness, flag handling, and explicit control over arithmetic behavior.

## Overview
- Computes a checksum over an array of signed integers
- Uses a fast 32-bit accumulation path
- Detects overflow explicitly in assembly
- Falls back to a 64-bit accumulation when overflow occurs
- Returns both the checksum value and an overflow indicator

## Implementation
- **C version** — reference implementation
- **Assembly version** — explicit overflow detection using processor flags
- Separate sanitizer functions to validate input data
- Results from C and assembly versions are compared for correctness

## Structure
- `main.c` — driver and test cases
- `adderInC.c` / `adderInAsm.S` — checksum logic
- `sanitizeInC.c` (inside `adderInC.c` file ) / `sanitizeInAsm.S` — input validation
- `report.pdf` — design explanation and test discussion

## Build & Run

```bash
gcc -m32 -Wall -Wextra -O2 main.c adderInC.c adderInAsm.S sanitizeInAsm.S -o checksum
./checksum
```
(32-bit build required for the assembly version.)

Note:
This project was done as part of a university course. Because these are very common assignments,
I had to prioritize writing an original solution that wouldn’t trigger plagiarism checks, rather
than fully optimizing style or structure.