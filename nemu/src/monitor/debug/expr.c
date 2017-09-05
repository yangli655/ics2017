#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ, TK_AND, TK_OR, TK_NOT, TK_NUM, TK_HEX, TK_LP, TK_RP, TK_REG, TK_MIN, TK_ADR
  /* TODO: Add more token types */
};

static struct rule {
  char *regex;
  int token_type;
  int priority;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE, 0},    // spaces
  {"\\+", '+', 4},         // plus
  {"\\-", '-', 4},         // minus
  {"\\*", '*', 3},         // star
  {"\\/", '/', 3},         // div
  {"==", TK_EQ, 7},        // equal
  {"!=", TK_NEQ, 7},       // not equal
  {"&&", TK_AND, 11},      // and
  {"||", TK_OR, 12},       // or
  {"!", TK_NOT, 2},        // not
  {"\\(", TK_LP, 1},       // lp
  {"\\)", TK_RP, 1},       // rp
  {"0[xX][0-9a-zA-Z]{1,32}", TK_HEX, 1}, // hex  
	{"\\$[a-z]{2,3}", TK_REG, 1},   //reg
	{"[0-9]{1,32}", TK_NUM, 1}, //num

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
  char str[32];
  int type;
  int priority;
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
          case '/':*/
          case TK_NOTYPE: 
            break;
          case '-':
            if(nr_token == 0 || tokens[nr_token-1].type != TK_HEX || tokens[nr_token-1].type != TK_NUM || tokens[nr_token-1].type != TK_REG) {
              tokens[nr_token].priority=2;
              tokens[nr_token].type=TK_MIN;
            }
            break;
          case '*':
            if(nr_token == 0 || tokens[nr_token-1].type != TK_HEX || tokens[nr_token-1].type != TK_NUM || tokens[nr_token-1].type != TK_REG) {
              tokens[nr_token].priority=2;
              tokens[nr_token].type=TK_ADR;
            }
            break;
          
          default:
          tokens[nr_token].priority=rules[i].priority;
            tokens[nr_token].type=rules[i].token_type;
            for(j=0;j<substr_len;j++){
              tokens[nr_token].str[j]=substr_start[j];
            }
        }
        nr_token++;
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

static bool check_parentheses(int p, int q) {
  int i,par=0;
  if (tokens[p].type == TK_LP && tokens[q].type == TK_RP) {
    for(i=p+1;i<q;i++) {
      if (tokens[i].type == TK_LP) par++;
      else if (tokens[i].type == TK_RP) par--;
      if (par<0) {
        return false;
      }
    }
    if (par == 0)
      return true;
  }
  return false;
}

uint32_t dominant_op(int p, int q) {
	int i, pos=p, par=0, min_priority=0;

	for (i = p; i <= q; i++) {
		if (tokens[i].type == TK_LP) par++;
		else if (tokens[i].type == TK_RP) par--;
    if (par == 0 && tokens[i].priority >= min_priority) {
        min_priority = tokens[i].priority;
        pos=i;
    }
  }
  if (pos > p && tokens[pos-1].priority == 2){
    pos--;
  }
	return pos;
}

static bool eval(int p, int q) {
  int value=0,val1=0,val2=0;
  if (p>q) {
    panic("Bad expression\n");
    assert(0);
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
      else panic(" Bad expression\n");
      assert(0);
    }
    else {
      panic("Bad expression\n");
      assert(0);
    }
  }
  else if (check_parentheses(p, q) == true) {
    return eval(p+1,q-1);
  }
  else {
    int op = dominant_op(p, q);
    if (tokens[op].priority == 2){
      val1=eval(p+1,q);
    }
    else {
      val1 = eval(p, op - 1);
      val2 = eval(op + 1, q);
    }
    int op_type=tokens[op].type;
    
		switch(op_type) {
			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2;
			case '/': return val1 / val2;
			case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case TK_OR: return val1 || val2;
      case TK_NOT: return !val1;
      case TK_MIN: return -val1;
      case TK_ADR: return vaddr_read(val1, 4);
      default: assert(0);
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
  *success=true;
  return eval(0, nr_token-1);
}
