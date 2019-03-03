#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "sh.h"
#include "kernel.h"
#include "env.h"
#include "fs.h"

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
#define TK_CD    22 // cd
#define TK_EXIT  23 // exit

#define END_OF_BLOCK(a) (a.pos == a.limit)
#define END_OF_PBLOCK(a) (a->pos == a->limit)

// Arrays with the shell tokens, values and other info

const char PROGMEM c[] = "!{};\n\r()dofiifincdforcasedoneelifelseesacthenexituntilwhile";
const uint8_t PROGMEM ta[] = {TK_EXCL, TK_BROP, TK_BRCL, TK_SC, TK_NL, TK_CR, \
                              TK_PO, TK_PC, TK_DO, TK_FI, TK_IF, TK_IN, TK_CD, \
                              TK_FOR, TK_CASE, TK_DONE, TK_ELIF, TK_ELSE, \
                              TK_ESAC, TK_THEN, TK_EXIT, TK_UNTIL, TK_WHILE};
// values grouped in three values: start, end, start_in_token_array
const uint8_t PROGMEM d[] = {0, 8, 0, 8, 18, 8, 18, 21, 13, 21, 49, 14, 49, 59, 21};
/*  token string length:     \_ 1 _/  \__ 2 _/  \___ 3 __/  \___ 4 __/  \___ 5 __/
    values:                  |  |  \-->start in ta array
                             |  \--> end in c array
                             \--> start in c array 
*/

// Private function prototypes
static int8_t eval(char *cmd, char *env[], size_t limit, uint8_t *quit);

// Functions

static uint8_t is_alpha(char c)
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

static uint8_t is_digit(char c)
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

static uint8_t tok_is_separator(uint8_t t)
{
	if (t == TK_NL || t == TK_CR || t == TK_SC)
		return true;
	return false;
}

static uint8_t is_separator(char c)
{
	if (c == '\n' || c == '\r' || c == ';')
		return true;
	return false;
}


static uint8_t is_varname_ok(char *t, size_t n)
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
		if (! (is_alpha(t[i]) || (is_digit(t[i]) && i) || t[i] == '_'))
			return false;
		i++;
	}
	return true;
}

static uint8_t is_var(char *t, size_t n)
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

static uint8_t is_var_assign(char *t, size_t n)
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

static size_t get_string(char *in, char **out)
/* Important function that recognizes, inside a block (a string), the next word 
 * (separated by spaces) or substring (taking into account "" or '')
 * 
 * Input: block
 * Returns: block with modified pos and len fields
 *          0 -> could not select any more words/substring
 *          1 -> word/substring selected
 */
{
	size_t i=0, len=0;
	uint8_t par=0, _exit=1;

	// remove begining spaces
	while (in[i] != 0 && in[i] == ' ')
		i++;

	*out = in + i;

	// delimit the string, considering the quotes
	while (_exit)
	{
		// take quotes into account, when we found one, continue until 
		// we find the pair
		switch (in[i])
		{
			case 0:
				_exit = 0;
				continue;
				break;
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
					if (len == 0)
						len++;
					// exit loop
					_exit = 0;
					continue;
				}
		}
		len++;
		i++;
	}

	return len;
}

static size_t extend_to_eol(char *str, size_t len)
/* Given a block with a substring selected, extend the selection until an end of
 * line is found.
 * 
 * Input: block
 * Returns: Modify block length
 */
{
	size_t i = len;
	uint8_t _exit = 1;

	while (_exit && str[i])
	{
		if (is_separator(str[i]))
		{
			_exit = 0;
			continue;
		}
		i++;
	}
	return i;
}

static uint8_t get_token(char *str, size_t len)
/* Given a block (a substring inside a string), return the token value or 0
 * if it's not a valid token
 * 
 * Input: a block structure
 * Return: 0 -> invalid token
 *         >0 -> token number
 */
{
	uint8_t i, start, limit, tpos;

	// strings with no lenght or greater than 5 cannot be valid tokens
	if (len > 5 || len == 0)
		return false;

	start = pgm_read_byte(&d[(len * 3) - 3]);
	limit = pgm_read_byte(&d[(len * 3) - 2]);
	tpos  = pgm_read_byte(&d[(len * 3) - 1]);

	// compare the string with every token of the same lenght
	for (i = start; i < limit; i += len)
	{
		// if found, return token
		if (strncmp_P((const char *)str, &c[i], len) == 0)
			return pgm_read_byte((void *) &ta[tpos]);
		tpos++;
	}
	
	return false;
}

