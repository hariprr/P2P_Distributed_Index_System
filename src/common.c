#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int split_strings(char **string, char splitStrings[100][100])
{
    fflush(NULL);
    printf("Received String of length %zu\n", strlen(*string));
    int str_len = strlen(*string);
    char *str = malloc(str_len);

    strcpy(str, *string);

    //char splitStrings[100][100]; //can store 10 words of 10 characters
    int i,j,cnt;

//    printf("input string length = %d\n", strlen(string));
    j=0; cnt=0;
    for(i=0;i<=(strlen(str));i++)
    {
        // if space or NULL found, assign NULL into splitStrings[cnt]
        if(str[i]==' ' ||str[i]=='\0')
        {
            splitStrings[cnt][j]='\0';
            cnt++;  //for next word
            j=0;    //for next word, init index to 0
        }
        else
        {
            splitStrings[cnt][j]=str[i];    // append the char to output string
            j++;    // move to next char in input string
        }
    }

    return cnt;
}

