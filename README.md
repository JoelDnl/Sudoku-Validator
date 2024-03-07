# Sudoku-Validator

## Project Overview
This Sudoku Validator is a multithreaded C program designed to check the validity of Sudoku puzzles. It uses separate threads to validate each row, column, and 3x3 subgrid of the puzzle, ensuring no rules are violated.

## Features
- **Multithreading:** Leverages POSIX threads to validate different sections of the puzzle concurrently.
- **Sudoku Rule Enforcement:** Checks for the uniqueness of numbers in rows, columns, and subgrids.
- **File Input:** Reads Sudoku puzzles from text files for validation.

## Prerequisites
To compile and run this program, you need:

- GCC compiler or any standard C compiler with support for the POSIX thread library (pthread).
- A Unix-like environment (Linux, macOS) or Windows with a POSIX compatibility layer like Cygwin.

## Compilation
Navigate to the project directory in your terminal and run the following command to compile the program:

`gcc -o sudoku_validator sudoku_validator.c -lpthread`

Replace `sudoku_validator.c` with the actual name of your source file.

## Usage
After compilation, you can run the program with:

`./sudoku_validator <path_to_sudoku_file>`

The Sudoku file should be a plain text file containing a 9x9 grid, where each row is represented by a line of numbers separated by spaces.

## File Format

The Sudoku file should contain 9 lines with 9 numbers on each line, separated by spaces. Each number should be between 1 and 9, inclusive. An example Sudoku file might look like either of the provided txt files: *valid_Sudoku.txt*, *invalid_Sudoku.txt*
