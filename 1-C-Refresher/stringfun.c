#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int count_words(char *, int, int);
//add additional prototypes here
int reverse_string(char *, int, int);
int word_print(char *, int, int);
 

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int j;
    int inWord;
    int strLength;
    char *tempStr = user_str;
    j = 0;
    inWord = 0;
    strLength = 0;

    while(*tempStr != '\0') {
	    strLength++;
	    tempStr++;
    }

    if(strLength > len) {
	    return -3;
    }

	while(*user_str != '\0' &&  j < len) {
		if(*user_str != '\t' || *user_str != ' ') {
			*(buff + j) = *user_str;
			j++;
			inWord = 1;
		} else {
			if(inWord) {
			*(buff + j) = ' ';
			j++;
			inWord = 0;
			}
		}
	user_str++;
	}	

	while(j < len) {
		*(buff + j) = '.';
		j++;
	}
	
	return strLength;

}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len) {
        int numberOfWords;
        int inWord;
        int i;
        i = 0;
        inWord = 0;
        numberOfWords = 0;

        if (str_len > len) {
                printf("Error Input string is too long");
                return -3;
        }

        for(i = 0; i < str_len && i < len; i++) {
                if(*(buff + i) == '\t' || *(buff + i) == ' ') {
                        if(inWord) {
                                numberOfWords++;
                                inWord = 0;
                        }
                } else {
                        inWord = 1;
                }
        }
        if(inWord) {
                numberOfWords++;
        }
        return numberOfWords;
}

int reverse_string(char *buff, int len, int str_len) {
    int i = 0, j = str_len - 1;
    char temp;

    if (str_len == 0) {
        return -3; // Error if the string is empty
    }

    while (i < j) {
        temp = *(buff + i);
        *(buff + i) = *(buff + j);
        *(buff + j) = temp;
        i++;
        j--;
    }

    printf("Reversed String: ");
    for (i = 0; i < str_len; i++) {
        printf("%c", *(buff + i));
    }
    printf("\n");

    return 0;
}

int word_print(char *buff, int len, int str_len) {
	int i;
	int inWord;
	int wordNum;
	int numChar;
	numChar = 0;
	wordNum = 1;
	inWord = 0;
	i = 0;
	
	if (str_len > len) {
		printf("Error Input string is too long");
		return -3;
	}

	printf("Word Print\n---------\n1. ");

	for(i = 0; i < str_len && i < len; i++) {
		if(*(buff + i) == '\t' || *(buff + i) == ' ') {
			if(inWord) {
				printf(" (%d)", numChar);
				numChar = 0;
				wordNum++;
				printf("\n%d. ", wordNum);
				inWord = 0;
			}
		} else {
			printf("%c", *(buff + i));
			inWord = 1;
			numChar++;
		}
	}
	if(inWord){
		printf(" (%d)", numChar);
		printf("\n");
	}

	return 0;
}


//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
     * This block is safe because it's checking the conditions of how to use the command. If the user's
     * command doesn't have atleast 2 arguments, or the second argument doesn't start with a hyphen, then
     * the command will output how to use the command (from usage method). 
     *
     * In the case that argv[1] does not exist, then commands such as "stringfun wrongInput" is a command
     * that would not be an incorrect input into the program. Although, it is very much a wrong input. 
     */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
     * The previous part of the program checks the -h command, which is the only command that allows
     * only 2 arguments. Afterwards, the other commands would need to have atleast 3 arguments to
     * work. Thus, the if statement below is checking whether 3 arguments were provided into the
     * command. For example, "stringfun -c "Check the cound of this sentence""
     * 
     */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = (char *)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL)
    {
	    printf("Memory allocation failed\n");
	    exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

	case 'r':
	    rc = reverse_string(buff, BUFFER_SZ, user_str_len);
	    if (rc < 0){
            	printf("Error reversing string, rc = %d", rc);
            	exit(2);
            }
	    break;

        case 'w':
            rc = word_print(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error printing string, rc = %d", rc);
                exit(2);
            }
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    //print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
//
//          I think it's good to providing the pointer and length in the helper function is
//          good practice because it can help add flexibility to the program and make it
//          safer. For example, if we decide to change the amount of memory we want to 
//          allocate in the future, changing the buffer size is all we would need
//          to do. Moreover, I think that it's safer to keep the buffer_size and lengths
//          in the functions to reinforce the fixed size of memory that we set
//          outside of the function.
