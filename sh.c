#include "sh.h"
#include "kernel.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char PROGMEM c[] = "!{};\n\r()dofiifinforcasedoneelifelseesacthenuntilwhile";
const token PROGMEM ta[] = {TK_EXCL, TK_BROP, TK_BRCL, TK_SC, TK_NL, TK_CR, TK_PO, TK_PC, \
                            TK_DO, TK_FI, TK_IF, TK_IN, TK_FOR, TK_CASE, TK_DONE, TK_ELIF, \
                            TK_ELSE, TK_ESAC, TK_THEN, TK_UNTIL, TK_WHILE};
// values grouped in three values: start, end, start_in_token_array
const uint8_t PROGMEM d[] = {0, 8, 0, 8, 16, 8, 16, 19, 12, 19, 43, 13, 43, 53, 19}; 
/*  token length:            \_ 1 _/  \__ 2 _/  \___ 3 __/  \___ 4 __/  \___ 5 __/
    values:                  |  |  \-->start in ta array
                             |  \--> limit in c array
                             \--> start in c array 
*/


#define END_OF_BLOCK(a) (a.pos == a.limit)
#define END_OF_PBLOCK(a) (a->pos == a->limit)

uint8_t syntax_if(block *a);
uint8_t eval_if(block a, char *env[]);
uint8_t eval_command(block a, char *env[]);

// Functions
uint8_t is_char(unsigned char c)
{
	if (c >= 'a' && c <='z')
		return 1;
	if (c >= 'A' && c <='Z')
		return 2;
	return FALSE;
}

uint8_t is_number(unsigned char c)
{
	if (c >= '0' && c<='9')
		return TRUE;
	return FALSE;
}

uint8_t is_varname_ok(char *t, size_t n)
{
	size_t i=0;

	if (! (is_char(t[i]) || t[i] == '_'))
		return FALSE;

	while (i < n && t[i] != '\0')
	{
		if (! (is_char(t[i]) || is_number(t[i]) || t[i] == '_'))
			return FALSE;
		i++;
	}
	return TRUE;
}

uint8_t is_var(char *t, size_t n)
{
  if (n < 2)
    return FALSE;
    
  if (t[0] == '$' && is_varname_ok(&t[1], n - 1))
    return TRUE;

  return FALSE;
}

uint8_t is_var_assign(char *t, size_t n)
{
  uint8_t i = 1;
  
  // find '=' character position
  while (i < n)
  {
    if (t[i] == '=')
      break;
    i++;
  }

  if (i < n)
    return i;
  return FALSE;
}

token str_to_token(block a)
{
  char *t = a.p + a.pos;
  uint8_t i, start, limit, tpos;

  if (a.len > 5 || a.len == 0)
    return FALSE;

  start = pgm_read_byte(&d[(a.len * 3) - 3]);
  limit = pgm_read_byte(&d[(a.len * 3) - 2]);
  tpos  = pgm_read_byte(&d[(a.len * 3) - 1]);
  for (i = start; i < limit; i += a.len)
  {
    if (strncmp_P(t, &c[i], a.len) == 0)
      return pgm_read_byte(&ta[tpos]);
    tpos++;
  }
  return FALSE;
}

uint8_t get_string(block *a)
{
  char *s;
  size_t pos;
  uint8_t par = 0;
  uint8_t _exit = 0;

  s = a->p;
  a->len = 0;

  // remove begining spaces
  while (!END_OF_PBLOCK(a) && s[a->pos] == ' ')
    a->pos++;

  pos = a->pos;

  // delimit the string, considering the quotes
  while (! _exit)
  {
    // check the limits
    if (END_OF_PBLOCK(a))
    {
      // if we already have something, exit loop
      if (a->len > 0)
        break;
      // we reached the end of the block without having found nothing, exit with error
      return FALSE;
    }

    // take some chars specially
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
    }
    // if we found any of this chars, then finish the string
    if ((s[pos] == ' ' || s[pos] == ';' || s[pos] == '\n' || s[pos] == '\r') && par == 0)
    {
      if (a->len == 0)
        _exit = 1;
      else
        break;
    }
    a->len++;
    pos++;
  }
  return TRUE;

}

void extend_to_eol(block *a)
// Extend a string to \n or ; characters found
{
  token tok;
  block b;

  b.p = a->p;
  b.pos = a->pos + a->len;
  b.len = 0;
  b.limit = a->limit;

  do
  {
    get_string(&b);
    tok = str_to_token(b);
    if (tok == TK_NL || tok == TK_CR || tok == TK_SC)
      break;
    b.pos += b.len;
  } while (!END_OF_BLOCK(b));
  a->len = b.pos - a->pos;
}

uint8_t str_to_argv(block a, char *dst, char *argv[], char *env[])
{
  size_t i = 0, j = 0, ret = 0;
  char p;

  while (true)
  {
    if (i >= ARGMAX)
      ret = 1; // line too long
    if (j >= NCARGS)
      ret = 2; // too many args
    if (ret || END_OF_BLOCK(a))
      break;

    p = a.p[a.pos];
    if (p == ' ')
    {
      dst[i] = 0;
      a.len = 0;
    }
    else
    {
      if (a.len == 0)
      {
        argv[j] = dst + i;
        j++;
      }
      dst[i] = p;
      a.len++;
    }
    i++;
    a.pos++;
  }

  if (a.len > 0)
    dst[i] = 0;

  for (; j < NCARGS; j++)
    argv[j] = NULL;

  return ret;

  return true;
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
      case FALSE:
        extend_to_eol(&a);
        // build the command block
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
  unsigned char error = 0;
  char str[ARGMAX];
  char *argv[NCARGS];
  uint8_t pos;
  
  // Check if is a variable assigment
  if ((pos = is_var_assign(a.p, a.limit)) != false)
  {
    if (!is_varname_ok(a.p, pos - 1))
      return 1;
    //return env_add_str(env, *cmd, *len, pos);
    return env_add_l(env, a.p, pos, a.p + pos + 1, a.limit - pos - 1);
  }

  //TODO: check if there is a env var here

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
      fprintf_P(stderr,PSTR("ERROR: Line too long, %d limit reached\n"), ARGMAX);
      return false;
    }
  }
  while (buff[buffp-1] != '\n' && buff[buffp-1] != '\r');

  // end the command with a \0
  buff[buffp] = 0;

  return buffp;
}

uint8_t main_sh(char *argv[], char *env[])
{
  // input line buffer
  char line[ARGMAX];
  size_t len;
  uint8_t exit_flag=1;
  uint8_t result, i;

  // add a environment if it doesn't exist
  if (env == NULL)
  {
    // alloc a new env
    if ((env = malloc(ENV_MAX * sizeof(char *))) == NULL)
      return 1; // cannot allocate memory
    // clean env
    for (i=0; i< ENV_MAX; i++)
      env[i] = NULL;
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

  return 0;
}


