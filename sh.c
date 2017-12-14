#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sh.h"
#include "kernel.h"
#include "env.h"

// Data types

struct block
{
  char *p;
  size_t pos;
  size_t len;
  size_t limit;
};
typedef struct block block;
typedef char token;

// Some usefull defines

#define TK_EXCL  1  // !
#define TK_BROP  2  // {
#define TK_BRCL  3  // }
#define TK_CASE  4  // case
#define TK_DO    5  // do
#define TK_DONE  6  // done
#define TK_ELIF  7  // elif
#define TK_ELSE  8  // else
#define TK_ESAC  9  // esac
#define TK_FI    10 // fi
#define TK_FOR   11 // for
#define TK_IF    12 // if
#define TK_IN    13 // in
#define TK_THEN  14 // then
#define TK_UNTIL 15 // until
#define TK_WHILE 16 // while
#define TK_NL    17 // \n
#define TK_CR    18 // \r
#define TK_SC    19 // ;
#define TK_PO    20 // (
#define TK_PC    21 // )

#define END_OF_BLOCK(a) (a.pos == a.limit)
#define END_OF_PBLOCK(a) (a->pos == a->limit)

// Arrays with the shell tokens, values and other info

const char PROGMEM c[] = "!{};\n\r()dofiifinforcasedoneelifelseesacthenuntilwhile";
const token PROGMEM ta[] = {TK_EXCL, TK_BROP, TK_BRCL, TK_SC, TK_NL, TK_CR, TK_PO, TK_PC, \
                            TK_DO, TK_FI, TK_IF, TK_IN, TK_FOR, TK_CASE, TK_DONE, TK_ELIF, \
                            TK_ELSE, TK_ESAC, TK_THEN, TK_UNTIL, TK_WHILE};
// values grouped in three values: start, end, start_in_token_array
const uint8_t PROGMEM d[] = {0, 8, 0, 8, 16, 8, 16, 19, 12, 19, 43, 13, 43, 53, 19};
/*  token string length:     \_ 1 _/  \__ 2 _/  \___ 3 __/  \___ 4 __/  \___ 5 __/
    values:                  |  |  \-->start in ta array
                             |  \--> limit in c array
                             \--> start in c array 
*/

// Private function prototypes

uint8_t syntax_if(block *a);
uint8_t eval_if(block a, char *env[]);
uint8_t eval_command(block a, char *env[]);

// Functions

uint8_t is_char(char c)
/* Check if a byte is a charater, distinguish between upper and lower case
 *  
 * Returns: 0 -> is not a char
 *          1 -> char lowercase
 *          2 -> char uppercase
 */
{
	if (c >= 'a' && c <='z')
		return true;
	if (c >= 'A' && c <='Z')
		return 2;
	return false;
}

uint8_t is_number(char c)
/* Check if a byte is a number
 * 
 * Returns: 0 -> not a number
 *          1 -> it's a number
 */
{
	if (c >= '0' && c<='9')
		return true;
	return false;
}

uint8_t is_varname_ok(char *t, size_t n)
/* Check if a string forms a valid variable name or not.
 * This function checks only the name, the "$" character is not included
 * 
 * Input: string, string lenght
 * Returns: 0 -> not valid
 *          1 -> valid
 *          2 -> special variable (builtins)
 */
{
	size_t i = 0;

  // empty var names are not welcome
  if (n == 0)
    return false;

  // Check for special variables (1 byte length)
  if ((n == 1) && (t[0] == '?' || t[0] == '$'))
    return 2;

  // A variable should contain just characters, numbers 
  // (but not start with one) and underscores
	while (i < n)
	{
		if (! (is_char(t[i]) || (is_number(t[i]) && i) || t[i] == '_'))
			return false;
		i++;
	}
	return true;
}

uint8_t is_var(char *t, size_t n)
/* Check if a string is a variable (including the starting "$")
 * 
 * Input: string, string length
 * Returns: 1 -> It's a valid variable name
 *          2 -> it's not a valid variable name
 */
{
  if (n > 1 && t[0] == '$' && is_varname_ok(t + 1, n - 1))
    return true;

  return false;
}

