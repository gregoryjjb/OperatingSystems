#include <stdio.h>
#include <stdbool.h>

#define GRID_SIZE 9

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
 * Checks an array of 9 numbers for repeats or missing
 */
bool check_arr(int* arr)
{
    int all_nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    bool any_repeat = false;
    bool any_errors = false;
    bool any_missing = false;
    
    for(int i = 0; i < 9; i++) {
        int index = arr[i] - 1;
        
        if(index > 8 || index < 0) {
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
    for(int i = 0; i < 9; i++) {
        if(all_nums[i] != 0) {
            any_missing = true;
        }
    }
    
    return !any_repeat && !any_errors && !any_missing;
}

bool check_rows(int (*grid)[GRID_SIZE])
{
    bool all_pass = true;
    
    for(int i = 0; i < GRID_SIZE; i++) {
        all_pass = all_pass && check_arr(grid[i]);
    }
    
    return all_pass;
}

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

bool check_square(int (*grid)[GRID_SIZE])
{
    return false;
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

    printf("CS149 Sudoku from Greg Brisebois\n");
    
    print_grid(puzzle1, "Puzzle 1");
    
    bool passes = check_rows(puzzle1) && check_columns(puzzle1);
    
    if(passes) printf("Wow it passed!\n");
    else printf("Boo it didn't pass\n");
    
    while(1) {}
}