/*
TODO:

make repeat
pass parameters in for the start command
handle relative paths in start

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

//constant variables
#define BUFFERSIZE 2000
#define MAXTOKENS 100
#define MAXHISTORYSIZE 10000

//Shell Flags
#define DEBUGMODE 0

// ======================
// Data struct definition
// ======================

// Holds directory information
typedef struct Directory{
    char* currentDirectory;
} Directory; 

// Holds the command name, number of parameters, and an array of the parameters
typedef struct Command{
    char* command;
    int numOfParameters;
    char** parameters;
} Command; 

// Holds an array containing the history
typedef struct History{
    Command *commands;
    int size;
} History; 

// Holds the active processes
typedef struct Processes{
    int processPIDS[BUFFERSIZE];
    pid_t size;
} Processes;


// ===================
// Function prototypes
// ===================

Directory *initDir();
 
void initHistory(int readFromFile);

char *getCommands(); // Grab commands from standard output and parse them

Command *parseCommand(); // Grab line, tokenize

void executeCommand(Command *pcommand);

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

// =======================
// Global access variables
// =======================

Directory *dirinfo; //init current directory
History *commandHistory;
Processes *activeProcesses;
    

// =========
// Functions
// =========

// Fetches the directory the shell is run from and returns it
Directory *initDir(){
    //Init directory info with the current working directory
    Directory* retval = malloc(sizeof(Directory));
    retval->currentDirectory = (char*) malloc(BUFFERSIZE * sizeof(char));

    getcwd(retval->currentDirectory, BUFFERSIZE);

    return retval;
}

// Allocates space for the command history. If readFromFile == 1 then will read
// from the associated history.txt file if it exists. Used to clear the history
// re-allocating empty space.
void initHistory(int readFromFile){
    commandHistory = malloc(sizeof(History) * 1);
    commandHistory->size = 0;
    commandHistory->commands = malloc(sizeof(Command) * MAXHISTORYSIZE);

    if(!readFromFile){
        return;
    }

    FILE *ptr;
    ptr = fopen("history.txt", "r");

    if(ptr != NULL){
        // while true loop to read the entire file
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
        fclose(ptr);
    }
    

    return;
}

// Gets the command and parameters from stdin and returns them as a string
char *getCommands(){

    // Convert buffersize to size_t
    size_t len = BUFFERSIZE;

    //grab commands
    char* line = malloc(BUFFERSIZE * sizeof(char));
    getline(&line, &len, stdin);
    line[strcspn(line, "\n")] = 0;

    return line;

}

// Parses input command into the command name and an array of the parameters.
// Returns a Command
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
    char* command = pcommand->command;

    if(strcmp(command, "movetodir") == 0){
        movetodir(pcommand);
    } 
    else if(strcmp(command, "whereami") == 0){
        whereami();
    }
    else if(strcmp(command, "history") == 0){
        history(pcommand);
    }
    else if(strcmp(command, "byebye") == 0){
        byebye();
    }
    else if(strcmp(command, "replay") == 0){
        replay(pcommand);
    }
    else if(strcmp(command, "start") == 0){
        start(pcommand);
    }
    else if(strcmp(command, "background") == 0){
        background(pcommand);
    }
    else if(strcmp(command, "dalek") == 0){
        dalek(pcommand);
    }
    else if(strcmp(command, "repeat") == 0){
        repeat(pcommand);
    }
    else if(strcmp(command, "dalekall") == 0){
        dalekall();
    } else {
        printf("This command is not a legal command!\n");
    }
}

// Changes the current directory to the given directory if it exists, else it 
// raises an error
void movetodir(Command *pcommand){

    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;
    char* newDirectory = pcommand->parameters[0];


  
    
    
    if(strcmp(newDirectory, "..") == 0){
        // If we enter the directory as '..' we will go back one directory
        // Effectivly we find the pointer to the last slash in the directory string
        // and set it to the termination character.
        char *last_slash = strrchr(dirinfo->currentDirectory, '/');

        // Couldn't go back
        if(last_slash == NULL) {
            printf("Could not desend directory tree.\n");
        }

        *last_slash ='\0';
        return;
    }
    DIR* dir = opendir(newDirectory);

    //Are we moving to an absolute path?
    if(pcommand->parameters[0][0] == '/'){
        if(dir != NULL){
            strcpy(dirinfo->currentDirectory, newDirectory);    
            if(DEBUGMODE){
                printf("Changed to: %s\n", dirinfo->currentDirectory);
            }
        } else if (ENOENT == errno){
        printf("This directory does not exist\n");
    }
    return;       
    }
    

    if(dir != NULL){
        closedir(dir);
        //dirinfo->currentDirectory = newDirectory;
        strcat(dirinfo->currentDirectory, "/");
        strcat(dirinfo->currentDirectory, newDirectory);
        if(DEBUGMODE){
            printf("Changed to: %s\n", dirinfo->currentDirectory);
        }
    }
    else if (ENOENT == errno){
        printf("This directory does not exist\n");
    }
    return;
}

// Prints the current directory
void whereami(){
    printf("%s\n", dirinfo->currentDirectory);
    return;
}

// Prints the history in reverse order (newest first). If "-c" is passed as a 
// parameter, then the history is cleared
void history(Command *pcommand){
    int params = pcommand->numOfParameters;

    if(params != 0 && strcmp(pcommand->parameters[0], "-c") == 0){
        // Free the old history data and replace with a blank copy
        free(commandHistory->commands);
        free(commandHistory);
        initHistory(0);
    }
    else{
        int size = commandHistory->size;
        for(int i = size - 1; i >= 0; i--){
            Command pcommand = commandHistory->commands[i];
            
            printf("[%d]: %s ", size - i - 1, pcommand.command);
            for(int i = 0; i < pcommand.numOfParameters; i++){
                printf("%s ", pcommand.parameters[i]);
            }
            printf("\n");
        }
        return;
    }
}

// Closes the shell and saves the current history to an external file. Byebye is
// not saved to the history
void byebye(){

    FILE *ptr;
    ptr = fopen("history.txt", "w");
    
    int size = commandHistory->size;
    for(int i = 0; i < size - 1; i++){
        Command pcommand = commandHistory->commands[i];
        
        fprintf(ptr, "%s ", pcommand.command);
        for(int i = 0; i < pcommand.numOfParameters; i++){
            fprintf(ptr, "%s ", pcommand.parameters[i]);
        }
        fprintf(ptr, "\n");
    }
    fclose(ptr);

    //Free global variables
    //Free dirinfo
    free(dirinfo);

    //Free commandHistory
    
    
    
    free(commandHistory);
    
    // Free active processes
    free(activeProcesses);

    exit(0);
    return;
}

// Replays the given command referenced in the order that they are printed (0 
// is the most recent call)
void replay(Command *pcommand){

    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;

    char* strNumber = pcommand->parameters[0];
    int commandNumber = (int) strtol(strNumber, (char **)NULL, 10);
    
    if(commandNumber > commandHistory->size){
        printf("Cannot replay a future command.\n");
    } else if(commandNumber < 0) printf("Invalid replay index.\n");

    printf("I am in replay and my command to rerun is: %d\n", commandNumber);
    printf("The command is %s\n", commandHistory->commands[commandNumber].command);

    // Passing the pcommand
    executeCommand(&commandHistory->commands[commandNumber]);
    return;
}

// Runs the given program with the parameters passed to it
void start(Command *pcommand){
    char program[BUFFERSIZE];

    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;

    strcpy(program, pcommand->parameters[0]);

    //pids
    pid_t parent = getpid();
    pid_t pid = fork();
    activeProcesses->processPIDS[activeProcesses->size++] = pid;
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

// Runs a given program with the parameters passed, but in the background. 
// Prints the PID of the processes that was created
void background(Command *pcommand){
    char program[BUFFERSIZE];

    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;

    strcpy(program, pcommand->parameters[0]);
    char *cmdString[] = {program, NULL};
    pid_t parent = getpid();
    pid_t child = fork();

    if(child == 0){
        execv(program, cmdString);
    }

    //In parent add PID to the struct
    activeProcesses->processPIDS[activeProcesses->size++] = child;
    printf("&[%d]\n", child);

    return;
}

// Terminates the process with the given PID
void dalek(Command *pcommand){
    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;

    int pid = strtol(pcommand->parameters[0], (char **)NULL, 10);
    printf("Killing [%d]\n", pid);
    kill(pid, SIGKILL);
    return;
}

// Runs the given program with the given parameters n times and prints the PIDs 
// of the processes
void repeat(Command *pcommand){
    return;
}

// Kills all processes that were created by the shell and prints the PIDs of 
// the killed processes
void dalekall(){
    for(int i = 0; i < activeProcesses->size; i++){
        printf("Killing PID: %d", activeProcesses->processPIDS[i]);
        kill(activeProcesses->processPIDS[i], SIGKILL);
    }
    return;
}

// Helper function that adds a given command to a list of commands
void addtohistory(Command *pcommand){
    commandHistory->commands[commandHistory->size] = *pcommand;
    commandHistory->size++;
    return;

}




int main(int argc, char **argv){

    // Initilize shell program
    dirinfo = initDir();
    initHistory(1);
    activeProcesses = malloc(sizeof(Processes));
    //Variables
    char *command;
    
    //Shell loop
    while(1){
        fflush(stdout);
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


