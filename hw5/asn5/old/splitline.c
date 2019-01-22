#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	"driver.h"
#include	"varlib.h"

/**
 **	splitline ( parse a line into an array of strings )
 **/
int splitline(char *cmdline, int *argcp, char *argv[], int max)
/*
 *	cmdline has a string of white-space separated tokens
 *	put the addresses of the tokens in the array argv[]
 *	put their number in *argcp and do not put more than max 
 *	in argv or suffer dire consequences!
 *	NOTE: this modifies cmdline
 *	returns FALSE on too many args or zero args.  TRUE for ok stuff
 */
{
	int i = 0, retval = FALSE ;
	char *cmdp = cmdline ;

	while ( i<=max ){			/* still room? 		*/
						/* skip leading space	*/
		while ( *cmdp == ' ' || *cmdp == '\t' )
			cmdp++;
						/* at end of string?	*/
		if ( *cmdp == '\0' )
			break;
						/* record string	*/
		argv[i++] = cmdp ;		/* and bump counter	*/
		
						/* move to end of word	*/
		while ( *++cmdp && *cmdp != ' ' && *cmdp != '\t' )
			;

		if ( *cmdp != '\0' )		/* past end of word	*/
			*cmdp++ = '\0';		/* terminate string	*/
	}
	if ( i > max )
		printf("Too many args\n");
	else if ( i > 0 ){
		argv[i] = NULL ;		/* mark end of array	*/
		*argcp = i;			/* and store argc	*/
		retval = TRUE ;			/* say ok		*/
	}
	return retval ;
}

/**
 ** parseLine( char* tok )
 **		Transforms the token with variables 
 **		and symbols to a replaced final parameter.
 **		Checks '\' for special characters,
 **		Ignores numbers after '$' signs and
 **		and replaces combinations of
 **		variables within the token.
 **		Works recursively along all the length
 **		of the command line.
 **
 **		If a variable is not found, it
 **		it is returned for use in error message.
 **/
char* parseLine( char* tok )
{
	char * cp, after[MAXCMDLEN], * ret = NULL;
	char * vname, * vresult, ch;
	int n;
	vname = malloc( MAXARG );

	if ( (cp=strchr(tok, '$')) == NULL)		/* look for '$' */
		return NULL;

	/* Check for preeceding '\' */

	if ( strchr(cp-1, 92) != NULL) {
		strcpy(after, cp);
		*(cp-1) = '\0';
		strcat(tok, after);		/* extract '\' from string */
		if ( strlen(cp) > 1 )	/* check the rest of the string */
			ret = parseLine(cp);
		return ret;				/* return if no more characters */
	}
	
	/** Check for valid characters	**/
	/** in variable name			**/
	
	/* Check for any invalid character after'$' */
	for (n=0,ch = *(cp+1);	(ch < 65 || ch > 90)  &&
							(ch < 97 || ch > 122) &&
							 ch != 95  && ch != '\0'  ; )
		n++, ch = *(cp+1+n);
	/* Supress '$' and leading invalid characters found */
	if (n > 0) {
		strcpy(after, cp+1+n);
		*cp = '\0';
		strcat(tok, after);
		if ( strlen(cp) > 1 )		/* are more character? */
		ret = parseLine(cp);		/* check the rest */
		return ret;					/* return result */
	}

	/** A valid variable name is detected,	**/
	/** now delimit the variable name		**/
	for (n=0,ch = *++cp;	(ch > 47 && ch < 58)  ||
							(ch > 64 && ch < 91)  ||
							(ch > 96 && ch < 123) ||
							 ch == 95				  ; )
		n++, ch = *(cp+n);

	/* extract variable name from string */
	strncpy( vname, cp, n);
	vname[n] = '\0';
	/* check in vars table */
	if ((vresult = VLlookup( vname )) == NULL)
		return vname;			/* if var not found */
								/* end this line parsing */
								/* and return var name for error msg */
	/* replace var name with var result */
	strcpy( after, cp+n );
	*--cp = '\0';
	strcat( strcat( tok, vresult), after);

	/* update and check for the rest of the string */
	cp = cp + strlen(vresult);
	ret = parseLine(cp);

	return ret;
}
