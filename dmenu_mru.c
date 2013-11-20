/* See LICENSE file for copyright and license details. */
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "mru.h"


#define HISTORY ".dmenu_hist"
#define MAX_HIST 1024
#define MAX_COMMAND 1024

static void die(const char *s) {
  fprintf(stderr, "dmenu_mru: %s\n", s);
  exit(EXIT_FAILURE);
}


/**
 * Read commands from stdin and return a double linked list of commands.
 */
ListEntry* read_commands() {
  ListEntry* commands = list_create();

  char line[1024];
  while (fgets(line, 1024, stdin) != NULL) { /* Read everything from stdin */
    size_t len = strlen(line) - 1;           /* Strip out newline character */
    if (line[len] == '\n')
      line[len] = '\0';
    if (len > 0) {              /* Build list of commands */
      list_add(commands, line);
    }
  }
  return commands;
}

/**
 * Read command history from file.
 */
ListEntry* read_history() {
  ListEntry* history = list_create();
  FILE *hist;
  char line [1024];

  if(!(hist = fopen(HISTORY, "r"))) { /* Open file for reading */
    return history;                   /* If history file is not there, just return an empty list */
  }

  while (fgets(line, sizeof line, hist) != NULL) { /* Read a line from file */
    size_t len = strlen(line) - 1;                 /* Strip out newline character */
    if (line[len] == '\n')
      line[len] = '\0';
    if (len > 0) {                 /* If it's non-empty, */
      list_add(history, line); /* add to list */
    }
  }
  fclose(hist);
  return history;
}

/**
 * Write commands to history file.
 */
void write_history(ListEntry* history) {
  FILE *hist;
  int i = 0;
  ListEntry* e = NULL;
  if(!(hist = fopen(HISTORY, "w"))) /* Open file for writing */
    die("failed to write history");
  /* Write out commands to file Note that we are only going to write out the first MAX_HIST
     commands */
  for(e = history->next; e != history && i < MAX_HIST; e = e->next, ++i) {
    fprintf(hist,  "%s\n", e->val);
  }
  fclose(hist);
}

/**
 * Move command to the top of the history list.
 */
void touch_history(ListEntry* history, char* command) {
  ListEntry* e = NULL;
  for(e = history->next; e != history; e = e->next) {
    if (strcmp(command, e->val) == 0) { /* Find the command in history */
      list_remove(e);
      break;                    /* History should be unique, no need to continue looking. */
    }
  }
  list_insert(history, command); /* Add command at the front */
}

/**
 * Move command to the top of the MRU history file.
 */
void touch_command(char* command) {
  ListEntry* history = read_history(); /* Read command history from file */
  touch_history(history, command);     /* Move command to the top of history */
  write_history(history);       /* Write command history back to file */
}

/**
 * Read commands from stdin and output them in MRU order.
 */
void process_commands() {
  ListEntry* e = NULL;

  ListEntry* history = read_history();   /* Read command history from file */
  ListEntry* commands = read_commands(); /* Read commands from stdin */
  HashEntry** commands_hash = hash_create(MAX_HIST, commands); /* Create hashmap for fast lookup */

  /* First output history entries that are present in the commands hash */
  for(e = history->next; e != history; e = e->next) {
    HashEntry* he = hash_get(commands_hash, MAX_HIST, e->val);
    if (he != NULL) {
      printf("%s\n", he->key);
      list_remove(he->val);   /* Remove entry from the commands list to make sure we don't output it
                                 again later */
    }
  }

  /* Then output the rest of the commands list */
  for(e = commands->next; e != commands; e = e->next) {
    printf("%s\n", e->val);
  }

  /* Free memory associated with the lists and hash */
  list_destroy(commands);
  list_destroy(history);
  hash_destroy(commands_hash, MAX_HIST);
}

/**
 * Join all elements of a string array starting at index seperated by sep and place into s.
 */
void join(int argc, char* argv[], int start, char sep, char* s, int size) {
  int i = 0;
  int len = 0;
  for(i = start; i < argc; ++i) {
    len = strlen(argv[i]);      /* Calculate the string length */
    if (size < len + 1)
      die("Command too long");  /* Make sure we don't overrun the buffer */
    strncpy(s, argv[i], len);   /* Copy string */
    s[len] = sep;               /* Replace end of string with sep */
    s += len + 1;               /* Move pointer by the amount we added */
    size -= len + 1;            /* Update size remaining */
  }
  if (argc - start > 0) {
    s[-1] = '\0';               /* NULL terminate the string for good measure */
  }
}

/**
 * This application has to modes of operation:
 *
 * 1. If no command-line arguments are passed, it will take * a list of via stdin, and move the most
 *    recently used to the front of the list.  This is useful when piping the output of dmenu_path
 *    to dmenu.  Command history is storied in ~/.dmenu_hist.
 * 2. If command-line arguments are passed, the application will update the history file
 *    (~/.dmenu_hist) and move or add the command to the front of the file.
 *
 * Together these can be used to make dmenu sort most used commands for easy access (see dmenu_do for an example).
 */
int main(int argc, char* argv[]) {
  const char *home;
  if(!(home = getenv("HOME")))  /* Get user's home dir */
    die("no $HOME");
  if(chdir(home) < 0)           /* Change to user's home dir so cache files are created here */
    die("chdir failed");

  if (argc > 1) {
    /* If a command is passed on the command line, then move this command to the top of the MRU
       history */
    char* command = malloc(MAX_COMMAND * sizeof(char));
    join(argc, argv, 1, ' ', command, MAX_COMMAND); /* Join all parameters into a single string
                                                       (skip the first because it's the executable
                                                       name) */
    touch_command(command);     /* Move command to the top of the MRU history */
    printf(command);            /* Output command so it can be executed */
  } else {                      /* Othewise, process stdin and sort commands in MRU order */
    process_commands();
  }
  return EXIT_SUCCESS;
}
