#ifndef __command
#define __command
typedef struct Command{
    int complete;
    int persist;
    char *cmd;
    struct Command *next;
} Command;

typedef struct
{
    int queue_len;
    int fin_len;
    Command *cmd_start;
    Command *cmd_completed;
} Commands;

#endif

Command *cmd_queue(Command *cmd, Commands cmds);

Command *cmd_dequeue(Commands cmd);

Command *cmd_append(Command *cmd, Command *start);

void cmd_reset_complete(Commands cmds);
