#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#define MAX_HISTORY 10

// Circular buffer for storing command history
char history[MAX_HISTORY][1024];
int history_start = 0, history_end = 0;

// Function to add a command to history
void add_history(const char *command) {
    strncpy(history[history_end], command, 1023);
    history[history_end][1023] = '\0';
    history_end = (history_end + 1) % MAX_HISTORY;
    if (history_end == history_start) {
        history_start = (history_start + 1) % MAX_HISTORY;
    }
}

// Function to display command history
void display_history() {
    int i = history_start;
    while (i != history_end) {
        printf("%s\n", history[i]);
        i = (i + 1) % MAX_HISTORY;
    }
}

// ... (same functions as before, i.e., is_exit, numOfArg, get_command_and_arg) ...
int is_exit(char *str)
{
    if (strcmp(str, "exit") == 0)
        return 1;
    else
        return 0;
}
int numOfArg(char *str)
{
    int count = 0;
    int flag = 0;
    for (int i = 0; i <= strlen(str); i++)
    {
        if (str[i] == ' ')
        {
            flag = 1;
        }
        if (flag == 1 && str[i] != ' ' && str[i] != '\n')
        {
            count++;
            flag = 0;
        }
    }
    //printf("count is %d\n",count);
    return count;
    
}
void get_command_and_arg(char *str, char **command_arg)
{
    int numArg = numOfArg(str);

    int j = 0, next_arg = 0, arg_start = 0;

    char temp[256] = {0};
    for (int i = 0; i <=strlen(str); i++)
    {
        //printf("str[%d] is '%c' \n",i,str[i]);
        if ((str[i] == ' ' || str[i] == '\0') && arg_start)
        {
            //printf("temp is %s and the length is %ld\n",temp, strlen(temp));
            strcpy(command_arg[next_arg], temp);
            j = 0;
            next_arg++;
            arg_start = 0;
            memset(temp, 0, strlen(temp));
        }
        else if (str[i] != ' ' && str[i] != '\0')
        {
            arg_start = 1;
            temp[j] = str[i];
            //printf("temp[%d] is '%c' \n",j,temp[j]);
            j++;
        }
    }
    command_arg[numArg + 1] = NULL;
   
}


// Function to check for I/O redirection symbols and set up file descriptors accordingly
void handle_io_redirection(char **args) {
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], ">") == 0) {
            int fd = open(args[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
        } else if (strcmp(args[i], "<") == 0) {
            int fd = open(args[i + 1], O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
        }
    }
}

// Function to process a single command with possible input/output redirection
void process_command(char **args) {
    handle_io_redirection(args);

    if (strcmp(args[0], "history") == 0) {
        display_history();
    } else {
        if (execvp(args[0], args) == -1) {
            printf("%s: command not found\n", args[0]);
            exit(EXIT_SUCCESS);
        }
    }
}

// Function to execute a single command or a pipeline of commands
void execute_command(char *str) {
    add_history(str);

    // Split commands by '|' symbol
    char *commands[256];
    int num_commands = 0;
    char *token = strtok(str, "|");
    while (token) {
        commands[num_commands++] = token;
        token = strtok(NULL, "|");
    }
    commands[num_commands] = NULL;

    int pipes[num_commands - 1][2];
    for (int i = 0; i < num_commands - 1; i++) {
        pipe(pipes[i]);
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            printf("Fork failed\n");
            exit(1);
        } else if (pid == 0) {
            // Set up pipe input/output for the current command
            if (i != 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
                close(pipes[i - 1][1]);
                    }
            if (i != num_commands - 1) {
            dup2(pipes[i][1], STDOUT_FILENO);
            close(pipes[i][0]);
            close(pipes[i][1]);
            }
        // Split the command into arguments
        char **args = (char **)malloc(256 * sizeof(char *));
        for (int j = 0; j < 256; j++) {
            args[j] = (char *)malloc(256 * sizeof(char));
        }
        get_command_and_arg(commands[i], args);

        // Process the command
        process_command(args);

        for (int j = 0; j < 256; j++) {
            free(args[j]);
        }
        free(args);

        exit(EXIT_SUCCESS);
    }
}

// Close all pipe ends in the parent process
for (int i = 0; i < num_commands - 1; i++) {
    close(pipes[i][0]);
    close(pipes[i][1]);
}

// Wait for all child processes to finish
for (int i = 0; i < num_commands; i++) {
    wait(NULL);
}

}

int main(int argc, char *argv[]) {
char str[1024];
while (1) {
time_t curr = time(NULL);
struct tm *t = localtime(&curr);
char temp[256];
strftime(temp, sizeof(temp), "%Y/%m/%d %H:%M:%S", t);
printf("%s ", temp);
    fgets(str, 1024, stdin);
    str[strlen(str) - 1] = '\0';
    if (is_exit(str) == 1) {
        printf("Bye !\n");
        exit(0);
    }
    execute_command(str);
}