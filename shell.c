/*
TODO:

check for fake dirs in movetodir

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

//constant variables
#define BUFFERSIZE 2000
#define MAXTOKENS 100
#define MAXHISTORYSIZE 10000

//Shell Flags
#define DEBUGMODE 1

typedef enum commandType {
    typemovetodir = 0,
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

typedef struct Processes{
    int processPIDS[BUFFERSIZE];
    pid_t size;
} Proceses;

//function prototypes
Directory *initDir();
void initHistory();
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
void dalekall();
void addtohistory(Command *pcommand);

// Global access variables
Directory *dirinfo; //init current directory
History *commandHistory;
Proceses activeProcesses = malloc(sizeof(activeProcesses));
    


Directory *initDir(){
    //Init directory info with the current working directory
    Directory* retval = malloc(sizeof(Directory));
    retval->currentDirectory = (char*) malloc(BUFFERSIZE * sizeof(char));

    getcwd(retval->currentDirectory, BUFFERSIZE);

    return retval;
}

void initHistory(int readFromFile){
    commandHistory = malloc(sizeof(commandHistory));
    commandHistory->size = 0;
    commandHistory->commands = malloc(sizeof(Command) * MAXHISTORYSIZE);

    if(!readFromFile){
        return;
    }

    FILE *ptr;
    ptr = fopen("history.txt", "r");

    if(ptr){
        while(1){
            Command *pcommand = malloc(sizeof(Command));
            
            size_t len = BUFFERSIZE;

            char* line = malloc(BUFFERSIZE * sizeof(char));
            getline(&line, &len, ptr);

            if(feof(ptr)){
                break;
            }

            line[strcspn(line, "\n")] = 0;

            pcommand = parseCommand(line);

            addtohistory(pcommand);
        }
    }
    fclose(ptr);

    return;
}

char *getCommands(){

    // Convert buffersize to size_t
    size_t len = BUFFERSIZE;

    //grab commands
    char* line = malloc(BUFFERSIZE * sizeof(char));
    getline(&line, &len, stdin);
    line[strcspn(line, "\n")] = 0;
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
    enum commandType type = setType(pcommand->command);

    switch (type) {
        case typemovetodir:
            movetodir(pcommand);
            break;
        case typewhereami:
            whereami();
            break;
        case typehistory:
            history(pcommand);
            break;
        case typebyebye:
            byebye();
            break;
        case typereplay:
            replay(pcommand);
            break;
        case typestart:
            start(pcommand);
            break;
        case typebackground:
            background(pcommand);
            break;
        case typedalek:
            dalek(pcommand);
            break;
        case typerepeat:
            repeat(pcommand);
            break;
        case typedalekall:
            dalekall();
            break;
        default:
            printf("This command is not a legal command!\n");
            break;
        return;
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

    dirinfo->currentDirectory = pcommand->parameters[0];

    if(DEBUGMODE){
        printf("Changed to: %s\n", dirinfo->currentDirectory);

    }
    return;
}
void whereami(){
    printf("%s\n", dirinfo->currentDirectory);
    return;
}
void history(Command *pcommand){
    int params = pcommand->numOfParameters;

    if(params != 0 && strcmp(pcommand->parameters[0], "-c") == 0){
        free(commandHistory->commands);
        free(commandHistory);
        initHistory(0);
    }
    else{
        int size = commandHistory->size;
        for(int i = size - 1; i >= 0; i--){
            printf("[%d]: %s\n", size - i - 1, commandHistory->commands[i].command);
        }
        return;
    }
}
void byebye(){

    FILE *ptr;
    ptr = fopen("history.txt", "w");
    
    int size = commandHistory->size;
    for(int i = 0; i < size - 1; i++){
        fprintf(ptr, "%s\n", commandHistory->commands[i].command);
    }
    fclose(ptr);
    exit(0);
    return;
}
void replay(Command *pcommand){
    int commandNumber = (int) strtol(pcommand->parameters[0], (char **)NULL, 1000);
    executeCommand(&commandHistory->commands[commandNumber]);
    return;
}
void start(Command *pcommand){
    char program[BUFFERSIZE];
    strcpy(program, pcommand->parameters[0]);

    //pids
    pid_t parent = getpid();
    pid_t pid = fork();
    activeProcesses.processPIDS[activeProcesses.size++] = pid;
    //construct command string
    char *cmdString[] = {program, NULL};
    if(pid == -1) {
        printf("Error starting process.\n");
    } else if(pid > 0){
        int status;
        waitpid(pid, &status, 0);
    } else {
        if(DEBUGMODE){
            printf("Starting... %s\n", program);
        }
        execv(program, cmdString);
    }
    
    return;
}
void background(Command *pcommand){
    char program[BUFFERSIZE];
    strcpy(program, pcommand->parameters[0]);

    //pids
    pid_t parent = getpid();
    pid_t pid = fork();
    activeProcesses.processPIDS[activeProcesses.size++] = pid;
    //construct command string
    char *cmdString[] = {program, NULL};
    if(pid == -1) {
        printf("Error starting process.\n");
    } else if(pid > 0){
        printf("I am the parent of this child!\n");

    } else {
        if(DEBUGMODE){
            printf("Starting... %s\n\n", program);
        }
        setpgid(0,0);
        execv(program, cmdString);
    }
    
    return;
}
void dalek(Command *pcommand){
    return;
}
void repeat(Command *pcommand){
    return;
}
void dalekall(){
    for(int i =0; i < activeProcesses.size; i++){
        
    }
    return;
}
void addtohistory(Command *pcommand){
    commandHistory->commands[commandHistory->size] = *pcommand;
    commandHistory->size++;
    return;

}




int main(int argc, char **argv){

    // Initilize shell program
    dirinfo = initDir();
    initHistory(1);
    //Variables
    char *command;
    
    //Shell loop
    while(1){
        printf("# ");
        fflush(stdout);
        
        Command *pcommand = malloc(sizeof(command));
        command = getCommands();
        pcommand = parseCommand(command);
        addtohistory(pcommand);
        executeCommand(pcommand);

        //DEBUG: Print the contents of pcommand
        if(DEBUGMODE) {
            printf("Command: %s\n", pcommand->command);
            for(int i = 0; i < pcommand->numOfParameters; i++){
                printf("Parameter %d: %s\n", i, pcommand->parameters[i]);
            }
        }

    }
}