uint8_t is_var_assign(char *t, size_t n)
/* Check if a string is a variable assignment (eg.: abc=3)
 * Finds the position of the '=' character and returns it
 * 
 * Input: string, string length
 * Returns: 0 -> is not a variable assignment
 *          >0 -> position of the '=' character
 */
{
  uint8_t i = 1;
  
  // find '=' character position
  while (i < n)
  {
    if (t[i] == '=')
      break;
    i++;
  }

  // If we didn't reach the end of the string, we must have found it
  if (i < n)
    return i;
  return false;
}

token str_to_token(block a)
/* Given a block (a substring inside a string), return the token value or 0
 * if it's not a valid token
 * 
 * Input: a block structure
 * Return: 0 -> invalid token
 *         >0 -> token number
 */
{
  char *t = a.p + a.pos;
  uint8_t i, start, limit, tpos;

  // strings with no lenght or greater than 5 cannot be valid tokens
  if (a.len > 5 || a.len == 0)
    return false;

  start = pgm_read_byte(&d[(a.len * 3) - 3]);
  limit = pgm_read_byte(&d[(a.len * 3) - 2]);
  tpos  = pgm_read_byte(&d[(a.len * 3) - 1]);
  // compare the string with every token of the same lenght
  for (i = start; i < limit; i += a.len)
  {
    // if found, return token
    if (strncmp_P((const char *)t, &c[i], a.len) == 0)
      return pgm_read_byte((void *) &ta[tpos]);
    tpos++;
  }
  return false;
}

uint8_t get_string(block *a)
/* Important function that recognizes, inside a block (a string), the next word 
 * (separated by spaces) or substring (taking into account "" or '')
 * 
 * Input: block
 * Returns: block with modified pos and len fields
 *          0 -> could not select any more words/substring
 *          1 -> word/substring selected
 */
{
  char *s;
  size_t pos;
  uint8_t par = 0;

  s = a->p;
  a->len = 0;

  // remove begining spaces
  while (!END_OF_PBLOCK(a) && s[a->pos] == ' ')
    a->pos++;

  pos = a->pos;

  // delimit the string, considering the quotes
  while (!END_OF_PBLOCK(a))
  {
    // take quotes into account, when we found one, continue until find the pair
    switch (s[pos])
    {
      case '\"':
        if (par == 0)
          par = 1;
        else if (par == 1)
          par = 0;
        break;
      case '\'':
        if (par == 0)
          par = 2;
        else if (par == 2)
          par = 0;
        break;
      case ' ':
      case ';':
      case '\n':
      case '\r':
        // if we found any of this chars, then finish the string
        if (par == 0)
        {
          if (a->len == 0)
            a->len++;
          // exit loop
          goto _exit;
        }
    }

    a->len++;
    pos++;
  }

_exit:
  // if we got someting, great!
  if (a->len > 0)
    return true;
  return false;

}

void extend_to_eol(block *a)
/* Given a block with a substring selected, extend the selection until an end of
 * line is found.
 * 
 * Input: block
 * Returns: Modify block length
 */
{
  token tok;
  block b;

  // init a new block with data
  b.p = a->p;
  b.pos = a->pos + a->len;
  b.len = 0;
  b.limit = a->limit;

  // loop until a valid token is found (\n, \r, ;)
  do
  {
    get_string(&b);
    tok = str_to_token(b);
    if (tok == TK_NL || tok == TK_CR || tok == TK_SC)
      break;
    b.pos += b.len;
  } while (!END_OF_BLOCK(b));

  // adjust the length
  a->len = b.pos - a->pos;
}

uint8_t str_to_argv(block a, char *dst, char *argv[], char *env[])
{
  size_t i = 0;
  uint8_t ret = 0, len, j = 1;

  argv[0] = dst;

  // first, copy the string and separate args
  while (!(ret || END_OF_BLOCK(a)))
  {
    // TODO: need to thread " " and ' '
    if (a.p[a.pos] == ' ')
    {
      dst[i] = 0;
      // start pointing to the new arg
      argv[j++] = dst + i + 1;
    }
    else
      dst[i] = a.p[a.pos];
    i++;
    a.pos++;
    if (i >= ARGMAX)
      ret = 1; // line too long
    if (j >= NCARGS)
      ret = 2; // too many args
  }

  if (!ret)
    dst[i] = 0;

  // check if any arg is a variable and substitute
  for (i = 0; i<j; i++)
  {
    len = strlen(argv[i]);
    if (is_var(argv[i], len))
      argv[i] = env_get_l(env, argv[i] + 1, len - 1);
  }
  
  for (; j < NCARGS; j++)
    argv[j] = NULL;

  return ret;
}

