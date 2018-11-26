#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    char* devnull = NULL;
    
    long n = 0;
    long v_addr = 0;
    
    printf("Virtual address translation by Greg Brisebois 004\n");
    
    n = strtol(argv[1], &devnull, 0);
    v_addr = strtol(argv[2], &devnull, 0);
    
    printf("V addr = %d, n = %n\n", v_addr, n);
    
    return 0;
}
