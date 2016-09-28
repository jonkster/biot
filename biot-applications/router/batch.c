#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "shell.h"
#include "shell_commands.h"

static shell_command_handler_t find_handler(const shell_command_t *command_list, char *command)
{
    const shell_command_t *command_lists[] = {
        command_list,
        _shell_command_list,
    };

    const shell_command_t *entry;

    for (unsigned int i = 0; i < sizeof(command_lists) / sizeof(entry); i++) {
        if ((entry = command_lists[i])) {
            while (entry->name != NULL) {
                if (strcmp(entry->name, command) == 0) {
                    return entry->handler;
                }
                else {
                    entry++;
                }
            }
        }
    }

    return NULL;
}

/* simplified arg processor - NB will not interpret quotes */
void batch(const shell_command_t *command_list, char *line)
{
    printf("BATCH %s\n", line);
    return;
    char* argString = strdup(line);

    enum { kMaxArgs = 5 };
    int argc = 0;
    char *argv[kMaxArgs];

    char *p = strtok(argString, " ");
    while (p && argc < kMaxArgs-1)
    {
        argv[argc++] = p;
        p = strtok(0, " ");
    }
    argv[argc] = 0;
    /*for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]=%s\n", i, argv[i]);
    }*/
    
    /* then we call the appropriate handler */
    shell_command_handler_t handler = find_handler(command_list, argv[0]);
    if (handler != NULL) {
        handler(argc, argv);
    }
    else {
        printf("shell: command not found: %s\n", argv[0]);
    }
    free(argString);
}
