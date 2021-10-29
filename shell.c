/*
Dr. Hensel COP4600 Assignment 2
Co-Authored by Robert DeRienzo and David Hranicky
*/

/*
TODO:

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

void banana(); // Banana, yo

// =======================
// Global access variables
// =======================

Directory *dirinfo; //init current directory
History *commandHistory;
Processes *activeProcesses;
    

// =========
// Functions
// =========

// Written by Robert

// Fetches the directory the shell is run from and returns it
Directory *initDir(){
    //Init directory info with the current working directory
    Directory* retval = malloc(sizeof(Directory));
    retval->currentDirectory = (char*) malloc(BUFFERSIZE * sizeof(char));

    getcwd(retval->currentDirectory, BUFFERSIZE);

    return retval;
}

// First draft by Robert, only created a blank history
// File input functionality written by David

// Allocates space for the command history. If readFromFile == 1 then will read
// from the associated history.txt file if it exists. Used to clear the history
// re-allocating empty space.
void initHistory(int readFromFile){
    // Alloc space for history
    commandHistory = malloc(sizeof(History) * 1);
    commandHistory->size = 0;
    commandHistory->commands = malloc(sizeof(Command) * MAXHISTORYSIZE);

    // Checks if the command should read from the stored file. If not, history 
    // is now blank, effectively erasing it
    if(!readFromFile){
        return;
    }

    FILE *ptr;
    ptr = fopen("history.txt", "r");

    // Read from file if it exists
    if(ptr != NULL){
        // while true loop to read the entire file
        while(1){
            Command *pcommand = malloc(sizeof(Command));
            
            size_t len = BUFFERSIZE;

            // Read in each line of the file
            char* line = malloc(BUFFERSIZE * sizeof(char));
            getline(&line, &len, ptr);

            // Stop reading at end of file
            if(feof(ptr)){
                break;
            }

            line[strcspn(line, "\n")] = 0;

            // Parse the command
            pcommand = parseCommand(line);

            // Add command to history
            addtohistory(pcommand);
        }
        fclose(ptr);
    }
    

    return;
}

// Written by Robert

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

// Written by Robert

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

// Written by both Robert and David, split evenly

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
    }else if(strcmp(command, "banana") == 0){
        banana();
    }
     else {
        printf("This command is not a legal command!\n");
    }
}

// First draft by Robert, just updated current directory variable
// Handling for non-existing directories written by David
// Handling for Relative paths written by Robert

// Changes the current directory to the given directory if it exists, else it 
// raises an error
void movetodir(Command *pcommand){

    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;
    char* newDirectory = pcommand->parameters[0];
    
    if(strcmp(newDirectory, "..") == 0){
        // If we enter the directory as '..' we will go back one directory
        // Effectivly we find the pointer to the last slash in the directory
        // string and set it to the termination character.
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

// Written by RObert

// Prints the current directory
void whereami(){
    printf("%s\n", dirinfo->currentDirectory);
    return;
}

// First draft written by Robert, only printed command history
// -c flag and printing parameters written by David

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
        // Loop through commandHistory backwards (stored FCFS, oldest is first)
        // and print the command along with the parameters
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

// First draft written by Robert, just called exit
// File output written by David
// Memory management written by Robert

// Closes the shell and saves the current history to an external file. Byebye is
// not saved to the history
void byebye(){

    FILE *ptr;
    ptr = fopen("history.txt", "w");
    
    // Write command history as it is stored in the program to an external file.
    // This simplifies reading from the file as it is a direct mirror of the
    // internal state of the history
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

// Written by Robert

// Replays the given command referenced in the order that they are printed (0 
// is the most recent call)
void replay(Command *pcommand){

    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;

    // Retrieve the number passed, change indexing to match how history is
    // printed
    char* strNumber = pcommand->parameters[0];
    int commandNumber = (int) strtol(strNumber, (char **)NULL, 10);
    // size - 2 because this gets executed after replay is added to the
    // history, so the size is one larger than the state of the history when 
    // replay was called.
    commandNumber = commandHistory->size - 2 - commandNumber;
    
    if(commandNumber > commandHistory->size){
        printf("Cannot replay a future command.\n");
    } else if(commandNumber < 0) printf("Invalid replay index.\n");

    if(DEBUGMODE){
        printf("I am in replay, my command to rerun is: %d\n", commandNumber);
        printf("The command is %s\n", commandHistory->commands[commandNumber].command);
    }

    // Passing the pcommand
    executeCommand(&commandHistory->commands[commandNumber]);
    return;
}

// First draft written by Robert, did not handle parameters
// Parameter handling written by David

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
    char *cmdString[MAXTOKENS];

    // Initialize space for the command string
    for(int i = 0; i < MAXTOKENS; i++){
        cmdString[i] = malloc(BUFFERSIZE * sizeof(char));
    }

    // Copy the parameters into the command string
    int numParams = pcommand->numOfParameters;
    for(int i = 0; i < numParams; i++){
        strcpy(cmdString[i], pcommand->parameters[i]);
    }

    // Null terminate so execv doesn't scream
    cmdString[numParams] = NULL;

    // -1 if something went wrong
    if(pid == -1) {
        printf("Error starting process.\n");
    } else if(pid > 0){ // what the parent does
        int status;
        waitpid(pid, &status, 0);
    } else { // what the child does
        if(DEBUGMODE){
            printf("Starting... %s\n", program);
        }
        execv(program, cmdString);
    }
    
    // Free command string
    for(int i = 0; i < MAXTOKENS; i++){
        free(cmdString[i]);
    }

    return;
}

// Same deal as start, initially written by Robert, parameter handling written
// by David

// Runs a given program with the parameters passed, but in the background. 
// Prints the PID of the processes that was created
void background(Command *pcommand){
    char program[BUFFERSIZE];

    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;

    strcpy(program, pcommand->parameters[0]);

    char *cmdString[MAXTOKENS];

    // Initialize space for the command string
    for(int i = 0; i < MAXTOKENS; i++){
        cmdString[i] = malloc(BUFFERSIZE * sizeof(char));
    }

    // Copy the parameters into the command string
    int numParams = pcommand->numOfParameters;
    for(int i = 0; i < numParams; i++){
        strcpy(cmdString[i], pcommand->parameters[i]);
    }

    // Null terminate so execv doesn't scream
    cmdString[numParams] = NULL;

    // pids
    pid_t parent = getpid();
    pid_t child = fork();

    if(child == 0){
        execv(program, cmdString);
    }

    //In parent add PID to the struct
    activeProcesses->processPIDS[activeProcesses->size++] = child;
    printf("&[%d]\n", child);

    // Free command string
    for(int i = 0; i < MAXTOKENS; i++){
        free(cmdString[i]);
    }

    return;
}

// Written by Robert

// Terminates the process with the given PID
void dalek(Command *pcommand){
    // Do nothing when told to do nothing
    if(pcommand->numOfParameters == 0) return;

    int pid = strtol(pcommand->parameters[0], (char **)NULL, 10);
    printf("Killing [%d]\n", pid);
    kill(pid, SIGKILL);
    return;
}

// Written by David

// Runs the given program with the given parameters n times and prints the PIDs 
// of the processes
void repeat(Command *pcommand){
    // Check for nnumber of repeats
    if(pcommand->parameters[0] == NULL){
        printf("Specify number of times to repeat\n");
    }

    // Get number of repeats
    char* strNumber = pcommand->parameters[0];
    int repeatnum = (int) strtol(strNumber, (char **)NULL, 10);
    
    // Check for command to repeat
    if(pcommand->parameters[1] == NULL){
        printf("Specify which command to repeat\n");
    }
    
    int numparams = pcommand->numOfParameters;

    // Create a new command to be passed into background. The heavy lifting is
    // done there, so just call that n times.
    Command *newcommand = malloc(sizeof(Command));
    newcommand->command = malloc(BUFFERSIZE * sizeof(char));
    strcpy(newcommand->command, pcommand->command);
    newcommand->numOfParameters = numparams - 1; // doesn't have n as a param
    newcommand->parameters = malloc(sizeof(char*) * MAXTOKENS);

    // Assign the ith newcommand param to the i+1th pcommand param as the 0th 
    // pcommand param is always n
    for(int i = 0; i < numparams - 1; i++){
        newcommand->parameters[i] = pcommand->parameters[i+1];
    }

    // Call background n times
    for(int i = 0; i < repeatnum; i++){
        background(newcommand);
    }
    
    return;
}

// Written by Robert

// Kills all processes that were created by the shell and prints the PIDs of 
// the killed processes
void dalekall(){
    for(int i = 0; i < activeProcesses->size; i++){
        printf("Killing PID: %d\n", activeProcesses->processPIDS[i]);
        kill(activeProcesses->processPIDS[i], SIGKILL);
    }
    return;
}

// Written by Robert

// Helper function that adds a given command to a list of commands
void addtohistory(Command *pcommand){
    commandHistory->commands[commandHistory->size] = *pcommand;
    commandHistory->size++;
    return;

}

// Banana, yo

void banana(){

    puts(" _\n//\\\nV  \\\n \\  \\_\n  \\,\'.`-.\n   |\\ `. `.       \n   ( \\  `. `-.                        _,.-:\\\n    \\ \\   `.  `-._             __..--\' ,-\';/\n     \\ `.   `-.   `-..___..---\'   _.--\' ,\'/\n      `. `.    `-._        __..--\'    ,\' /\n        `. `-_     ``--..\'\'       _.-\' ,\'\n          `-_ `-.___        __,--\'   ,\'\n             `-.__  `----\"\"\"    __.-\'\n                  `--..____..--\'");
}


// Written by Robert

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
        printf("\n# ");
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


