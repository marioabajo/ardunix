#include "sh.h"
#include "kernel.h"
#include "support.h"
#include <stdio.h>
#include <string.h>

// Functions
char strncmp_fast(char *t1, char *t2, size_t n)
{
	size_t i=0;

	while (i < n)
	{
		if (t1[i] != t2[i])
			return FALSE;
		if (t1[i] == '\0' || t2[i] == '\0')
			return FALSE;

		i++;
	}
	return TRUE;
}

char is_char(unsigned char c)
{
	if (c >= 'a' && c <='z')
		return 1;
	if (c >= 'A' && c <='Z')
		return 2;
	return 0;
}

char is_number(unsigned char c)
{
	if (c >= '0' && c<='9')
		return TRUE;
	return FALSE;
}

char is_var(char *t, size_t n)
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

token str_to_token(char *t, size_t n)
{
	// First, clasify token by the length of it, it's fast
	switch (n)
	{
		case 1:
			if (strncmp_fast("!", t, 1))
				return TK_EXCL;
			else if (strncmp_fast("{", t, 1))
				return TK_BROP;
			else if (strncmp_fast("}", t, 1))
				return TK_BRCL;
			else if (strncmp_fast(";", t, 1))
				return TK_SC;
			else if (strncmp_fast("\n", t, 1))
				return TK_NL;
      else if (strncmp_fast("\r", t, 1))
        return TK_CR;
			else if (strncmp_fast("(", t, 1))
				return TK_PO;
			else if (strncmp_fast(")", t, 1))
				return TK_PC;
			break;
		case 2:
			if (strncmp_fast("do", t, 2))
				return TK_DO;
			else if (strncmp_fast("fi", t, 2))
				return TK_FI;
			else if (strncmp_fast("if", t, 2))
				return TK_IF;
			else if (strncmp_fast("in", t, 2))
				return TK_IN;
			break;
		case 3:
			if (strncmp_fast("for", t, 3))
				return TK_FOR;
			break;
		case 4:
			if (strncmp_fast("case", t, 4))
				return TK_CASE;
			else if (strncmp_fast("done", t, 4))
				return TK_DONE;
			else if (strncmp_fast("elif", t, 4))
				return TK_ELIF;
			else if (strncmp_fast("else", t, 4))
				return TK_ELSE;
			else if (strncmp_fast("esac", t, 4))
				return TK_ESAC;
			else if (strncmp_fast("then", t, 4))
				return TK_THEN;
			break;
		case 5:
			if (strncmp_fast("until", t, 5))
				return TK_UNTIL;
			else if (strncmp_fast("while", t, 5))
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

	// remove begining spaces and new lines
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
				return cmd;
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
		//printf("DEBUG: %.*s\n", (int)len2, aux);
		aux = get_string(aux, len, limit);
	}
	*len = limit2 - *limit;
}

char * trim_NL(char *cmd, size_t *len, size_t *limit)
{
	// TODO: remove new lines, carrier returns and ";" simbols
	return get_string(cmd, len, limit);
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
	//if (!found)
	//	printf("Syntax error, nor else neither fi found\n");

	return found;
}

bool trim_spaces(char *str, size_t len, size_t *pos)
{
    while (*pos < len && str[*pos] == ' ')
      *pos = *pos + 1;

    return (*pos == len);
}

uint8_t str_to_argv(char *src, char *dst, size_t limit, char **argv, uint8_t *argc)
{
  size_t i = 0, j = 0, len = 0;
  token tok;

  *argc = 0;
  
  while ((src = get_string(src, &len, &limit)) != NULL)
  {
    // Check if the string is an end of line of any type
    tok = str_to_token(src, len);
    if (tok == TK_NL || tok == TK_CR || tok == TK_SC)
      break;

    memcpy(dst + j, src, len);
    argv[*argc] = dst + j;
    j += len;
    dst[j++] = 0;
    //DEBUG: printf("D: j:%d len:%d src:%x argc:%d (%s)\n", j, len, src, *argc, argv[*argc]);
    *argc = *argc + 1;
  }
  for (i = *argc; i < NCARGS; i++)
    argv[i] = NULL;

  return true;
}

