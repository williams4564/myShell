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

/*
char *strtok_r(char *str, const char *delim, char **save)
{
    char *res, *last;

    if( !save )
        return strtok(str, delim);
    if( !str && !(str = *save) )
        return NULL;
    last = str + strlen(str);
    if( (*save = res = strtok(str, delim)) )
    {
        *save += strlen(res);
        if( *save < last )
            (*save)++;
        else
            *save = NULL;
    }
    return res;
}
*/
int getNumChar(char **array, int start, int end){

    int i;
    int count = 0;
    for(i = start; i < end; i++){
        count += strlen(array[i]);
    }
    return count;
}

char **get_arr_ofStrings(int length, char* string, int argc, char **og_arr){
    char **string_arr = malloc(argc * sizeof(string)*strlen(string));
    int j = 0;
    char *delim = " \t\n";
    //char *ptr = strtok(string, delim);

    //og_arr[j] =
    char *ptr = strtok(string, delim);
 //   og_arr[j] = strtok(string, ' ');
  //  char *ptr = string;


  //  char *ptr = og_arr[j];
    while(ptr){
      //  string_arr[j] = ptr;
       // strcpy(og_arr[j], ptr);
       printf("tokens:%s\n", ptr);
       //strcpy(string_arr[j] , ptr);
       string_arr[j] = ptr;
        ptr = strtok((char*)0, delim);
        j++;
    }
    return string_arr;

}
/*
char **get_me_some_strings() {
  char *ary[] = { "ABC", "BCD", NULL };
  char **strings = ary; // a pointer to a pointer, for easy iteration
  char **to_be_returned = malloc(sizeof(char*) * 3);
  int i = 0;
  while(*strings) {
    to_be_returned[i] = malloc( sizeof(char) * strlen( *strings ) );
    strcpy( to_be_returned[i++], *strings);
    strings++;
  }
  return to_be_returned;
}
void i_need_me_some_strings() {
  char **strings = get_me_some_strings();
  while(*strings) {
    printf("a fine string that says: %s", *strings);
    strings++;
  }
}
*/

int main(){
    char str[] = "split into tokens\n\0";
    int len = strlen(str); 
    //char *array[len];
    char  **array;
    array = get_arr_ofStrings(len, str, 5, array);
    int count = getNumChar(array, 0, 3);
    printf("= %d\n", count);
    return 0;
}