void init_block(block src, block *dst)
{
  dst->p = src.p + src.pos;
  dst->pos = 0;
  dst->len = 0;
  dst->limit = src.len;
}

uint8_t eval(char *cmd, size_t limit, char *env[])
{
	token tok;
	uint8_t error = 0;
  block a, b;
  char ret[4];

  a.p = cmd;
  a.len = 0;
  a.pos = 0;
  a.limit = limit;

  do
	{
    get_string(&a);
    if (END_OF_BLOCK(a))
      break;
		tok = str_to_token(a);

		switch (tok)
		{
      // ignore new lines, carrier returns and semicolons
      case TK_CR:
      case TK_NL:
      case TK_SC:
        break;

#if !defined CMD_SH_SIMPLE || CMD_SH_SIMPLE != true
      // If structure
      case TK_IF:
        // syntax check the if statement and find the limits
        if ((error = syntax_if(&a)) > 0)
          break;
        // build the if block
        //init_block(a, &b);
        b.p = a.p + a.pos;
        b.pos = 0;
        b.len = 0;
        b.limit = a.len;
        error = eval_if(b, env);
				break;

			// for structure
			case TK_FOR:
				// TODO
				break;
#endif

      // a simple command followed by parameters until '\n' or ';'
      case false:
        extend_to_eol(&a);
        // build the command block
        //init_block(a, &b);
        b.p = a.p + a.pos;
        b.pos = 0;
        b.len = 0;
        b.limit = a.len;
        error = eval_command(b, env);
        break;

			default:
				printf_P(PSTR("Syntax error: %s\n"), a.p + a.pos);
				return 1;
		}
    // advance cursor to the next block
    a.pos += a.len;
    a.len = 0;
    
    snprintf_P(ret, 3, PSTR("%d"), error);
    env_add(env, "?", ret);
        
	} while (!error);

	return error;
}

uint8_t eval_command(block a, char *env[])
{
  char error = 0;
  char str[ARGMAX];
  char *argv[NCARGS];
  uint8_t pos;
  
  // Check if is a variable assigment
  if ((pos = is_var_assign(a.p, a.limit)) != false)
  {
    if (!is_varname_ok(a.p, pos))
      return 1;
    //return env_add_str(env, *cmd, *len, pos);
    return env_add_l(env, a.p, pos, a.p + pos + 1, a.limit - pos - 1);
  }

  // Discard empty commands
  if (a.p[0] == 0)
    return error;
  str_to_argv(a, str, argv, env);
  error = execve((const char **) argv, env);

  return error;  
}

uint8_t find_else_fi(block a, size_t *pos)
{
  uint8_t found = 0;
  uint8_t count = 0;
  token prev = 0;
  token tok;

  // search for "if" "else" of "fi" until found or until end of block
  while (!END_OF_BLOCK(a) && found == 0)
  {
    get_string(&a);
    tok = str_to_token(a);
    switch(tok)
    {
      case TK_IF:  // found an anidated if, look for the fi
        count++;
        break;
      case TK_ELSE:
        if (!count)
          if (prev == TK_NL || prev == TK_CR || prev == TK_SC)
            found = 2;
        break;
      case TK_FI:
        if (count)
          count--;
        else
          if (prev == TK_NL || prev == TK_CR || prev == TK_SC)
            found = 3;
        break;      
    }
    if (!found)
    {
      a.pos += a.len;
      prev = tok;
    }
  }

  *pos = a.pos;

  return found;
}

