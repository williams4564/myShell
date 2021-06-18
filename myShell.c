//my shell
/*
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#define MAX_IN 100
#define MAX_ARGS 20

static sigjmp_buf env;
//prevents crashing if signal delivered after jump point
//if flag is true then skip call to longjmp and goto function handler
//volatile due to asynch access by multiple threads i.e main and signal handler. atomic access
static volatile sig_atomic_t jump_active = 0;
int AND_flag, OR_flag;

int get_input(char *cmd, char **argv){
    int argc = 0;

    char *delim = " \t\n";  //&& || also?

    //check for 2 commands
    char *substr = strstr(cmd, "&&");
    if(substr){
        AND_flag = 1;
    }
    argv[argc] = strtok(cmd, delim);

    while(argc+1 < MAX_ARGS && argv[argc] != NULL){
        argv[++argc] = strtok((char*)0, delim);
    }
    return argc;
}
char **getptrArray(char **input, int start, int end){
    char **output = malloc((sizeof(input[0]) + strlen(input[0])) * (end+1));
    /*char *delim = " \n";
    
    output[index] = strtok(input)*/
    int index = 0;
    int i = 0;
    for(index=start; index<end; index++){
        output[i] = input[index];
        i++;
    }
    return output;
}
char *concat_strings(char *array[], int start, int end){
    char *final = malloc((sizeof(array[0]) + strlen(array[0])) * (end+1));
    int k;
    char *space = " ";
    
    for(k=start; k < end; k++){
        strncat(final, array[k], strlen(array[k]));
        strncat(final, space, 1); //SEGFAULT here
        //strcat(final, *(array + k));
    }
    return final;
}
char **get_arr_ofStrings(int length, char* string, int argc, char **og_arr){
    char **string_arr = malloc(argc * sizeof(string)*strlen(string));
    int j = 0;
    char *delim = " \t\n";
    char *ptr = strtok(string, delim);
 //   og_arr[j] = strtok(string, ' ');
  //  char *ptr = string;
    while(ptr){
      //  string_arr[j] = ptr;
       // strcpy(og_arr[j], ptr);
       printf("tokens:%s\n", ptr);
       og_arr = ptr;
        ptr = strtok(NULL, delim);
        j++;
    }
    return string_arr;

}
int getNumChar(char **array, int start, int end){

    int i;
    int count = 0;
    for(i = start; i < end; i++){
        count += strlen(array[i]);
    }
    return count;
}


typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);

