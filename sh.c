#include "sh.h"
#include "kernel.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned char eval_if(char **cmd, size_t *len, size_t *limit, char *env[]);
unsigned char eval_command(char **cmd, size_t *len, size_t *limit, char *env[]);

// Functions
uint8_t is_char(unsigned char c)
{
	if (c >= 'a' && c <='z')
		return 1;
	if (c >= 'A' && c <='Z')
		return 2;
	return 0;
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

token str_to_token(char *t, size_t n)
{
	// First, clasify token by the length of it, it's fast
	switch (n)
	{
		case 1:
			if (*t == '!')
				return TK_EXCL;
			else if (*t == '{')
				return TK_BROP;
			else if (*t == '}')
				return TK_BRCL;
			else if (*t == ';')
				return TK_SC;
			else if (*t == '\n')
				return TK_NL;
      else if (*t == '\r')
        return TK_CR;
			else if (*t == '(')
				return TK_PO;
			else if (*t == ')')
				return TK_PC;
			break;
    case 2:
      if (strncmp_P(t, PSTR("do"), 2) == 0)
        return TK_DO;
      else if (strncmp_P(t, PSTR("fi"), 2) == 0)
        return TK_FI;
      else if (strncmp_P(t, PSTR("if"), 2) == 0)
        return TK_IF;
      else if (strncmp_P(t, PSTR("in"), 2) == 0)
        return TK_IN;
      break;
    case 3:
      if (strncmp_P(t, PSTR("for"), 3) == 0)
        return TK_FOR;
      break;
    case 4:
      if (strncmp_P(t, PSTR("case"), 4) == 0)
        return TK_CASE;
      else if (strncmp_P(t, PSTR("done"), 4) == 0)
        return TK_DONE;
      else if (strncmp_P(t, PSTR("elif"), 4) == 0)
        return TK_ELIF;
      else if (strncmp_P(t, PSTR("else"), 4) == 0)
        return TK_ELSE;
      else if (strncmp_P(t, PSTR("esac"), 4) == 0)
        return TK_ESAC;
      else if (strncmp_P(t, PSTR("then"), 4) == 0)
        return TK_THEN;
      break;
    case 5:
      if (strncmp_P(t, PSTR("until"), 5) == 0)
        return TK_UNTIL;
      else if (strncmp_P(t, PSTR("while"), 5) == 0)
        return TK_WHILE;
			break;
	}

	// TODO: Now check for variables, commands, etc
	// if we found an '=' is an asigment
	// else it's a name (function, varname, command, etc...)
	// variable substitution should be done earlier....

	return FALSE;
}

char * get_string(char *old_cmd, size_t *len, size_t *limit)
{
	char *cmd;
	uint8_t par = 0;
	uint8_t _exit = 0;

	cmd = old_cmd + *len;
	*len = 0;

	// remove begining spaces
	while (*limit > 0 && cmd[0] == ' ')
	{
		cmd++;
		(*limit)--;
	}

	// delimit the string, considering the quotes
	while (! _exit)
	{
		// check the limits
		if (*limit == 0)
		{
			if (*len > 0)
				break;
			return NULL;
		}

		// take some chars specially
		switch (cmd[*len])
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
		if ((cmd[*len] == ' ' || cmd[*len] == ';' || cmd[*len] == '\n' || cmd[*len] == '\r') && par == 0)
		{
			if (*len == 0)
				_exit = 1;
			else
				break;
		}
		(*limit)--;
		(*len)++;
	}
	return cmd;
}

void extend_string(char *cmd, size_t *len, size_t *limit)
// Extend a string until end of line or ;
{
	token tok;
	char *aux;
	size_t limit2 = *limit + *len;

	aux = get_string(cmd, len, limit);
	while (aux != NULL)
	{
		tok = str_to_token(aux, *len);
		if (tok == TK_NL || tok == TK_CR || tok == TK_SC)
			break;
		//printf("DEBUG: %.*s\n", (int)*len, aux);
		aux = get_string(aux, len, limit);
	}
	*len = limit2 - *limit;
}

unsigned char find_else_fi(char *cmd, size_t *len, size_t *limit)
{
	unsigned char count = 0;
	unsigned char found = 0;

	while (cmd != NULL && found == 0)
	{
		//printf("DEBUG: %.*s\n", (int)*len, cmd);
		switch (str_to_token(cmd, *len))
		{
			case TK_IF:  // found an anidated if, look for the fi
				count++;
			case TK_ELSE:
				if (!count)
					found = 1;
				break;
			case TK_FI:
				if (count)
					count--;
				else
					found = 2;
				break;
		}
		if (!found)
			cmd = get_string(cmd, len, limit);
	}

	return found;
}

uint8_t str_to_argv(char *src, char *dst, size_t limit, char **argv, char *env[])
{
  size_t i = 0, j = 0, len = 0;
  token tok;
  
  while ((src = get_string(src, &len, &limit)) != NULL)
  {
    // Check if the string is an end of line of any type
    tok = str_to_token(src, len);
    if (tok == TK_NL || tok == TK_CR || tok == TK_SC)
      break;

    // TODO: check for an env variable here

    memcpy(dst + j, src, len);
    argv[i] = dst + j;
    j += len;
    dst[j] = 0;
    i++;
    j++;
    if (i >= NCARGS - 1) // save the last arg for the NULL termination
      return false;
    //DEBUG: printf("D: j:%d len:%d src:%x argc:%d (%s)\n", j, len, src, *argc, argv[*argc]);
  }
  for (; i < NCARGS; i++)
    argv[i] = NULL;

  return true;
}

unsigned char eval(char *cmd, size_t limit, char *env[])
{
	token tok;
	unsigned char error = 0;
	size_t len = 0;

  do
	{
    if ((cmd = get_string(cmd, &len, &limit)) == NULL)
      break;
		tok = str_to_token(cmd, len);

		switch (tok)
		{
      // ignore new lines, carrier returns and semicolons
      case TK_CR:
      case TK_NL:
      case TK_SC:
        break;

			// a simple command followed by parameters until '\n' or ';'
			case FALSE:
        error = eval_command(&cmd, &len, &limit, env);
        break;
      
      // If structure
      case TK_IF:
        error = eval_if(&cmd, &len, &limit, env);
				break;

			// for structure
			case TK_FOR:
				// TODO
				break;

			default:
				printf_P(PSTR("Syntax error: %s\n"), cmd);
				return 1;
		}
	} while (true);

	return error;
}

unsigned char eval_command(char **cmd, size_t *len, size_t *limit, char *env[])
{
  unsigned char error = 0;
  char str[ARGMAX];
  char *argv[NCARGS];
  uint8_t pos;
  
  // Check if is a variable assigment
  if ((pos = is_var_assign(*cmd, *len)) == 1)
  {
    if (!is_varname_ok(*cmd, pos - 1))
      return 1;
    env_add_str(env, *cmd, *len, pos);
    return 0;
  }

  //TODO: check if there is a env var here

  extend_string(*cmd, len, limit);
  // Discard empty commands
  if (*cmd[0] == 0)
    return error;
  str_to_argv(*cmd, str, *len, argv, env);
  error = execve((const char **) argv, env);
  // TODO: add variable $? with the result of the last command
        
  return error;  
}

unsigned char eval_if(char **cmd, size_t *len, size_t *limit, char *env[])
{
  size_t len2;
  size_t limit2;
  unsigned char found;
  unsigned char cond;
  unsigned char error = 0;
  token tok;

  // get the condition
  *cmd = get_string(*cmd, len, limit);
  if (*cmd == NULL)
    return 1;
  extend_string(*cmd, len, limit);
  cond = eval(*cmd, *len, env);

  // now we need the then
  *cmd = get_string(*cmd, len, limit);
  tok = str_to_token(*cmd, *len);
  if (tok != TK_THEN)
  {
    printf_P(PSTR("Error, then expected\n"));
    return 1;
  }

  // look for the "else" or "fi" token
  len2 = *len;
  limit2 = *limit;
  *cmd = get_string(*cmd, &len2, &limit2);
  found = find_else_fi(*cmd, &len2, &limit2);
  // if we don't find any "else" neither "fi", then we have a syntax error
  if (!found)
  {
    /*printf_P(PSTR("Error, else or fi expected\n"));
    return 1;*/
    goto exitfi;
  }

  // process the "then" part
  if (!cond)
    error = eval(*cmd, *limit - limit2 - len2, env);

  // goto the end of the "then" section
  *cmd += *limit - limit2;
  *limit = limit2 + len2;

  // if we have an "else" part
  if (found == 1)
  {
    // find fi
    len2 = 0;
    limit2 = *limit;
    found = find_else_fi(*cmd, &len2, &limit2);
    if (found != 2)
    {
exitfi:
      printf_P(PSTR("Error, fi expected\n"));
      return 1;
    }
    if (cond)
      error = eval(*cmd, *limit - limit2 - len2, env);
    // weird adjust
    *cmd += len2;
  }

  // goto fi
  *cmd += *limit - limit2 - len2;
  *limit = limit2 + len2 - *len;
  *len = 0;

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

  do
  {
    // show prompt
    printf_P(PSTR("# "));

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


