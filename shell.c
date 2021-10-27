#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//constant variables
#define BUFFERSIZE 2000
#define MAXTOKENS 100

//Shell Flags
#define DEBUGMODE 1

// Holds directory information
typedef struct directory{
    char* currentDirectory;
} directory; 

typedef struct Command{
    char* command;
    int numOfParameters;
    char** parameters;
} Command; 

//function prototypes
directory *initDir();
char *getCommands(); // Grab commands from standard output and parse them
Command *parseCommand(); // Grab line, tokenize

// Global access variables
directory *dirinfo; //init current directory

    


directory *initDir(){
    //Init directory info with the current working directory
    directory* retval = malloc(sizeof(directory));
    retval->currentDirectory = (char*) malloc(BUFFERSIZE * sizeof(char));
    getcwd(retval->currentDirectory, BUFFERSIZE);
    return retval;
}

char *getCommands(){

    // Convert buffersize to size_t
    size_t len = BUFFERSIZE;

    //grab commands
    char* line = malloc(BUFFERSIZE * sizeof(char));
    getline(&line, &len, stdin);

    //TODO: parse commands with multiple parameters

    
    return line;

}

// Tokenize command
Command *parseCommand(char* command){

    // Init struct to hold command
    Command *pcommand = malloc(sizeof(Command));
    pcommand->command = malloc(sizeof(char) * BUFFERSIZE);
    pcommand->parameters = malloc(sizeof(char*) * MAXTOKENS);


    //Grab first token
    char *token = strtok(command, " ");
    strcpy(pcommand->command, token);

    //iterator
    int i = 0;


    // Read the number of tokens
    while((token = strtok(NULL, " ")) != NULL){

        pcommand->parameters[i++] = token;
        
    }

    if(DEBUGMODE){
        printf("%d parameters read from command %s\n", i,pcommand->command);
    }

    //Set the number of parameters
    pcommand->numOfParameters = i;

    return pcommand;

}
int main(int argc, char **argv){

    // Initilize shell program
    dirinfo = initDir();

    //Variables
    char *command;
    
    //Shell loop
    while(1){
        Command *pcommand = malloc(sizeof(command));
        command = getCommands();
        pcommand = parseCommand(command);

        //DEBUG: Print the contents of pcommand
        if(DEBUGMODE) {
            printf("Command: %s\n", pcommand->command);
            for(int i = 0; i < pcommand->numOfParameters; i++){
                printf("Parameter %d: %s\n", i, pcommand->parameters[i]);
            }
        }

    }
}