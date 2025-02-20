#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    int rc = 0;
    cmd_buff_t cmd;

    // TODO IMPLEMENT MAIN LOOP
    while(1){
	printf("%s", SH_PROMPT);
	if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
		printf("\n");
		break;
         }
    cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    rc = build_cmd_list(cmd_buff, &cmd);

    if (rc == WARN_NO_CMDS){
        printf(CMD_WARN_NO_CMD);
        continue;
    }
    if (rc != OK){
        printf("Error: %d\n", rc);
        continue;
    }
    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    if (strcmp(cmd.argv[0], EXIT_CMD) == 0){
        break;
    }
    if (strcmp(cmd.argv[0], "cd") == 0){
        if (cmd.argc == 1){
            continue;
        }
        else
        {
            if (chdir(cmd.argv[1]) != 0){
                perror("cd");
            }
        }
        continue;
    }


    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    pid_t pid = fork();
    if (pid == 0)
    {
        // Child
        execvp(cmd.argv[0], cmd.argv);
        perror("Command execution error");
        exit(ERR_EXEC_CMD);
    } else if (pid > 0) {
        // Parent
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)){
            int exit_status = WEXITSTATUS(status);
        if (exit_status != 0)
        {
            printf("Command failed with exit status %d\n", exit_status);
        }
    } else {
        printf("Command terminated abnormally\n");
        }
    } else {
        perror("fork");
    }

    }
    return OK;

}

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, cmd_buff_t *cmd)
{
    if (cmd == NULL || cmd_line == NULL || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    cmd->argc = 0;
    cmd->_cmd_buffer = strdup(cmd_line);

    if (cmd->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    char *ptr = cmd->_cmd_buffer;
    bool in_quotes = false;
    char *start = NULL;

    // Leading spaces removal
    while (*ptr != '\0') {
        while (*ptr == SPACE_CHAR) {
            ptr++;
        }

        if (*ptr == '\0') {
            break;
        }

	start = ptr;

        if (*ptr == '"') {
            in_quotes = !in_quotes;
	    ptr++;
    	    start = ptr;        
	    while(*ptr != '\0' && (in_quotes || *ptr != SPACE_CHAR)) {
                if (*ptr == '"') {
                    in_quotes = !in_quotes;
        	    *ptr = '\0';
		    ptr++;
		    break;
		}
                ptr++;
            }
        } else {
	    while(*ptr != SPACE_CHAR && *ptr != '\0') {
		    ptr++;
	    }
	}

        if (*ptr == '\0' || *ptr == SPACE_CHAR) {
            char end_char = *ptr;
            *ptr = '\0';
            cmd->argv[cmd->argc++] = strdup(start);
            if (end_char == '\0') {
                break;
            }
            ptr++;
        }

        if (cmd->argc >= CMD_ARGV_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
    }

    cmd->argv[cmd->argc] = NULL;

    if (cmd->argc == 0) {
        return WARN_NO_CMDS;
    }

    return OK;
}