static uint8_t str_to_argv(char *input, size_t limit, char *argstr, char *argv[], char *env[])
/* Given a block (string), an allocated string with size ARGMAX, an array of arg values
 * of size NCARGS and an environment array; split the block string, copy the args to
 * dst and fill the argv array and if an argument is a variable, substitute it
 *
 * Input: block, string, argv array and env array
 * Returns: number of args processed
 */
{

	size_t i = 0, len;
	uint8_t j = 0;
	char *str, *save;
	
	save = input;

	// first, separate args and copy the string
	while ((len = get_string(input, &str)) > 0 && j < NCARGS -1 && input <= save + limit )
	{
		if (is_separator(str[0]))
		{
			// advance input pointer
			input = str + len;
			continue;
		}
		// copy arg to dst
		memcpy(argstr, str, len);
		// point arg to dst
		argv[j++] = argstr;
		// update dst to the end of the arg
		argstr += len;
		// end the arg string with a 0 (that why we copy it to dst
		//, because the block may not be writable)
		*(argstr++) = 0;
		// advance input pointer
		input = str + len;
	}

	// check if any arg is a variable and substitute
	// TODO: Actually we only tread arguments that contains only the variable name
	//       not a mix of strings and variables in the same arg or inside quotes
	for (i = 0; i<j; i++)
	{
		len = strlen(argv[i]);
		if (is_var(argv[i], len))
			argv[i] = env_get_l(env, argv[i] + 1, len - 1);
	}

	// Put the last arg to NULL to end the list
	argv[j] = NULL;

	return j;
}

//static uint8_t find_token(char *cmd, size_t *pos, uint8_t target)
static uint8_t find_tokens(char *cmd, size_t *pos, uint8_t *prev, uint8_t nargs, ...)
{
	size_t len;
	char *str, *save;
	uint8_t tok = 0, i, _exit = 1, t, aux = 0;
	int8_t do_c = 1, if_c = 1; //br_c = 1, ca_c = 1, pa_c = 1;
	va_list target;
	
	save = cmd;

	while ((len = get_string(cmd, &str)) > 0 && _exit)
	{
		// DEBUG
		//printf("DEBUG: find_token: do:%d if:%d %d %.*s\n", do_c, if_c, len, len, str);
		if (prev)
			*prev = tok;
		tok = get_token(str, len);

		// thread tokens that are part of syntax blocks
		switch(tok)
		{
			case TK_DO:
				// increment DO/DONE block counter
				do_c++;
				break;
			case TK_DONE:
				// decrement DO/DONE block counter
				do_c--;
				break;
			case TK_IF:
				if_c++;
				// increment IF/FI block counter
				break;
			case TK_FI:
				if_c--;
				// decrement IF/FI block counter
				break;
			/*case TK_BROP:
				// increment Brackets block counter
				break;
			case TK_BRCL:
				// decrement Brackets block counter
				break;
			case TK_CASE:
				// increment CASE/ESAC block counter
				break;
			case TK_ESAC:
				// decrement CASE/ESAC block counter
				break;
			case TK_POP:
				// increment Parenthesis block counter
				break;
			case TK_PCL:
				// decrement Parenthesis block counter
				break;*/
		}

		// Check if the token is found and is not part of another syntax
		// block.
		va_start(target, nargs);
		for (i=0; i<nargs; i++)
		{
			t = va_arg(target, int);
			if (t == tok)
			{
				// adjust the pointer to set it up at the
				// beginning of the token
				aux = len;
				
				if (do_c + if_c <= 2)
				{
					_exit = 0;
					break;
				}
			}
		}
		va_end(target);
		/*if (target == tok)
		{
			if (do_c + if_c < 2)
				break;*/
			/*if (tok == TK_DO || tok == TK_DONE)
			{
				if (!do_c)
					break;
			}
			else if (tok == TK_IF || tok == TK_FI || TK_ELIF \
				|| TK_ELSE)
			{
				if (!if_c)
					break;
			}
			else
				break;*/
		//}

		// increment to pointer to the next token to be read
		cmd = str + len;
	}
	// Save the position of the found token (beginning)
	*pos = cmd - save - aux;
	
	return tok;
}

