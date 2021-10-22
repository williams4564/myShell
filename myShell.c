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
int AND_flag, REDIR_flag, PIPE_flag;

int get_input(char *cmd, char **argv){
    int argc = 0;

    char *delim = " \t\n";  //&& || also?

    //check for 2 commands
    char *substr = strstr(cmd, "&&");
    if(substr){
        AND_flag = 1;
    } else{
        AND_flag = 0;
    }
    char *substr2 = strstr(cmd, "|");
    if(substr2){
        PIPE_flag = 1;
    } else{
        PIPE_flag = 0;
    }
    char *substr3 = strstr(cmd, ">"); char *substr4 = strstr(cmd, "<");
    if(substr3 || substr4){
        REDIR_flag = 1;
    } else
        REDIR_flag = 0;

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

struct command {
  const char **argv;
};
int spawn_proc (int in, int out, struct command *cmd) {
  pid_t pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }

      return execvp (cmd->argv [0], (char * const *)cmd->argv);
    }

  return pid;
}
int fork_pipes (int n, struct command *cmd) {
  int i;
  pid_t pid;
  int in, fd [2];

  /* The first process should get its input from the original file descriptor 0.  */
  in = 0;

  /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
  for (i = 0; i < n - 1; ++i)
    {
      pipe (fd);

      /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
      spawn_proc (in, fd [1], cmd + i);

      /* No need for the write end of the pipe, the child will write here.  */
      close (fd [1]);

      /* Keep the read end of the pipe, the next child will read from there.  */
      in = fd [0];
    }

  /* Last stage of the pipeline - set stdin be the read end of the previous pipe
     and output to the original file descriptor 1. */  
  if (in != 0)
    dup2 (in, 0);

  /* Execute the last stage with the current process. */
  return execvp (cmd [i].argv [0], (char * const *)cmd [i].argv);
}
void Handle_Redir(int argc, char *argv[])
{
    int out_loc = 0;
    int  in_loc = 0;
    int i;
    for(i = 0; i<argc; i++){
        if(strcmp(argv[i], ">") ==0 ) {
            if(out_loc != 0){
                fprintf(stderr, "Cannot output more than one file \n");
                _exit(EXIT_FAILURE);
            }
            else if(i==0){
                fprintf(stderr, "No command entered \n");
                _exit(EXIT_FAILURE);
            }
            out_loc = i;
        }
        else if(strcmp(argv[i], "<")==0){
            if(in_loc !=0){
                fprintf(stderr, "Cannot input more than one file  \n");
                _exit(EXIT_FAILURE);
            }
            else if(i==0){
                fprintf(stderr,"No command entered \n");
                _exit(EXIT_FAILURE);
            }
            in_loc = i;
        }
    }
    int fd;
    if(out_loc !=0){
        if(argv[out_loc+1] == NULL){
            fprintf(stderr, "There is no file \n");
            _exit(EXIT_FAILURE);
        }
       fd = open(argv[out_loc+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    
    if(fd ==-1){
       // fprintf(stderr, "Error opening out_loc file \n");
       perror("Error opening out_loc file\n");
        _exit(EXIT_FAILURE);    
    }
    dup2(fd, 1);
    close(fd);
    argv[out_loc] = NULL;
     }

     int fid;
     if(in_loc != 0){
        if(argv[in_loc +1] == NULL){
            fprintf(stderr, "There is no file \n");
            _exit(EXIT_FAILURE);
        }
        fid = open(argv[in_loc + 1], O_RDONLY, S_IRUSR | S_IWUSR);
        if(fid == -1){
            perror( "Error opening at in_loc file \n");
            _exit(EXIT_FAILURE);
        }
        dup2(fid, 0);
        close(fid);
        argv[in_loc] = NULL;
     }



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
    //OR_flag = 0;
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
        pid = fork();  //COMMENTED OUT FOR DEBUGGING ONLY
        if(pid < 0){
            printf("Fork failed");
            exit(1);
        }
        if(pid == 0){
            if(REDIR_flag == 1){
                Handle_Redir(argc, input);
            }
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
            } else if(PIPE_flag == 1){
                int fd1[2];
               // int fd2[2];
                //find where | is and seperate commands
                int k = 0;
                int location;
                do{
                    printf("input+%d =%s\n",k,  *(input + k));
                    char *substr = strstr(*(input + k) , "|");
                    if(substr){
                        location = substr; //- *(input + k);
                        printf("Location in input array = %d\n", k); //*(input + k) - substr); //- *(input + k));
                        break;
                    }
                    else{
                        k++;
                    }
                    
                }while(*(input + k) != '\0');
                char **com1 = getptrArray(input, 0, k);
                char **com2 = getptrArray(input, k+1, argc);
                int stdin_save, stdout_save;
                  /* Duplicate stdin and stdout so we can restore them later. */
              //  stdin_save = dup(STDIN_FILENO);
              //  stdout_save = dup(STDOUT_FILENO);
                struct command test_cmd [] = {{com1}, {com2} };
                fork_pipes(2, test_cmd);
                
                /*
                pid_t p_pid;
                char *cwd;
               // getcwd(cwd, 256);
                if(pipe(fd1) == -1){
                    perror("Pipe #1 failure");
                    exit(1);
                }
                if( (p_pid = fork()) == -1){  //received signal SIGTTIN, Stopped (tty input).
                    close(fd1[1]);
                    perror("Fork failure");
                    exit(1);
                }
                if(p_pid == 0){
                    //setenv("parent", getcwd(cwd, 256), 1);
                    //close(fd1[1]);
                    dup2(fd1[0], 0);
                    //dup2(fd1[0], STDIN_FILENO);
                    close(fd1[1]);
                    if(execvp(com1[0], com1) == -1){  //program ends here
                        perror("exec error");
                        exit(1);
                    }
                } else {
                    //wait(NULL);
                    
                    //close(fd1[0]);
                   // close(fd1[1]);
                    dup2(fd1[1], 1);
                   // close(fd1[1]);
                    close(fd1[0]);
                    if(execvp(com2[0], com2) == -1){
                        perror("exec error");
                        exit(1);
                    }
                }
              //  close(fd1[0]);
                //close(fd1[1]);
            */
            }
            //waitpid(pid, NULL, 0);
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
