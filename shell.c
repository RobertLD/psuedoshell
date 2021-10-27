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
void movetodir(Command *pcommand);
void whereami();
void history(Command *pcommand);
void byebye();
void replay(Command *pcommand);
void start(Command *pcommand);
void background(Command *pcommand);
void dalek(Command *pcommand);
void repeat(Command *pcommand);
void dalekall(Command *pcommand);

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

// Executes the command selected by the user
void executeCommand(Command *pcommand){
    commandType type = setType(pcommand->command);

    switch (type) {
        case typemovetodir:
        case typewhereami:
        case typehistory:
        case typebyebye:
        case typereplay:
        case typestart:
        case typebackground:
        case typedalek:
        case typerepeat:
        case typedalekall:
<<<<<<< HEAD

=======
        default:
        return;
>>>>>>> b37904cf03ae2f0fe4952d2e07bb949413f70949
    }

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

void movetodir(Command *pcommand){

    Directory->currentDirectory = pcommand->parameters[0];

    if(DEBUGMODE){
        printf("Changed to: %s\n", Directory->currentDirectory);

    }
    return;
}
void whereami(){
    printf("%s\n", Directory->currentDirectory);
    return;
}
void history(){
    return;
}
void byebye(){
    return;
}
void replay(){
    return;
}
void start(){
    return;
}
void background(){
    return;
}
void dalek(){
    return;
}
void repeat(){
    return;
}
void dalekall(){
    return;
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