unsigned char eval(char *cmd, size_t limit)
{
	token tok;
	unsigned char error = 0, cond;
	unsigned char found;
  char str[ARGMAX];
  char *argv[NCARGS];
  uint8_t argc = 0;
	size_t len = 0;
	size_t len2;
	size_t limit2;

	cmd = get_string(cmd, &len, &limit);
	while (cmd != NULL)
	{
		tok = str_to_token(cmd, len);
		//printf("+ %.*s  (token: %d)\n",(int)len, cmd, tok);
		switch (tok)
		{
			case TK_NL:
      case TK_CR:
				break;

			// a simple command followed by parameters until '\n' or ';'
			case FALSE:
				extend_string(cmd, &len, &limit);
        str_to_argv(cmd, str, len, argv, &argc);
        error = execve(argv[0], (const char **) &argv[1], NULL);
				break;

			// If structure
			case TK_IF:
				//printf("IF: ");
				// get the condition
				cmd = get_string(cmd, &len, &limit);
				if (cmd == NULL)
					return 1;
				extend_string(cmd, &len, &limit);
				cond = eval(cmd, len);

				// now we need the then
				cmd = trim_NL(cmd, &len, &limit);
				tok = str_to_token(cmd, len);
				if (tok != TK_THEN)
				{
					printf_P(PSTR("Syntax error, then expected\n"));
					return 1;
				}

				// look for the "else" or "fi" token
				len2 = len;
				limit2 = limit;
				cmd = get_string(cmd, &len2, &limit2);
				found = find_else_fi(cmd, &len2, &limit2);
				// if we don't find any "else" neither "fi", then we have a syntax error
				if (!found)
				{
					printf_P(PSTR("Syntax error, nor else neither fi found\n"));
					return 1;
				}

				// process the "then" part
				if (!cond)
					error = eval(cmd, limit - limit2 - len2);

				// goto the end of the "then" section
				cmd += limit - limit2;
				limit = limit2 + len2;

				// if we have an "else" part
				if (found == 1)
				{
					// find fi
					len2 = 0;
					limit2 = limit;
					found = find_else_fi(cmd, &len2, &limit2);
					if (found != 2)
					{
						printf_P(PSTR("Syntax error, expected fi not found\n"));
						return 1;
					}
					if (cond)
						error = eval(cmd, limit - limit2 - len2);
					// weird adjust
					cmd += len2;
				}

				// goto fi
				cmd += limit - limit2 - len2;
				limit = limit2 + len2;
				len = 0;

				break;

			// for structure
			case TK_FOR:
				// TODO
				break;

			default:
				printf_P(PSTR("Syntax error: %s\n"), cmd);
				return 1;
		}
		cmd = get_string(cmd, &len, &limit);
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

/*uint8_t varexpansion(char line[], struct dict_list **env)
{
  struct dict_list e;
  uint8_t pos = 0, len, varlen, j;
  int8_t extralen;
  char *var, *i;

  len = strlen(line);

  while (pos < len)
  {
    switch (line[pos])
    {
      // Invalidate function of next character
      case '\\':
        pos++;
        break;
      // Parameter expansion
      case '$':
        //TODO: check for length
        pos++;
        // It's a variable name?
        if ((line[pos] >= 'a' && line[pos] <= 'z') ||
            (line[pos] >= 'A' && line[pos] <= 'Z'))
        {
          // calculate the end of the parameter name
          i = line + pos;
          while ((*i >= 'a' && *i <= 'z') ||
                 (*i >= 'A' && *i <= 'Z') ||
                 (*i >= '0' && *i <= '9') ||
                 (*i == '_'))
            i++;
          // now i points to the end of the parameter, this minus pos equals
          // to the lenght of the parameter name
          extralen = i - (line + pos);
          varlen = 0;
          // get the environment variable value
          if ((var = env_nget(*env, line + pos, extralen)) != NULL)
          {
            // get the lenght of the parameter value
            varlen = strlen(var);
            // calculate the difference betwen the length of the variable 
            // name and the length of the data stored in it
            extralen -= varlen - 1;
          }
          extralen = -extralen;
          // Now, maybe the value of the parameter is bigger that the parameter
          // name, so, to put it in the same line we have to shift the line
          // the number of characters in extralen
          // First, check the limits
          if (len + extralen > ARGMAX)
          {
            fprintf_P(stderr,PSTR("ERROR: Line too long, %d limit reached\n"), ARGMAX);
            return 0;
          }
          // adjust the length of the line (now is bigger)
          len += extralen;
          // shift the string the value of extralen (negative for right shift)
          j = len;
          while (j >= pos)
          {
            //printf("DEBUG: ini env = %x\n", *env);
            line[j] = line[j - extralen];
            //printf("DEBUG: fin env = %x\n", *env);
            j--;
          }
          // put value of the environment variable
          strncpy(line + pos -1, var, varlen);
          pos += varlen - 2;
        }

        break;
    }

    pos++;
  }
  return 1;
}*/

uint8_t main_sh(uint8_t argc, char *argv[], char *env[])
{
  // input line buffer
  char line[ARGMAX];
  // array with pointers to arguments inside line buffer
  //char *args[NCARGS];
  //uint8_t argsnum=0;
  size_t len;
  uint8_t exit_flag=1;
  uint8_t result;

  do
  {
    // show prompt
    printf_P(PSTR("# "));

    // Get command
    len = getcmd(line);
    if (!len)
      continue;
    result = eval(line, len);
    printf_P(PSTR("Result: %d\n"), result);

  // TODO: Implement a decent exit mechanism
  } while(exit_flag);

  return 0;
}


