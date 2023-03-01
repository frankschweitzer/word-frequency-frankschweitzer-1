/**
 * This program reads a text file and [file this in] The file name is provided using command line 
 * arguments. If the file name is not provided or the file is not readable, the program will exit
 * and provide an error message.
 *
 * @author Your Name Here {@Frank Schweitzer }
 * @date Jan. 30, 2022
 * @assignment Lab 3
 * @course CSC 250
 **/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STRING_SIZE 20

/* a useful struct, if you choose to use it  */ 
/* will store every word and the respective words wc */
struct WordFreq {
    char* word;
    int count;
};

/* function declarations go here */

/* processing characters a function that reads a file and adds the words */
int process_characters(char filename[], char output[]);
/* function that scans all of the structs and adds the current word buf accordingly */
int is_found(char buf[], struct WordFreq ** wfppp, int size);
/* add a new word by increasing the size of wfpp to add another pointer */
struct WordFreq ** add_word(struct WordFreq ** wfpp, char buf[], int size);
/* bubble sort */
struct WordFreq ** bubble_sort(struct WordFreq ** wfpp, int size);
/* printing results */
int print_results(char output[], struct WordFreq ** wfpp, int size);

int main(int argc, char* argv[]){
    int fileOK;
    fileOK = 1;

    /* makes sure you have the necessary 3 arguments */
    if(argc < 3){
        printf("Usage: %s filename frequency_file \n", argv[0]);
        return 1;
    }

    /* call process character to read the input file and count the words */
    fileOK = process_characters(argv[1], argv[2]);

    /* makes sure you can open the file */
    if(!fileOK){
        printf("%s could not open file %s \n", argv[0], argv[1]);
        return 2;
    }

    return 0;
}

