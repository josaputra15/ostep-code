#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_LINE	1024
#define MAX_ARGS	50


static char **	parse_args(char *cp);
static void	print_args(char **argv);

/*
 * An example of how to parse command line arguments.
 *
 * Reads input lines.
 * Parses them into char **s delimited by whitespace.
 * Prints out the char **s.
 */
int main(int argc, char **argv)
{
	char buff[MAX_LINE],
	     **cpp;

	while (fgets(buff, MAX_LINE, stdin) != NULL)  {
		cpp = parse_args(buff);
		print_args(cpp);
	}
	return 0;
}

/*
 * Given a string, parse it into an array of whitespace separated tokens.
 * The pointer AFTER the final token should be NULL.
 */
static char **
parse_args(char *cp)
{
        char** s = malloc(sizeof(char*)*5); // I arbitrarily chose 5 - try with different numbers and see what happens
        char* token = strtok(cp, " ");
        int i = 0;
        while(token){
                s[i] = token;
                token = strtok(NULL, " ");
                i++;
        }
        s[i] = NULL;
        return s;
}

/*
 * Prints out an array of tokens.
 */
static void
print_args(char **argv)
{
	int i = 0;

	for (i = 0; argv[i] != NULL; i++) {
		printf("arg %d is %s\n", i, argv[i]);
	}
}