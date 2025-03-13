#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

// Existing code
int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    int rc = 0;
    command_list_t clist; // Use command_list_t instead of cmd_buff_t
    int num_cmds = 0;

    while (1) {
        // Print the prompt only before taking user input
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        rc = build_cmd_list(cmd_buff, &clist); // Adjusted to use command_list_t
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }
        if (rc != OK) {
            printf("Error: %d\n", rc);
            continue;
        }

        num_cmds = clist.num; // Update num_cmds based on clist

        if (strcmp(clist.commands[0].argv[0], EXIT_CMD) == 0) {
            printf("exiting...\n");
            break;
        }

        if (strcmp(clist.commands[0].argv[0], "cd") == 0) {
            if (clist.commands[0].argc == 1) {
                continue;
            } else {
                if (chdir(clist.commands[0].argv[1]) != 0) {
                    perror("cd");
                }
            }
            continue;
        }

        // Setup pipes and execute commands
        int pipes[num_cmds - 1][2];
        pid_t pids[num_cmds];

        for (int i = 0; i < num_cmds - 1; i++) {
            if (pipe(pipes[i]) < 0) {
                perror("pipe");
                return -1;
            }
        }

        for (int i = 0; i < num_cmds; i++) {
            pids[i] = fork();
            if (pids[i] < 0) {
                perror("fork");
                return -1;
            }
            if (pids[i] == 0) {
                // Close all pipes that are not being used
                if (i > 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO); // Get input from the previous pipe
                }
                if (i < num_cmds - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO); // Output to the next pipe
                }

                // Close all pipe descriptors in child process
                for (int j = 0; j < num_cmds - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                execvp(clist.commands[i].argv[0], clist.commands[i].argv); // Execute the command
                perror("execvp");
                exit(ERR_EXEC_CMD);
            }
        }

        // Parent process closes all pipes
        for (int i = 0; i < num_cmds - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // Wait for all child processes to finish
        for (int i = 0; i < num_cmds; i++) {
            int status;
            waitpid(pids[i], &status, 0);
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status != 0) {
                    printf("Command failed with exit status %d\n", exit_status);
                }
            } else {
                printf("Command terminated abnormally\n");
            }
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
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (clist == NULL || cmd_line == NULL || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    clist->num = 0;
    char *ptr = cmd_line;
    bool in_quotes = false;
    char *start = NULL;

    while (*ptr != '\0') {
        while (*ptr == SPACE_CHAR) {
            ptr++;
        }

        if (*ptr == '\0') {
            break;
        }
        if (clist->num > 8) {
            printf("error: piping limited to 8 commands\n");
            return -2; 
        }

        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        cmd_buff_t *cmd = &clist->commands[clist->num];
        cmd->argc = 0;
        cmd->_cmd_buffer = strdup(ptr);
        if (cmd->_cmd_buffer == NULL) {
            return ERR_MEMORY;
        }

        char *cmd_ptr = cmd->_cmd_buffer;

        while (*cmd_ptr != '\0') {
            while (*cmd_ptr == SPACE_CHAR) {
                cmd_ptr++;
            }
            if (*cmd_ptr == '\0' || *cmd_ptr == PIPE_CHAR) {
                break;
            }

            start = cmd_ptr;

            if (*cmd_ptr == '"') {
                in_quotes = !in_quotes;
                cmd_ptr++;
                start = cmd_ptr;
                while (*cmd_ptr != '\0' && (in_quotes || *cmd_ptr != SPACE_CHAR)) {
                    if (*cmd_ptr == '"') {
                        in_quotes = !in_quotes;
                        *cmd_ptr = '\0';
                        cmd_ptr++;
                        break;
                    }
                    cmd_ptr++;
                }
            } else {
                while (*cmd_ptr != SPACE_CHAR && *cmd_ptr != '\0' && *cmd_ptr != PIPE_CHAR) {
                    cmd_ptr++;
                }
            }

            if (*cmd_ptr == '\0' || *cmd_ptr == SPACE_CHAR || *cmd_ptr == PIPE_CHAR) {
                char end_char = *cmd_ptr;
                *cmd_ptr = '\0';
                cmd->argv[cmd->argc++] = strdup(start);
                if (end_char == '\0' || end_char == PIPE_CHAR) {
                    break;
                }
                cmd_ptr++;
            }

            if (cmd->argc >= CMD_ARGV_MAX) {
                return ERR_TOO_MANY_COMMANDS;
            }
        }

        cmd->argv[cmd->argc] = NULL;

        if (cmd->argc == 0) {
            return WARN_NO_CMDS;
        }

        clist->num++;

        while (*ptr != '\0' && *ptr != PIPE_CHAR) {
            ptr++;
        }
        if (*ptr == PIPE_CHAR) {
            ptr++;
        }
    }

    return OK;
}