/* This function allocates and builds a string buffer from file input.
    How or if you use this function is up to you. It is only meant to demonstrate
    this process. If you use this function, you will have to modify according
    to your needs. E.g., the argument list may need to be modified, or you may
    need a different type of return value.
    You may decide to open the fie in main and pass the file pointer as an argument.
*/
int process_characters(char filename[], char output[]){
    char ch;
    /* use this pointer to traverse through the file */
    FILE *filePtr;
    /* buffer will store the current word we are working on reading */
    char *buffer;
    /* current character location */
    int n;
    /*size has the total number of structs in the wfpp pointer*/
    int size;
    int index;
    struct WordFreq **wfpp;
    /* we use this boolean to manually insert the first word we read in each file */
    bool first;
    /* buffer used to temporarily store the current word */
    char *str_buff;
    /* use this to iterate through wfpp to free pointers */
    int i;
    
    n = 0;
    buffer = (char *)malloc(sizeof(char) * MAX_STRING_SIZE);
    first = false;
    filePtr = NULL;
    wfpp = NULL;
    str_buff = NULL;
    index = 0;
    size = 0;

    /* initializing wfpp and wfp to have one struct for the first word also initializing size as the size of wfpp */ 
    wfpp = (struct WordFreq**) malloc(sizeof(struct WordFreq *));
    wfpp[0] = (struct WordFreq*) malloc(sizeof(struct WordFreq));
    /* size now becomes 1 */
    size = 1;
    
    filePtr = fopen(filename, "rie");
    /* checks to make sure we can open the file */
    if(filePtr == 0) {
        printf("could not open %s \n", filename);
        return 0;
    }    
    
    ch = fgetc(filePtr);
    /* iterating until we hit the escape character */
    while(ch != EOF) {
        /* checks if the character is a letter and add it to the buffer to eventually get the whole word */
        if (n>=19){
            /* using string buffer */
            str_buff = (char *) malloc(sizeof(char) * (strlen(buffer)+1));
            strcpy(str_buff, buffer);
            /* store the word (C-string in buffer) in dynamic list */
            if (first == false){
                wfpp[0]->word = str_buff;
                wfpp[0]->count = 1;
                first = true;
            }
            else {
                index = is_found(str_buff, wfpp, size);
                if (index > -1){
                    wfpp[index]->count = wfpp[index]->count + 1;
                }
                else {
                    wfpp = add_word(wfpp, str_buff, size);
                    size = size + 1;
                }
            }
            /* reset escape character */
            buffer[0] = '\0';
            n = 0;
        }
        if(isalpha(ch)) {
            ch = tolower(ch);
            buffer[n] = ch;
            /* increment the position of the buffer */
            n++;
            /* updates the escape character */
            buffer[n] = '\0';
        }
        /* n tracks where you are in the buffer array & this is if a character is not a letter */
        else if (n > 0){
            str_buff = (char *) malloc(sizeof(char) * (strlen(buffer)+1));
            strcpy(str_buff, buffer);
            /* store the word (C-string in buffer) in dynamic list */
            /* if we are storing the first word, we will skip the search method and 
            instead just manually insert the word */
            if (first == false){
                wfpp[0]->word = str_buff;
                wfpp[0]->count = 1;
                first = true;
            }
            /* otherwise we check if the current word has already been processed */
            else {
                index = is_found(str_buff, wfpp, size);
                /* if the word has already been seen we update the count in its corresponding struct */
                if (index > -1){
                    wfpp[index]->count = wfpp[index]->count + 1;
                }
               /* if the word has not been found we add the word by calling add_word which returns a pointer
               to a list of points that points to struct, with a new struct for the new word we found */ 
                else {
                    wfpp = add_word(wfpp, str_buff, size);
                    /* since we added a new struct for the new word we increment the count */
                    size = size + 1;
                }
            }
            buffer[0] = '\0';
            n = 0;
        }
        /* read the next character and continue */
        ch = fgetc(filePtr);
    }

    /*picks up last word if not space at the end of file and we do the same process for processing words,
    which is to see if the word has already been processed and if so update its count accordingly and if
    not we allocate space for a new struct and store the new word there */
    if (strlen(buffer) > 0){
        str_buff = (char *) malloc(sizeof(char) * (strlen(buffer)+1));
        strcpy(str_buff, buffer);
        index = is_found(str_buff, wfpp, size);
        if (index > -1){
            wfpp[index]->count = wfpp[index]->count + 1;
        }
        else {
            wfpp = add_word(wfpp, str_buff, size);
            size = size + 1;
        }
    }

    /* calling bubble sort to sort the words by frequency */
    wfpp = bubble_sort(wfpp, size);
    
    /* printing the sorted results to an output file */
    print_results(output, wfpp, size);

    fclose(filePtr);

    for (i=0;i<size;i++){
        free((void *)wfpp[i]->word);
        free((void *)wfpp[i]);
    }
    free((void *)buffer);
    free((void *)str_buff);
    free((void *)wfpp);

    return 1;
}
int is_found(char buf[], struct WordFreq ** wfpp, int size){
    /* index will either be the index of where this word (buf) was found
    or it will return -1 if the word was not found and realloc is necessary */
    int index;
    index = 0;
    /* iterating through our wfpp pointer to analyze the words at each struct and determining if our
    current word matches any of the words previously seen */
    while (index < size){
        if (strcmp(buf, wfpp[index]->word) == 0){
            return index;
        }
        index++;
    }
    return -1;
}
/* we run this method when the current word has not been found in our list of structs
so we allocate space for a new struct and return a pointer that points to this new
list of structs */
struct WordFreq ** add_word(struct WordFreq **wfpp, char buf[], int size){
    /* reallocating to add space for an additional pointer in wfpp */
    wfpp = (struct WordFreq**) realloc((void*) wfpp, (sizeof(struct WordFreq *) * (size + 1)));
    /* allocating enough space for a struct so we can add our new word to this struct */
    wfpp[size] = (struct WordFreq*) malloc(sizeof(struct WordFreq));
    /* setting the word to the new word and setting the count to 1 */
    wfpp[size]->word = buf;
    wfpp[size]->count = 1;
    return wfpp;
}
/* performs a bubble sort on the structs in wfpp and sorts them based on count*/
struct WordFreq ** bubble_sort(struct WordFreq ** wfpp, int size){
    int i;
    int j;

    /*creating a temp struct*/
    struct WordFreq *wfp;
    
    /*wfp = (struct WordFreq *) malloc(sizeof(struct WordFreq));*/
    /* implementing a bubble sort */
    for (i=0;i<size-1;i++){
        for (j=0;j<(size-i-1);j++){
            /* performing a swap if there is a lower frequency before a higher frequency */
            if (wfpp[j]->count < wfpp[j+1]->count){
                wfp = wfpp[j];
                wfpp[j] = wfpp[j+1];
                wfpp[j+1] = wfp;
            }
        }
    }
    return wfpp;
}
int print_results(char output[], struct WordFreq ** wfpp, int size){
    /* declaring a new file pointer for our output file */
    FILE *outputFilePtr;
    int i;

    outputFilePtr = fopen(output, "w");
    /* checks to make sure we can open the file */
    if(outputFilePtr == 0) {
        printf("could not open %s \n", output);
        return 0;
    }
    /* looping through the sorted words and printing them onto the output file */
    for (i=0;i<size;i++){
        /* printf("word: %s count: %d \n", wfpp[i]->word, wfpp[i]->count); */
        fprintf(outputFilePtr," %s %d\n", wfpp[i]->word, wfpp[i]->count);
    }
    fclose(outputFilePtr);
    return 1;
}