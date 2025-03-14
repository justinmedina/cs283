#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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

char *trim_whitespace(char *str)
{
    while (isspace((unsigned char)*str))
        str++; // Trim leading spaces
    if (*str == 0)
        return str; // Empty string

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    *(end + 1) = '\0';
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
            return OK;
        if (chdir(cmd->argv[1]) != 0)
        {
            perror("cd");
            return ERR_EXEC_CMD;
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

/**
 * Parses the input command line into a list of commands, handling pipes.
 */
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
                arg_ptr++;

            if (*arg_ptr == '"' || *arg_ptr == '\'')
            {
                char quote = *arg_ptr++;
                char *start = arg_ptr;
                while (*arg_ptr && *arg_ptr != quote)
                    arg_ptr++;

                if (*arg_ptr == quote)
                    *arg_ptr++ = '\0';

                cmd_buff->argv[cmd_buff->argc++] = start;
            }
            else
            {
                char *start = arg_ptr;
                while (*arg_ptr && !isspace((unsigned char)*arg_ptr))
                    arg_ptr++;

                if (*arg_ptr)
                    *arg_ptr++ = '\0';

                cmd_buff->argv[cmd_buff->argc++] = start;
            }
        }

        cmd_buff->argv[cmd_buff->argc] = NULL;
        cmd_count++;
    }

    clist->num = cmd_count;
    return OK;
}

void handle_redirection(cmd_buff_t *cmd)
{
    int in_fd = -1, out_fd = -1;
    int new_argc = 0;

    for (int i = 0; i < cmd->argc; i++)
    {
        if (strcmp(cmd->argv[i], "<") == 0 && i + 1 < cmd->argc)
        {
            in_fd = open(cmd->argv[i + 1], O_RDONLY);
            if (in_fd == -1)
            {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            i++; // Skip the filename argument
        }
        else if (strcmp(cmd->argv[i], ">") == 0 && i + 1 < cmd->argc)
        {
            out_fd = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd == -1)
            {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            i++; // Skip the filename argument
        }
        else if (strcmp(cmd->argv[i], ">>") == 0 && i + 1 < cmd->argc)
        {
            out_fd = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (out_fd == -1)
            {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            i++; // Skip the filename argument
        }
        else
        {
            cmd->argv[new_argc++] = cmd->argv[i];
        }
    }

    cmd->argv[new_argc] = NULL;
    cmd->argc = new_argc;

    if (in_fd != -1)
    {
        if (dup2(in_fd, STDIN_FILENO) == -1)
        {
            perror("Failed to redirect input");
            exit(EXIT_FAILURE);
        }
        close(in_fd);
    }

    if (out_fd != -1)
    {
        if (dup2(out_fd, STDOUT_FILENO) == -1)
        {
            perror("Failed to redirect output");
            exit(EXIT_FAILURE);
        }
        close(out_fd);
    }
}

int exec_command(cmd_buff_t *cmd)
{
    Built_In_Cmds built_in_status = exec_built_in_cmd(cmd);
    if (built_in_status == BI_EXECUTED)
    {
        return OK;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        handle_redirection(cmd);
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp"); // Prints error message
        exit(127);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status); // Properly return the child’s exit status
        }
    }
    else
    {
        perror("fork");
        return ERR_EXEC_CMD;
    }

    return ERR_EXEC_CMD;
}

int execute_pipeline(command_list_t *clist)
{
    if (clist->num > CMD_MAX)
    {
        fprintf(stderr, "error: piping limited to %d commands\n", CMD_MAX);
        return 1; // Exit with failure status
    }

    int num_cmds = clist->num;
    int pipes[2 * (num_cmds - 1)];
    pid_t pids[num_cmds];

    for (int i = 0; i < num_cmds - 1; i++)
    {
        if (pipe(pipes + i * 2) == -1)
        {
            perror("pipe");
            return ERR_MEMORY;
        }
    }

    for (int i = 0; i < num_cmds; i++)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0)
        {
            if (i > 0)
            {
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < num_cmds - 1)
            {
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }

            for (int j = 0; j < 2 * (num_cmds - 1); j++)
            {
                close(pipes[j]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp failed");
            exit(127);
        }
    }

    for (int i = 0; i < 2 * (num_cmds - 1); i++)
    {
        close(pipes[i]);
    }

    int status = 0;
    for (int i = 0; i < num_cmds; i++)
    {
        waitpid(pids[i], &status, 0);
    }

    return WIFEXITED(status) ? WEXITSTATUS(status) : ERR_EXEC_CMD;
}

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
            return 1; // Ensure the shell exits with an error status
        }

        int status = 0;
        if (clist.num == 1)
        {
            status = exec_command(&clist.commands[0]);
        }
        else
        {
            status = execute_pipeline(&clist);
        }

        if (status != 0)
        {
            fprintf(stderr, CMD_ERR_EXECUTE); // Print error message if command fails
        }
    }
    return 0;
}