static uint8_t find_token(char *cmd, size_t *pos, uint8_t *prev, uint8_t target)
{
	return find_tokens(cmd, pos, prev, 1, target);
}

static uint8_t syntax_if(char *cmd)
{
	uint8_t tok = 0, prev_tok;
	uint8_t found = 0;
	uint8_t state = 0;
	uint8_t error = 0;
	size_t len;
	char *str;

	while (state < 8)
	{
		len = get_string(cmd, &str);
		tok = get_token(str, len);

		switch (state)
		{
			case 0: // jump the if
				break;
			case 1: // condition
				len = extend_to_eol(str, len);
				break;
			case 2: // check new line or separator
				if (! tok_is_separator(tok))
					error = 1;
				break;
			case 3: // check then token
				if (tok != TK_THEN)
					error = 1;
				break;
			case 4: // jump "then" block
				found = find_tokens(str, &len, &prev_tok, 3, TK_ELIF, TK_ELSE, TK_FI);
				if (! tok_is_separator(prev_tok))
					error = 1;
				if (found == TK_FI)
					state = 7;
				break;
			case 5:
				// check else token
				if (tok != TK_ELSE)
					error = 1;
				break;
			case 6: // jump else block
				if (find_token(str, &len, &prev_tok, TK_FI) != TK_FI)
					error = 1;
				// check new line or separator
				if (! tok_is_separator(prev_tok))
					error = 1;
				break;
			case 7: // check fi token
				if (tok != TK_FI)
					error = 1;
				break;
		}
		// DEBUG
		//printf("DEBUG: %d tok: %d (%.*s)\n", state, tok, len, str);

		if (error)
		{
			printf_P(PSTR("Error in IF statement, pos: %s\n"), cmd);
			return 1;
		}
		cmd = str + len;
		state++;
	}

	return 0;
}

static int8_t eval_if(char *cmd, size_t *len, char *env[])
{
	uint8_t found = 0;
	uint8_t cond = 0;
	int8_t error = 0;
	uint8_t state = 0;
	uint8_t quit = 0;
	size_t i = *len;
	char *str, *save;
	
	save = cmd;

	while (state < 8 && ! quit)
	{
		i = get_string(cmd, &str);

		switch (state)
		{
			case 0: // jump the if
				break;
			case 1: // get the condition
				i = extend_to_eol(str, i);
				cond = eval(str, env, i, &quit);
				//printf("DEBUG eval_if: cond=%d\n",cond);
				break;
			case 2: // jump the new line/semi colon character
			case 3: // now we need the then
				break;
			case 4: // process the "then" part
				found = find_tokens(str, &i, NULL, 3, TK_ELIF, TK_ELSE, TK_FI);
				if (!cond)
					error = eval(str, env, i, &quit);
				// if we don't found an else, skip to the fi
				if (found == TK_FI)
					state = 6;
			case 5:
				break; // read the "else"
			case 6: // if we have an "else" part, limit it and run it
				find_token(str, &i, NULL, TK_FI);
				if (cond)
					error = eval(str, env, i, &quit);
			case 7:	// jump the fi
				break;
		}
		// DEBUG
		//printf("DEBUG eval_if: %d %d %.*s\n", state, i, i, str);
		cmd = str + i;
		state++;
	}
	*len = cmd - save;
	return error;
}

static uint8_t syntax_for(char *cmd)
{
	uint8_t tok = 0, prev_tok;
	uint8_t state = 0;
	uint8_t error = 0;
	size_t i;
	char *str;

	while (state < 8)
	{
		i = get_string(cmd, &str);
		tok = get_token(str, i);

		switch (state)
		{
			case 0: // jump the if
				break;
			case 1: // the variable name
				if (! is_varname_ok(str, i))
					error = 1;
				break;
			case 2: // the optional "in"
				if (tok == TK_IN)
					;
				else if (tok == TK_DO)
					state += 3;
				else
					error = 1;
				break;
			case 3: // the values
				i = extend_to_eol(str, i);
				break;
			case 4: // check new line or separator
				if (! tok_is_separator(tok))
					error = 1;
				break;
			case 5: // "do"
				if (tok != TK_DO)
					error = 1;
				break;
			case 6: // the instructions
				if (find_token(str, &i, &prev_tok, TK_DONE) != TK_DONE)
					error = 1;
				if (! tok_is_separator(prev_tok))
					error = 1;
				break;
			case 7: // done
				if (tok != TK_DONE)
					error = 1;
				break;
		}
		// DEBUG
		//printf("DEBUG: %d tok: %d\n", state, tok);

		if (error)
		{
			printf_P(PSTR("Error in FOR statement, state: %d pos: %s\n"), state, cmd);
			return 1;
		}
		cmd = str + i;
		state++;
	}

	return 0;
}

