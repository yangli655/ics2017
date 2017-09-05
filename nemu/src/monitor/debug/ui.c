#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_p(char *args);

static int cmd_x(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Let the program step by step after the implementation of n instructions to suspend\
  execution, when n is not given, the default is 1", cmd_si },
  { "info", "Print register status or monitor point information", cmd_info },
  { "p", "Find the value of the expression EXPR", cmd_p },
  { "x", "Find the value of the expression EXPR,the result as a starting memory address, \
  hexadecimal form of the output of the continuous n 4 bytes", cmd_x },
  { "w", "When the value of the expression EXPR changes, the program execution is suspended", cmd_w },
  { "d", "Delete the monitoring point with serial number n", cmd_d },
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  int i;
  if (args == NULL) {
    cpu_exec(1);
  }
  else {
    i=atoi(args);
    cpu_exec(i);
  }
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) {
    printf("Instruction requires parameters\n");
  }
  else {
    if (!strcmp(args, "r")) {
      printf("eax: 0x%08x\t\tecx: 0x%08x\t\tedx: 0x%08x\t\tebx: 0x%08x\n", cpu.eax, cpu.ecx, cpu.edx, cpu.ebx);
      printf("esp: 0x%08x\t\tebp: 0x%08x\t\tesi: 0x%08x\t\tedi: 0x%08x\n", cpu.esp, cpu.ebp, cpu.esi, cpu.edi);
      printf("eip: 0x%08x\n", cpu.eip);
    }
    else if (strcmp(args, "w") == 0) {
      return 0;
    }
    else {
      printf("Unknown instruction parameters '%s'\n", args);
    }
  }
  return 0;
}

static int cmd_p(char *args) {
  bool flag;
  if (args == NULL) {
    printf("Instruction requires expression\n");
  }
  else {
    int value = expr(args, &flag);
    printf("result:\t%d\thex:\t0x%x\n", value,value);
  }
  
  return 0;
}

static int cmd_x(char *args) {
  int num,i;
  char EXPR[32];
  bool flag;
  if (args == NULL) {
    printf("Instruction requires parameters\n");
  }
  else {
    sscanf(args, "%d %s", &num, EXPR);
    if(strlen(EXPR)) {
      int value = expr(EXPR, &flag);
      for(i=0;i<num;i++) {
        printf("%x\t",vaddr_read(value+i*4, 4));
      }
    }
    else {
      printf("Instruction requires parameters\n");
    }
  }
  
  return 0;
}

static int cmd_w(char *args) {
  return 0;
}

static int cmd_d(char *args) {
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
