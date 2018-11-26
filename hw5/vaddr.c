#include <stdio.h>
#include <stdlib.h>

#define KB 1024

int main(int argc, char const *argv[])
{
    printf("Virtual address translation by Greg Brisebois 004\n");
    
    if(argc < 3) {
        printf("Please pass in 2 numbers\n");
        return 1;
    }
    
    char* none;
    
    int n = strtol(argv[1], &none, 0);
    int virtual_address = strtol(argv[2], &none, 0);
    
    if(n < 1) {
        printf("N must be >= 1\n");
        return 1;
    }
    
    int page_size = KB * n;
    int page_num = virtual_address / page_size;
    int offset = virtual_address - (page_num * page_size);
    
    printf("Page size = %d, virtual address %d => page number => %d, offset = %d\n", page_size, virtual_address, page_num, offset);
    
    return 0;
}
