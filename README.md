# File Reversal

A C++ based file processing project developed as part of the Git laboratory activity.

## Project Overview

The project implements file reversal and verification operations using C++ and Linux system calls.

## Project Structure

- `src/` - Source code
- `include/` - Header files
- `docs/` - Documentation

## Git Lab

This repository demonstrates Git concepts including commits, branches, merging, stash, reflog, conflict resolution, and tagging.
## File Reversal Modes

The program supports three file reversal modes:

- Flag 0: Block-wise file reversal using a specified block size.
- Flag 1: Complete file reversal.
- Flag 2: Partial range reversal using specified start and end indices.
## Build Instructions

The project uses the GNU C++ compiler.

Compile the source code using:

```bash
g++ src/main.cpp -o fileReversal
## Flag 0 - Block Reversal

Flag 0 performs block-wise reversal of the input file.

Usage:

```bash
./fileReversal <input-file> 0 <block-size>

## Flag 1 - Full File Reversal

Flag 1 reverses the complete contents of the input file.

Usage:

```bash
./fileReversal <input-file> 1

## Flag 2 - Partial Range Reversal

Flag 2 reverses the portions of the file outside a specified range while preserving the selected range.

Usage:

```bash
./fileReversal <input-file> 2 <start-index> <end-index>

## Verification

The project includes verification functionality to check whether the processed file has been generated correctly.

Verification checks include:

- File existence
- File size
- Processed file contents
- File permissions
- Correctness according to the selected reversal flag
## Full File Reversal

The full file reversal mode reverses the complete contents of the input
file and writes the reversed data to the output file while preserving
the original file.
### Full Reversal Usage

The full reversal operation processes the complete input file and
produces a reversed version of its contents. The original input file
remains unchanged.

### Full Reversal Input and Output

The full reversal operation reads the complete input file and generates
the reversed content as output. The input file is preserved so that the
original data remains available for verification.

### Full Reversal Verification

The generated output can be verified by comparing its contents with the
expected reversed representation of the original input file.

## Partial Reversal

Partial reversal processes only a selected portion of the input file.
The specified range is reversed while the remaining contents of the file
are kept unchanged.

### Partial Reversal Range Selection

The partial reversal operation uses a specified starting position and
ending position to identify the portion of the file that should be
reversed.
### Partial Reversal Unchanged Regions

Only the selected range is modified during partial reversal. Data outside
the specified range retains its original ordering.

### Partial Reversal Verification

The result of a partial reversal can be verified by checking that the
selected range appears in reverse order while all other file contents
remain unchanged.