static int8_t eval_for(char *cmd, size_t *len, char *env[])
{
	uint8_t tok = 0;
	int8_t error = 0;
	uint8_t state = 0;
	uint8_t quit = 0;
	size_t i = *len, j;
	char *str, *save, *var, *loop, *values, *actual;
	
	save = cmd;
	values = NULL;
	loop = NULL;

	while (state < 8 && ! quit)
	{
		i = get_string(cmd, &str);

		switch (state)
		{
			case 0: // jump the for
				break;
			case 1: // the variable name
				str[i] = 0;
				var = str;
				i++;
				//DEBUG: printf("DEBUG var: \"%s\"\n", var);
			case 2: // the optional "in"
				tok = get_token(str, i);
				if (loop == NULL)
					loop = str + i;
				if (tok == TK_IN)
					;
				else if (tok == TK_DO)
				{
					/*positional++;
					if (argv[positional] == NULL)
					{
						find_token(str, &i, NULL, TK_DONE);
						state = 6;
					}
					else
					{
						env_add(env, var, argv[positional]);
						state += 3;
					}*/
				}
				break;
			case 3: // the values
				if (values == NULL)
					values = str;
				j = get_string(values, &actual);
				//DEBUG: printf("DEBUG values: %d \"%.*s\"\n", j, j, actual);
				if (j == 0 || is_separator(actual[0])) // no more values
				{
					i = extend_to_eol(str, i);
					i = get_string(str + i, &str); // read the new line separator
					i = get_string(str + i, &str); // read the "do"
					str += i;
					find_token(str, &i, NULL, TK_DONE);
					state = 6;
				}
				else
				{
					env_add_l(env, var, strlen(var), actual, j);
					values = actual + j;
					i = extend_to_eol(str, i);
				}
			case 4: // check new line or separator
			case 5: // "do"
				break;
			case 6: // the instructions
				find_token(str, &i, NULL, TK_DONE);
				//printf("DEBUG do: %d \"%.*s\"\n", i, i, str);
				error = eval(str, env, i, &quit);
				str = loop;
				i = 0;
				state = 1; 
				break;
			case 7: // done
				break;
		}
		// DEBUG
		//printf("DEBUG eval_for: %d %d %.*s\n", state, i, i, str);
		cmd = str + i;
		state++;
	}
	*len = cmd - save;
	return error;
}

static int8_t eval_cd(char *str, size_t len, char *env[])
{
	int8_t ret;
	char *path;
	char args[ARGMAX];
	char *argv[NCARGS];

	// transform string in parameters
	str_to_argv(str, len, args, argv, env);

	// if it has no parameters, just cd to home
	if (argv[1] == NULL)
		return chdir(HOME);

	// take only the first parameter and normalize it
	path = sanitize_path(argv[1]);

	// change directory
	ret = chdir(path);
	free(path);

	return ret;
}

static int8_t eval_exit(char *str, size_t len, char *env[])
{
	int ret = 0;
	char args[ARGMAX];
	char *argv[NCARGS];

	// transform string in parameters
	str_to_argv(str, len, args, argv, env);

	if (argv[1] != NULL)
	{
		ret = atoi(argv[1]);
		if (ret > 127 || ret < 0 || (ret == 0 && argv[1][0] != '0'))
			ret = EINVAL;
	}

	return (int8_t)ret;
}

