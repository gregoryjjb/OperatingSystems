#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define DEBUG false
#define GRID_SIZE 9

typedef struct
{
    int (*grid)[GRID_SIZE];
    int row;
    int column;
    bool result;
} parameters;

/**
 * Construct a new parameter struct
 */
parameters* gen_params(int (*grid)[GRID_SIZE])
{
    parameters* p = (parameters *) malloc(sizeof(parameters));
    p->grid = grid;
    return p;
}

/**
 * Prints out a grid (puzzle) to the console
 */
void print_grid(int (*grid)[GRID_SIZE], char* name)
{
    printf("%s\n", name);
    
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

/**
 * Checks an array of numbers for repeated or missing values
 */
bool check_arr(int* arr)
{
    int all_nums[GRID_SIZE];
    for(int i = 0; i < GRID_SIZE; i++) {
        all_nums[i] = i + 1;
    }
    
    bool any_repeat = false;
    bool any_errors = false;
    bool any_missing = false;
    
    for(int i = 0; i < GRID_SIZE; i++) {
        int index = arr[i] - 1;
        
        if(index >= GRID_SIZE || index < 0) {
            // Check for out of range
            any_errors = true;
        }
        else {
            // Check for repeating numbers
            if(all_nums[index] == 0) {
                any_repeat = true;
            }
            all_nums[index] = 0;
        }
    }
    
    // Check for missing numbers
    // (This might be redundant?)
    for(int i = 0; i < GRID_SIZE; i++) {
        if(all_nums[i] != 0) {
            any_missing = true;
        }
    }
    
    return !any_repeat && !any_errors && !any_missing;
}

/**
 * Check the rows of a grid for repeated or missing values
 */
bool check_rows(int (*grid)[GRID_SIZE])
{
    bool all_pass = true;
    
    for(int i = 0; i < GRID_SIZE; i++) {
        all_pass = all_pass && check_arr(grid[i]);
    }
    
    return all_pass;
}

/**
 * Check the columns of a grid for repeated or missing values
 */
bool check_columns(int (*grid)[GRID_SIZE])
{
    bool all_pass = true;
    
    for(int i = 0; i < GRID_SIZE; i++) {
        int column[GRID_SIZE];
        
        for(int j = 0; j < GRID_SIZE; j++) {
            column[j] = grid[j][i];
        }
        
        all_pass = all_pass && check_arr(column);
    }
    
    return all_pass;
}

/**
 * Check a 3x3 square of a grid for repeated or missing values
 * Coordinates range from 0 - (GRID_SIZE/3)
 */
bool check_square(int (*grid)[GRID_SIZE], int square_row, int square_column)
{
    int row = square_row * 3;
    int column = square_column * 3;
    
    if (row >= GRID_SIZE || row < 0 || column >= GRID_SIZE || column < 0)
        return false;

    int square[GRID_SIZE];
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            int r = row + i;
            int c = column + j;
            int index = 3 * i + j;
            square[index] = grid[r][c];
        }
    }
    
    return check_arr(square);
}

/**
 * Threadified version of check_rows
 */
void* thread_rows(void* params)
{
    if(DEBUG) printf("Thread checking rows\n");
    
    parameters* p = (parameters*) params;
    p->result = check_rows(p->grid);
    
    return NULL;
}

/**
 * Threadified version of check_columns
 */
void* thread_cols(void* params)
{
    if(DEBUG) printf("Thread checking columns\n");
    
    parameters* p = (parameters*) params;
    p->result = check_columns(p->grid);
    
    return NULL;
}

/**
 * Threadified version of check_square
 */
void* thread_square(void* params)
{
    parameters* p = (parameters*) params;
    p->result = check_square(p->grid, p->row, p->column);
    
    return NULL;
}

/**
 * Create and handle the threads to check all aspects
 * of the puzzle
 */
bool check_sudoku(int (*grid)[GRID_SIZE])
{
    pthread_t row_thread, col_thread;
    pthread_t square_threads[9];
    
    // Initialize param structs
    parameters* p_rows = gen_params(grid);
    parameters* p_cols = gen_params(grid);
    parameters* p_squares[9];
    
    // Start the threads for rows and columns
    pthread_create(&row_thread, NULL, thread_rows, (void*) p_rows);
    pthread_create(&col_thread, NULL, thread_cols, (void*) p_cols);
    
    // Start the threads for the squares
    for(int i = 0; i < 9; i++) {
        parameters* p = gen_params(grid);
        p->row = i / 3;
        p->column = i % 3;
        
        p_squares[i] = p;
        
        pthread_create(&square_threads[i], NULL, thread_square, (void*) p);
        
        if(DEBUG) printf("Checking square %d %d\n", p->row, p->column);
    }
    
    // Wait for the threads
    pthread_join(row_thread, NULL);
    pthread_join(col_thread, NULL);
    
    for(int i = 0; i < 9; i++) {
        pthread_join(square_threads[i], NULL);
        
        if(DEBUG) printf("Thread %d returned %d\n", i, p_squares[i]->result);
    }
    
    if(DEBUG) printf("Row thread returned %d\n", p_rows->result);
    if(DEBUG) printf("Column thread returned %d\n", p_cols->result);
    
    // Aggregate results from threads
    bool passing = p_rows->result && p_cols->result;
    for(int i = 0; i < 9; i++) passing = passing && p_squares[i];
    
    // Free parameter struct memory
    free(p_rows);
    free(p_cols);
    for(int i = 0; i < 9; i++) free(p_squares[i]);
    
    return passing;
}

/**
 * Check a puzzle
 */
void sudoku(int (*puzzle)[GRID_SIZE], char* name)
{
    print_grid(puzzle, name);
    
    printf("Checking puzzle...\n");
    
    bool pass = check_sudoku(puzzle);
    
    if(pass) {
        printf("The puzzle passed!\n");
    }
    else {
        printf("The puzzle did not pass\n");
    }
    
    printf("\n");
}

int main()
{
    int puzzle1[GRID_SIZE][GRID_SIZE] = {
        {4, 1, 9, 3, 7, 6, 5, 8, 2},
        {3, 6, 7, 8, 5, 2, 1, 4, 9},
        {2, 8, 5, 1, 9, 4, 3, 7, 6},
        {6, 4, 1, 9, 3, 7, 8, 2, 5},
        {9, 3, 2, 4, 8, 5, 6, 1, 7},
        {5, 7, 8, 2, 6, 1, 4, 9, 3},
        {8, 5, 6, 7, 4, 9, 2, 3, 1},
        {1, 9, 4, 5, 2, 3, 7, 6, 8},
        {7, 2, 3, 6, 1, 8, 9, 5, 4}
    };
    
    int puzzle2[GRID_SIZE][GRID_SIZE] = {
        {7, 3, 9, 8, 4, 6, 1, 2, 5},
        {4, 1, 2, 9, 7, 5, 8, 6, 3},
        {8, 0, 5, 2, 3, 1, 9, 4, 7},
        {5, 4, 7, 6, 9, 3, 2, 1, 8},
        {3, 9, 1, 7, 8, 2, 4, 5, 6},
        {2, 8, 6, 5, 1, 4, 3, 7, 9},
        {9, 2, 8, 4, 6, 7, 5, 3, 1},
        {1, 7, 4, 3, 5, 9, 1, 8, 2},
        {6, 5, 3, 1, 2, 8, 7, 9, 4}
    };

    printf("CS149 Sudoku from Greg Brisebois\n");
    
    sudoku(puzzle1, "Puzzle 1");
    sudoku(puzzle2, "Puzzle 2");
    
    while(1) {}
}