#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

#define MAXLINE 80
#define DEBUG false

// Splits a string of arguments into an array of strings
void split(char* source, char* dest[], char* delim)
{
    char* t = strtok(source, delim);
    int i = 0;
    while(t != NULL) {
        dest[i] = t;
        i += 1;
        t = strtok(NULL, delim);
    }
    dest[i] = NULL;
}

int main()
{
    char* args[MAXLINE / 2 + 1];
    bool shouldRun = true;

    printf("CS149 Shell from Greg Brisebois\n");
    
    while(shouldRun)
    {
        printf("Greg-004> ");
        fflush(stdout);

        char input[MAXLINE];

        if(fgets(input, MAXLINE, stdin) != NULL)
        {
            if(DEBUG) printf("@  You entered %s\n", input);
            
            // Strip newline
            strtok(input, "\n");
            
            // Split args on space
            split(input, args, " ");
            
            // Print args
            int i = 0;
            while(args[i] != NULL) {
                if(DEBUG) printf("@  Arg %d: %s\n", i, args[i]);
                i += 1;
            }
            
            // Check for 'exit' command
            if(strcmp(args[0], "exit") == 0) {
                printf("Goodbye\n");
                return 1;
            }
            
            // Check for ampersand
            bool ampersand = false;
            int last = -1;
            while(args[last + 1] != NULL) {
                last += 1;
            }
            if(DEBUG) printf("@  Last arg index is %d '%s'\n", last, args[last]);
            if(strcmp(args[last], "&") == 0) {
                if(DEBUG) printf("@  Ampersand passed");
                ampersand = true;
                args[last] = NULL;
            }
            
            // Fork off new process
            __pid_t pid = fork();
            
            // Either run command or wait
            if(pid == -1) {
                printf("Error executing command; fork failed\n");
            }
            if(pid != 0) {
                // Parent process
                if(DEBUG) printf("@  This is the parent %d\n", pid);
                int status = 0;
                
                // Wait for child process
                if(!ampersand) {
                    waitpid(pid, &status, 0);
                }
            }
            else {
                // Child process
                shouldRun = false;
                
                if(ampersand) {
                    // Redirect stdout to devnull
                    freopen("/dev/null", "w", stdout);
                }
                
                if(DEBUG) printf("@  This is the child %d\n", pid);
                
                // Start new process
                __pid_t cmdPid = execvp(args[0], args);
                
                if(DEBUG) printf("@  Command PID %d\n", cmdPid);
                
                if(cmdPid == -1) {
                    printf("Error executing command '%s'\n", args[0]);
                }
            }
        }
        else {
            printf("No input\n");
        }
    }

    return 1;
}
