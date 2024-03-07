/*
 * File: Sudoku-Validator.c
 * Author: Joel Daniel
 * Date: March 5th, 2024
 * Description: Verifies the validity of a Sudoku puzzle solution using multithreading.
 *              Each thread checks a specific row, column, or subgrid. It outputs the 
 *              validity of each check and summarizes the overall solution's correctness.
 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Define the size of the Sudoku grid and the total number of threads required.
#define SIZE 9
#define NUM_THREADS (SIZE * 3) // 27 threads: 9 for rows, 9 for columns, 9 for subgrids

// Mutex for synchronizing access to shared resources among threads.
pthread_mutex_t mutex;

// Struct to pass parameters to the threads.
typedef struct {
    int row;
    int col;
    int sudoku[SIZE][SIZE];
} parameters;

// Struct to store the validation result and message for each thread.
typedef struct {
    int valid; // 0 for invalid, 1 for valid
    char message[100];
} validationResult;

// Array to store the results from all threads.
validationResult results[NUM_THREADS];


/*
 * Function: checkRow
 * ------------------
 * Validates all numbers in a specific row of the Sudoku grid. Ensures that there are no Duplicates
 *
 * params: Pointer to a parameters struct containing the row to check, the column (unused), and the Sudoku grid.
 *
 * Returns: NULL after printing the validation result and exiting the thread accordingly.
 */

void *checkRow(void *params) {
    parameters *data = (parameters *)params;
    int row = data->row;
    int check[SIZE] = {0};

    // Iterate through each row
    for (int i = 0; i < SIZE; i++) {
        int num = data->sudoku[row][i];

        // Validate the number (it should be between 1 and SIZE inclusive) and checks for duplicates
        if (num < 1 || num > SIZE || check[num - 1]++) {
            // Lock the mutex before accessing shared data to avoid race conditions
            pthread_mutex_lock(&mutex);

            // Mark the row as invalid in the shared results array
            sprintf(results[row].message, "Thread # %2d (row %d) is INVALID\n", row + 1, row + 1);
            results[row].valid = 0;

            // Unlock the mutex after modifying shared data
            pthread_mutex_unlock(&mutex);

            // Exit the thread as we have determined the row is invalid
            pthread_exit(NULL);
        }
    }

    // If we reach here, no duplicates or invalid numbers were found in the row

    pthread_mutex_lock(&mutex);

    // Mark the row as valid in the shared results array
    sprintf(results[row].message, "Thread # %2d (row %d) is valid\n", row + 1, row + 1);
    results[row].valid = 1;

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}


/*
 * Function: checkColumn
 * ---------------------
 * Validates all numbers in a specific column of the Sudoku grid. Ensures that there are no duplicates
 *
 * params: Pointer to a parameters struct containing the column to check, the row (unused), and the Sudoku grid.
 *
 * Returns: NULL after printing the validation result and exiting the thread accordingly.
 */