// Syntax check for "if" blocks
uint8_t syntax_if(block *a)
{
  block b;
  token tok = 0;
  uint8_t found = 0;
  uint8_t state = 0;
  uint8_t error = 0;
  size_t pos;

  memcpy(&b,a,sizeof(block));

  while (state < 8)
  {
    if (state != 4 && state != 6)
    {
      get_string(&b);
      if (state != 1)
        tok = str_to_token(b);
    }
    else
      found = find_else_fi(b, &pos);

    switch (state)
    {
      case 0: // check if token
        if (tok != TK_IF)
          error = 1;
        break;
      case 1: // jump condition
        extend_to_eol(&b);
        break;
      case 2: // check new line
        if (tok != TK_NL && tok != TK_CR && tok != TK_SC)
          error = 1;
        break;
      case 3: // check then token
        if (tok != TK_THEN)
          error = 1;
        break;
      case 4: // jump then block
      case 6: // jump else block
        if (!found)
          error = 1;
        break;
      case 5:
        // check else token
        if (tok != TK_ELSE)
          error = 1;
        break;
      case 7: // check fi token
        if (tok != TK_FI)
          error = 1;
        break;
    }

    if (error)
    {
      printf_P(PSTR("Error in IF statement, pos: %d\n"), b.pos);
      return 1;
    }
    if (state != 4 && state != 6)
      b.pos += b.len;
    else
      b.pos = pos;
    if (state == 4 && found != 2)
      state = 7;
    else
      state++;
  }

  a->len = b.pos - a->pos;

  return 0;
}

uint8_t eval_if(block a, char *env[])
{
  uint8_t found = 0;
  uint8_t cond = 0;
  uint8_t error = 0;
  uint8_t state = 0;
  size_t limit;

  while (state < 7)
  {
    if (state != 4 && state != 6)
      get_string(&a);
    else
      found = find_else_fi(a, &limit);

    switch (state)
    {
      case 0: // jump the if
        break;
      case 1: // get the condition
        extend_to_eol(&a);
        cond = eval(a.p + a.pos, a.len, env);
        break;
      case 2: // jump the new line/semi colon character
        break;
      case 3: // now we need the then
        break;
      case 4: // look for the "else" or "fi" token and save the block limit
        break;
      case 5: // process the "then" part
        if (!cond)
          error = eval(a.p + a.pos, limit - a.pos, env);
        // goto the end of the "then" section
        a.pos = limit;
        if (found != 2)
          state++;
        break;
      case 6: // if we have an "else" part
        if (cond)
          error = eval(a.p + a.pos, limit - a.pos, env);
        break;
    }

    if (state != 4)
      a.pos += a.len;
    state++;
  }

  return error;
}

// get command from input
uint8_t getcmd(char buff[])
{
  uint8_t buffp = 0;

  do
  {
    // read input
    buff[buffp]=getchar();

    // treat special charaters
    switch(buff[buffp])
    {
      // backspace
      case 0x08:
        // TODO: go back one position in screen
        buff[buffp]=0;
        if (buffp > 0)
          buffp--;
        break;
      // horizontal tab
      case 0x09:
        // TODO
        break;
      // Rest of characters
      default:
        // increment index
        buffp++;
    }

    // Check the buffer used
    if (buffp == ARGMAX)
    {
      puts_P(PSTR("ERROR: Line too long, " STR(ARGMAX) " limit reached\n"));
      return false;
    }
  }
  while (buff[buffp-1] != '\n' && buff[buffp-1] != '\r');

  // end the command with a \0
  buff[buffp] = 0;

  return buffp;
}

int8_t main_sh(char *argv[], char *env[])
{
  // input line buffer
  char line[ARGMAX];
  size_t len;
  uint8_t exit_flag = 1;
  int8_t result;
  uint8_t delete_at_exit = 0;

  if (argv[1] != NULL)
  {
    // TODO
    printf_P(PSTR("TODO RUN: %s\n"), argv[1]);
    return 0;
  }

  // add a environment if it doesn't exist
  if (env == NULL)
  {
    // alloc a new env
    if ((env = malloc(ENV_MAX * sizeof(char *))) == NULL)
      return 1; // cannot allocate memory
    // clean env
    memset(env, 0, ENV_MAX * sizeof(char *));
    delete_at_exit = 1;
  }

  // Set Current Working Directory if doesn't exists
  if (env_get(env, "CWD") == NULL)
    env_add(env, "CWD", HOME);

  // Loop until we exit cmd
  do
  {
    // show prompt
    printf_P(PSTR("%s# "), env_get(env, "CWD"));

    // Get command
    len = getcmd(line);
    if (!len)
      continue;
    result = eval(line, len, env);
    printf_P(PSTR("Result: %d\n"), result);

  // TODO: Implement a decent exit mechanism
  } while(exit_flag);

  if (delete_at_exit)
    free(env);

  return 0;
}


