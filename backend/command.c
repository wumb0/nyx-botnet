#include "command.h"

int *cmd_queue(Command *cmd, Commands cmds){
    if ( cmds.cmd_start == (void *)0 ) {
        cmds.cmd_start = cmd;
        cmds.queue_len++;
        return 0;
    }
    Command *c = cmd_append(cmd,cmds.cmd_start);
    if ( c != (void *)0 ) {
        cmds.queue_len++;
    }
    return 0;
}


Command *cmd_dequeue(Commands cmds) {
    if ( cmds.cmd_start == (void *)0 ) {
        return (void *)0;
    }
    
    // ignore completed commands
    Command *c = cmds.cmd_start;
    Command *p = (void *)0;
    while ( (*c).complete == 1 ) {
        p = c;
        c = (*c).next;
        if ( c == (void *)0 ) {
            return (void *)0;
        }
    }

    (*c).complete = 1;
    
    // remove from queue if not persistent
    if ( p == (void *)0 && !(*c).persist ) {
        cmds.cmd_start = (*cmds.cmd_start).next;
    } else if ( !(*c).persist ) {
        (*p).next = (*c).next;
        //add to finished list
        Command *f = cmd_append(c,cmds.cmd_completed);
        if ( f != (void *)0 ) {
            cmds.fin_len++;
        }
        cmds.queue_len--;
    }
    return c;
}

Command *cmd_append(Command *cmd, Command *start) {
    if ( start == (void *)0 ) {
        return (void *)0;
    }
    
    Command *cur = start;
    while ( cur->next != (void *)0 ){
        cur = cur->next;
    }
    cur->next = cmd;
    return cur;
}


void cmd_reset_complete(Commands cmds) {

    if ( cmds.cmd_start == (void *)0 ) {
        return;
    }

    Command cur = *cmds.cmd_start;
    while ( cur.next != (void *)0 ){
            cur.complete = 0;
        cur = (*cur.next);
    }
    cur.complete = 0;
}