static int8_t eval_command(char *str, size_t len, char *env[])
{
	char error = 0;
	char args[ARGMAX];
	char *argv[NCARGS];
	size_t pos;
  
	// Check if is a variable assigment
	if ((pos = is_var_assign(str, len)) != false)
	{
		if (!is_varname_ok(str, pos))
			return 1;
		return env_add_l(env, str, pos, str + pos + 1, len - pos - 1);
	}

	// Discard empty commands
	if (str[0] == 0)
		return error;
	str_to_argv(str, len, args, argv, env);
	error = execve((const char **) argv, env);

	return error;  
}

static int8_t eval(char *cmd, char *env[], size_t limit, uint8_t *quit)
{
	size_t len;
	uint8_t tok = 0;
	int8_t error = 0;
	char *str, *input;
	char retcode[4];

	input = cmd;

	while ((len = get_string(input, &str)) > 0 && ! *quit)
	{
		// If the limit is stablished, check it
		if (limit > 0 && str + len > cmd + limit)
			break;

		// extract the token
		tok = get_token(str, len);
		//DEBUG
		//printf("DEBUG STR: %2d %2d %.*s\n", len, tok, len, str);
		
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
				if ((error = syntax_if(str)) > 0)
					break;
				error = eval_if(str, &len, env);
				//printf("DEBUG: \"%.*s\"\n", len, str);
				break;

			// for structure
			case TK_FOR:
				if ((error = syntax_for(str)) > 0)
					break;
				error = eval_for(str, &len, env);
				break;
#endif

			case TK_CD:
				len = extend_to_eol(str, len);
				error = eval_cd(str, len, env);
				break;

			case TK_EXIT:
				len = extend_to_eol(str, len);
				error = eval_exit(str, len, env);
				if (error >= 0)
					*quit = 1;
				break;

			// a simple command followed by parameters until '\n' or ';'
			case false:
				len = extend_to_eol(str, len);
				//DEBUG
				//printf("DEBUG: eval_command: %.*s\n", len, str);
				error = eval_command(str, len, env);
				break;

			default:
				printf_P(PSTR("Syntax error: %s\n"), str);
				return 1;
		}
		// advance input pointer
		input = str + len;

		// save the result of the last command
		snprintf_P(retcode, 3, PSTR("%d"), error);
		env_add(env, "?", retcode);
	}
	
	return error;
}

// get command from input
static uint8_t getcmd(char buff[])
{
  uint8_t buffp = 0;
  uint8_t c;
  uint8_t _exit = 1;

  do
  {
    // read input
    c = getchar();
    // local echo
    if (CONSOLE_ECHO)
    {
	if (c == '\r')
		putchar('\n');
	putchar(c);
    }

    // treat special charaters
    switch(c)
    {
      // delete (bakcspace over serial port)
      case 0x7f:
        if (buffp > 0)
        {
          putchar(8);
          putchar(' ');
          putchar(8);
        }
      // backspace
      case 0x08:
        buff[buffp]=0;
        if (buffp > 0)
          buffp--;
        break;
      // horizontal tab
      case 0x09:
        // TODO
        break;
      case '\n':
      case '\r':
        _exit = 0;
      // Rest of characters
      default:
        buff[buffp++] = c;
        // increment index
        //buffp++;
    }

    // Check the buffer used
    if (buffp == ARGMAX)
    {
      puts_P(PSTR("ERROR: Line too long, " STR(ARGMAX) " limit reached\n"));
      return false;
    }
  }
  while (_exit);

  // end the command with a \0
  buff[buffp] = 0;

  return buffp;
}

int8_t main_sh(char *argv[], char *env[])
{
  // input line buffer
  char line[ARGMAX], *cwd;
  size_t len;
  uint8_t quit = 0;
  int8_t result = 0;

  if (argv[1] != NULL)
  {
    /*if ((open(argv[1], O_RDONLY, &fd)) != 0)
      return 1;
    */
    // TODO
    printf_P(PSTR("TODO RUN: %s\n"), argv[1]);
    return 0;
  }

  // Loop until we exit cmd
  do
  {
    // show prompt
    cwd = getcwd(NULL, 0);
    printf_P(PSTR("%s# "), cwd);
    free(cwd);

    // Get command
    len = getcmd(line);
    if (!len)
      continue;

    result = eval(line, env, len, &quit);
    // For debuging
    printf_P(PSTR("Result: %d\n"), result);

  // TODO: Implement a decent exit mechanism
  } while(!quit);

  return result;
}


