#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//constant variables
#define BUFFERSIZE 2000
#define MAXTOKENS 100

//Shell Flags
#define DEBUGMODE 1

typedef enum commandType {
    typemovetodir,
    typewhereami,
    typehistory,
    typebyebye,
    typereplay,
    typestart,
    typebackground,
    typedalek,
    typerepeat,
    typedalekall

} commandType;



// Holds directory information
typedef struct Directory{
    char* currentDirectory;
} Directory; 

typedef struct Command{
    char* command;
    int numOfParameters;
    char** parameters;
} Command; 

typedef struct History{
    Command *commands;
    int size;
} History; 


//function prototypes
Directory *initDir();
char *getCommands(); // Grab commands from standard output and parse them
Command *parseCommand(); // Grab line, tokenize
void executeCommand(Command *pcommand);
commandType setType(char* command);

// Global access variables
Directory *dirinfo; //init current directory

    


Directory *initDir(){
    //Init directory info with the current working directory
    Directory* retval = malloc(sizeof(Directory));
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


    //Grab first token | Command
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



commandType setType(char* command){

    if(strcmp(command, "movetodir") == 0){
        return typemovetodir;
    } 
    else if(strcmp(command, "whereami") == 0){
        return typewhereami;
    }
    else if(strcmp(command, "history") == 0){
        return typehistory;
    }
    else if(strcmp(command, "byebye") == 0){
        return typebyebye;
    }
    else if(strcmp(command, "replay") == 0){
        return typereplay;
    }
    else if(strcmp(command, "start") == 0){
        return typestart;
    }
    else if(strcmp(command, "background") == 0){
        return typebackground;
    }
    else if(strcmp(command, "dalek") == 0){
        return typedalek;
    }
    else if(strcmp(command, "repeat") == 0){
        return typerepeat;
    }
    else if(strcmp(command, "dalekall") == 0){
        return typedalekall;
    } else {
        return -1;
    }


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


