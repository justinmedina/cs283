#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 *
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 *
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 *
 *   Also, use the constants in the dshlib.h in this code.
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 *
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 *
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 *
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

/**
 * Trims leading and trailing whitespace from a string.
 */
char *trim_whitespace(char *str)
{
    while (isspace((unsigned char)*str))
        str++; // Trim leading spaces
    if (*str == 0)
        return str; // Empty string

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;         // Trim trailing spaces
    *(end + 1) = '\0'; // Null-terminate
    return str;
}

/**
 * Executes built-in commands like cd and exit.
 */
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    if (strcmp(cmd->argv[0], "cd") == 0)
    {
        if (cmd->argc == 1)
            return OK; // Do nothing if no argument provided
        if (chdir(cmd->argv[1]) != 0)
        {
            perror("cd");
            return ERR_EXEC_CMD;
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    memset(clist, 0, sizeof(command_list_t));

    char *token;
    char *rest = cmd_line;
    int cmd_count = 0;

    while ((token = strtok_r(rest, PIPE_STRING, &rest)))
    {
        token = trim_whitespace(token);

        if (cmd_count >= CMD_MAX)
            return ERR_TOO_MANY_COMMANDS;

        cmd_buff_t *cmd_buff = &clist->commands[cmd_count];
        cmd_buff->argc = 0;

        char *arg_ptr = token;
        while (*arg_ptr)
        {
            while (isspace((unsigned char)*arg_ptr))
                arg_ptr++; // Skip leading spaces

            if (*arg_ptr == '"' || *arg_ptr == '\'')
            {
                // Handle quoted argument
                char quote = *arg_ptr++;
                char *start = arg_ptr;
                while (*arg_ptr && *arg_ptr != quote)
                    arg_ptr++;

                if (*arg_ptr == quote)
                    *arg_ptr++ = '\0'; // Remove closing quote

                cmd_buff->argv[cmd_buff->argc++] = start;
            }
            else
            {
                // Handle normal argument
                char *start = arg_ptr;
                while (*arg_ptr && !isspace((unsigned char)*arg_ptr))
                    arg_ptr++;

                if (*arg_ptr)
                    *arg_ptr++ = '\0'; // Null-terminate argument

                cmd_buff->argv[cmd_buff->argc++] = start;
            }
        }

        cmd_buff->argv[cmd_buff->argc] = NULL; // Null-terminate args
        cmd_count++;
    }

    clist->num = cmd_count;
    return OK;
}

/**
 * Handles input (<) and output (>, >>) redirection.
 */
void handle_redirection(cmd_buff_t *cmd)
{
    int in_fd = -1, out_fd = -1;

    // Scan for redirection operators
    for (int i = 0; i < cmd->argc; i++)
    {
        if (strcmp(cmd->argv[i], "<") == 0)
        {
            in_fd = open(cmd->argv[i + 1], O_RDONLY);
            if (in_fd == -1)
            {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            cmd->argv[i] = NULL;
        }
        else if (strcmp(cmd->argv[i], ">") == 0)
        {
            out_fd = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd == -1)
            {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            cmd->argv[i] = NULL;
        }
        else if (strcmp(cmd->argv[i], ">>") == 0)
        {
            out_fd = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (out_fd == -1)
            {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            cmd->argv[i] = NULL;
        }
    }

    if (in_fd != -1)
    {
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }
    if (out_fd != -1)
    {
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }
}

/**
 * Executes a single command.
 */
int exec_command(cmd_buff_t *cmd)
{
    Built_In_Cmds built_in_status = exec_built_in_cmd(cmd);
    if (built_in_status == BI_EXECUTED)
    {
        return OK;
    }

    // Handle redirection before forking
    handle_redirection(cmd);

    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        perror("fork");
        return ERR_EXEC_CMD;
    }

    return OK;
}

int execute_pipeline(command_list_t *clist)
{
    int num_cmds = clist->num;
    int pipes[2 * (num_cmds - 1)];
    pid_t pids[num_cmds];

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++)
    {
        if (pipe(pipes + i * 2) == -1)
        {
            perror("pipe");
            return ERR_MEMORY;
        }
    }

    // Fork and execute commands
    for (int i = 0; i < num_cmds; i++)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0)
        { // Child process
            if (i > 0)
            { // If not the first command, read from previous pipe
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < num_cmds - 1)
            { // If not the last command, write to next pipe
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipes in child
            for (int j = 0; j < 2 * (num_cmds - 1); j++)
            {
                close(pipes[j]);
            }

            // Ensure execvp receives the correct argument list
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror(CMD_ERR_EXECUTE);
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipes in the parent process
    for (int i = 0; i < 2 * (num_cmds - 1); i++)
    {
        close(pipes[i]);
    }

    // Wait for all child processes
    for (int i = 0; i < num_cmds; i++)
    {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}

/**
 * Main shell loop: Reads, parses, and executes commands.
 */
int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    command_list_t clist;

    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        char *trimmed_cmd = trim_whitespace(cmd_buff);

        if (strlen(trimmed_cmd) == 0)
        {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        if (strcmp(trimmed_cmd, EXIT_CMD) == 0)
        {
            printf("Exiting shell...\n");
            return 0;
        }

        int result = build_cmd_list(trimmed_cmd, &clist);
        if (result == ERR_TOO_MANY_COMMANDS)
        {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }

        if (clist.num == 1)
        {
            exec_command(&clist.commands[0]);
        }
        else
        {
            execute_pipeline(&clist);
        }
    }
    return 0;
}