void *checkColumn(void *params) {
    parameters *data = (parameters *)params;
    int col = data->col;
    int check[SIZE] = {0};

    // Iterate through each column
    for (int i = 0; i < SIZE; i++) {
        int num = data->sudoku[i][col];

        // Validate the number (it should be between 1 and SIZE inclusive) and check for duplicates
        if (num < 1 || num > SIZE || check[num - 1]++) {
            pthread_mutex_lock(&mutex);

            sprintf(results[SIZE + col].message, "Thread # %2d (column %d) is INVALID\n", col + 10, col + 1);
            results[SIZE + col].valid = 0;

            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
    }

    // If we reach here, no duplicates or invalid numbers were found in the column

    pthread_mutex_lock(&mutex);

    sprintf(results[SIZE + col].message, "Thread # %2d (column %d) is valid\n", col + 10, col + 1);
    results[SIZE + col].valid = 1;

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}


/*
 * Function: checkSubgrid
 * ----------------------
 * Validates all numbers in a specific 3x3 subgrid of the Sudoku grid. Ensures that there are no duplicates
 *
 * params: Pointer to a parameters struct containing the top-left coordinates of the subgrid and the Sudoku grid.
 *
 * Returns: NULL after printing the validation result and exiting the thread accordingly.
 */

void *checkSubgrid(void *params) {
    parameters *data = (parameters *)params;
    int rowStart = data->row;
    int colStart = data->col;
    int check[SIZE] = {0};

    // Calculate the index for results array specifically for subgrids,
    // adjusting it based on its position in the overall thread/task structure
    int index = (rowStart / 3) * 3 + (colStart / 3) + (2 * SIZE); // Adjust index for subgrids

    // Iterate through each cell in the subgrid
    for (int row = rowStart; row < rowStart + 3; row++) {

        for (int col = colStart; col < colStart + 3; col++) {
            int num = data->sudoku[row][col];

            // Validate the number (should be between 1 and SIZE) and check for duplicates
            if (num < 1 || num > SIZE || check[num - 1]++) {
                pthread_mutex_lock(&mutex);

                sprintf(results[index].message, "Thread # %2d (subgrid %d) is INVALID\n", index + 1, index - (2 * SIZE) + 1);
                results[index].valid = 0;

                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
        }

    }

    // If we reach here, no duplicates or invalid numbers were found in the subgrid

    pthread_mutex_lock(&mutex);

    sprintf(results[index].message, "Thread # %2d (subgrid %d) is valid\n", index + 1, index - (2 * SIZE) + 1);
    results[index].valid = 1;
    
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}


/*
 * Function: loadSudoku
 * --------------------
 * Loads the Sudoku puzzle from a plain text file into a 2D integer array.
 *
 * params: 
 *      filename: String path to the file containing the Sudoku puzzle.
 *      sudoku: 2D array (9x9) to store the Sudoku puzzle numbers.
 *
 * Returns: void. Exits the program on file read error.
 */

void loadSudoku(const char *filename, int sudoku[SIZE][SIZE]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fscanf(file, "%d", &sudoku[i][j]);
        }
    }
    fclose(file);
}


/*
 * Function: printResults
 * ----------------------
 * Prints the validation results for each row, column, and subgrid checked by the threads.
 *
 * Returns: void.
 */

void printResults() {
    for (int i = 0; i < NUM_THREADS; i++) {
        if (results[i].valid) {
            printf("%s", results[i].message);
        } else {
            printf("%s", results[i].message);
        }
    }
}


/*
 * Function: main
 * --------------
 * Program's entry point. Verifies the validity of a Sudoku puzzle solution using multithreading and prints the outcome.
 *
 * argc: The number of command-line arguments.
 * argv: Array of command-line arguments.
 *
 * Returns: Exits with EXIT_SUCCESS if the program runs successfully, or EXIT_FAILURE on error.
 */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <sudoku_puzzle_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int sudoku[SIZE][SIZE];
    loadSudoku(argv[1], sudoku);
    
    pthread_mutex_init(&mutex, NULL);
    
    pthread_t tids[NUM_THREADS];
    parameters params[NUM_THREADS];
    
    // Initialize parameters for each thread
    for (int i = 0; i < SIZE; i++) {
        // Initialize row checkers
        params[i].row = i;
        params[i].col = 0;
        memcpy(params[i].sudoku, sudoku, SIZE*SIZE*sizeof(int));
        pthread_create(&tids[i], NULL, checkRow, &params[i]);

        // Initialize column checkers
        params[SIZE + i].row = 0;
        params[SIZE + i].col = i;
        memcpy(params[SIZE + i].sudoku, sudoku, SIZE*SIZE*sizeof(int));
        pthread_create(&tids[SIZE + i], NULL, checkColumn, &params[SIZE + i]);
    }

    // Initialize subgrid checkers
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            int index = 2*SIZE + (i + j/3);
            params[index].row = i;
            params[index].col = j;
            memcpy(params[index].sudoku, sudoku, SIZE*SIZE*sizeof(int));
            pthread_create(&tids[index], NULL, checkSubgrid, &params[index]);
        }
    }
    
    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    // Check results and determine overall validity
    bool isValid = true; // Assume the solution is valid initially
    for (int i = 0; i < NUM_THREADS; i++) {
        if (results[i].valid == 0) { // If any thread found the solution to be invalid
            isValid = false;
            break;
        }
    }

    // Print results
    printResults();

    pthread_mutex_destroy(&mutex);

    // Print the final outcome
    if (isValid) {
        printf("%s contains a valid solution\n", argv[1]);
    } else {
        printf("%s contains an INVALID solution\n", argv[1]);
    }

    return EXIT_SUCCESS;
}
