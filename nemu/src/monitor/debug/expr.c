#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ, TK_AND, TK_OR, TK_NOT, TK_NUM, TK_HEX, TK_LP, TK_RP, TK_REG
  /* TODO: Add more token types */
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // star
  {"\\/", '/'},         // div
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_NEQ},       // and
  {"||", TK_NEQ} ,      // or
  {"!", TK_NEQ},        // not
  {"\\(", TK_LP},       // lp
  {"\\)", TK_RP},       // rp
  {"0[xX][0-9a-zA-Z]{1,32}", TK_HEX}, // hex  
	{"\\$[a-z]{2,3}", TK_REG},   //reg
	{"[0-9]{1,32}", TK_NUM}, //num

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i,j;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        tokens[nr_token].type = rules[i].token_type;
        switch (rules[i].token_type) {
          /*case TK_EQ:
          case TK_NEQ:
          case TK_AND:
          case TK_OR:
          case TK_NOT: 
          case TK_NUM:
          case TK_HEX:
          case TK_LP:
          case TK_RP:
          case TK_REG:
          case '+':
          case '-':
          case '*':
          case '/':*/
          case TK_NOTYPE: break;
          default:
            tokens[nr_token].type=rules[i].token_type;
            for(j=0;j<substr_len;j++){
              tokens[nr_token].str[j]=substr_start[j];
            }
            nr_token ++;
        }
        //nr_token ++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool eval(int p, int q) {
  int value;
  if (p>q) {
    panic(" Bad expression ");
    return false;
  }
  else if (p==q) {
    if (tokens[p].type == TK_HEX){
      sscanf(tokens[p].str,"%x", &value);
      return value;
    }
    else if (tokens[p].type == TK_NUM) {
      sscanf(tokens[p].str,"%d", &value);
      return value;
    }
    else if (tokens[p].type == TK_REG) {
      if (!strcmp(tokens[p].str,"$eax")) return cpu.eax;
			else if (!strcmp(tokens[p].str,"$ecx")) return cpu.ecx;
			else if (!strcmp(tokens[p].str,"$edx")) return cpu.edx;
			else if (!strcmp(tokens[p].str,"$ebx")) return cpu.ebx;
			else if (!strcmp(tokens[p].str,"$esp")) return cpu.esp;
			else if (!strcmp(tokens[p].str,"$ebp")) return cpu.ebp;
			else if (!strcmp(tokens[p].str,"$esi")) return cpu.esi; 
			else if (!strcmp(tokens[p].str,"$edi")) return cpu.edi;
			else if (!strcmp(tokens[p].str,"$eip")) return cpu.eip;
			else panic(" Bad expression ");
    }
    else {
      panic(" Bad expression ");
    }
  }
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token-1);
}