void sigint_handler(int signo){
    //printf("\nDetected SIGINT Ctrl+C\nmyShell>");
    if(!jump_active)
        return;
    siglongjmp(env, 42);
}
int main(void){

    char cmd[MAX_IN];
    char *input[MAX_ARGS];
    int argc;
    pid_t pid, wpid;
    int status;
    AND_flag = 0;
    OR_flag = 0;
   // signal(SIGINT, sigint_handler);  function less portable
    struct sigaction s;
    s.sa_handler = sigint_handler;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_RESTART;
    sigaction(SIGINT, &s, NULL);
    while(1){
        if(sigsetjmp(env, 1) == 42){
            printf("Restarting myShell...\n");
        }
        jump_active = 1;

        printf("myShell>");
        fgets(cmd, MAX_IN, stdin);
        argc = get_input(cmd, input); //num of arguments

        //empty inputs
        if(!input[0]){
            //free(input);
            //free(cmd);
            continue;
        }
        int i = 0;
        printf("argc=%d \n",argc);
        for(; i < argc; i++){
            printf("argv[%d] = %s\n", i, input[i]);
        }
        if(argc == 1){
            if(strcmp(input[0], "exit") == 0){
                exit(0);
            } else if(strcmp(input[0], "pwd") == 0){
                char temp[256];
                getcwd(temp, 256);
                printf(temp);
                printf("\n");
            }
        }
        else if(strcmp(input[0], "cd") == 0){
            char *dir;
             if(argc==1)
             {
                 dir =  getenv("HOME");   
             }
             else
             {
                dir = input[1];
             }
            if( chdir(dir) ==-1)
            {
                perror("error changing directory");
                continue;
            }
        }
        pid = fork();  //COMMENTED OUT FOR DEBUGGING ONLY!!!!
        if(pid < 0){
            printf("Fork failed");
            exit(1);
        }
        if(pid == 0){
            if(AND_flag == 1){
                //fork again for both commands
                //only continue executing if 1st command works
                //char *substr = strstr(input, "&&");
                int k = 0;
                int location;
                do{
                    printf("input+%d =%s\n",k,  *(input + k));
                    char *substr = strstr(*(input + k) , "&&");
                    if(substr){
                        location = substr; //- *(input + k);
                        printf("Location in input array = %d\n", k); //*(input + k) - substr); //- *(input + k));
                        break;
                    }
                    else{
                        k++;
                    }
                    
                }while(*(input + k) != '\0');
                printf("\nHERE\n");
                int j;
                //concatonate strings into 2 commands, for like ls -l && pwd
                int x = getNumChar(input, 0, k);
                int y = getNumChar(input, k+1, argc);
                char **test1 = getptrArray(input, 0, k);
                char **test2 = getptrArray(input, k+1, argc);

                /*
                char *cm1 =concat_strings(input, 0, k);
                char *cm2 =concat_strings(input, k+1, argc);
                char command_1[x+2];
                char command_2[y+2];
                for(j=0; j<x+2;j++){
                    command_1[j] = *(cm1 + j);
                }
                for(j=0; j<y+2;j++){
                    command_2[j] = *(cm2 + j);
                }
                printf("\nHERE\n");

                printf("Command 1: %s\n", command_1);
                printf("Command 2: %s\n", command_2);
                //need array of char* for execvp to work
                printf("\nHERE\n");
                int c1Len = strlen(command_1);
                int c2Len = strlen(command_2);
                char **cmd_1_arr;
                char **cmd_2_arr;
                
                for(j=0; j < c1Len; j++){
              //      strncpy(cmd_1_arr[j], "00", 2); //SEGFAULT here
                }
                char *store = strtok(command_1, " ");
                cmd_1_arr[0] = store;
                printf("cmd_1_arr:%s", cmd_1_arr[0]);
                
                for(j=1; (j < c1Len) && (cmd_1_arr[j] != NULL); j++){
                    cmd_1_arr[j] = strtok((char*)0, " ");
                    printf("cmd_1_arr:%s", cmd_1_arr[j]);
                    printf("\n");
                }
                char *store2 = strtok(command_2, " ");
                cmd_2_arr[0] = store2;
                printf("cmd_2_arr:%s", cmd_2_arr[0]);
                //int j;
                for(j=1; j < c2Len || (cmd_1_arr[j] != NULL); j++){
                    cmd_2_arr[j] = strtok((char*)0, " ");
                    printf("cmd_2_arr:%s", cmd_2_arr[j]);
                    printf("\n");
                }
                int array1_size, array2_size;
                for(j=0; j < strlen(command_1); j++){
                    char* temp = cmd_1_arr[j];
                    if(strcmp(temp, "00") ==0){
                        break;
                    }
                }
                array1_size = j;
                for(j=0; j < strlen(command_2); j++){
                    char* temp = cmd_1_arr[j];
                    if(strcmp(temp, '`') ==0){
                        break;
                    }
                }
                array2_size = j;
                //get_arr_ofStrings(strlen(command_1), command_1, argc, cmd_1_arr);
                //get_arr_ofStrings(strlen(command_2), command_2, argc, cmd_2_arr);
                
                for(j=0; j < argc; j++){
                    //printf("cmd_1[%d] = %s\n", j, cmd_1_arr[j]);
                }
                for(j=0; j < argc; j++){
                  //  printf("cmd_2[%d] = %s\n", j, cmd_2_arr[j]);
                }
                */



                //using location seperate the 2 commands:
                pid_t and_fork;
                and_fork = fork();
                
                if(and_fork == 0){
                    if(execvp(test1[0], test1) == -1)
                        perror("exec error");
                } else {
                    if(execvp(test2[0], test2) == -1)
                        perror("exec error");
                }
            }
            else if(execvp(input[0], input) == -1)
                perror("exec error");
        } else {
            do{
                printf("parent\n");
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        
        //char *argv[] = {"ls", "-l", "-h", "-a", NULL};
        //execvp(argv[0], argv);
        //break;
    }
}




