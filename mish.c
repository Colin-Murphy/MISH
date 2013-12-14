#include "mish.h"

char *commandHistory[1024];
int count = 0;
bool verb = false;

int main(int argc, char *argv[]) {
	
	size_t maxLine = 1024;
	char *line = NULL;
	line = (char *)malloc( maxLine );
	int bytesRead = 0;
	while (!feof(stdin)) {
		printf("mish[%d]> ",count+1);
		bytesRead = getline(&line, &maxLine, stdin);
		
		//Ctrl-d were done
		if (bytesRead < 0) {
			break;
		}
				
		
		char *tokens[bytesRead];
		
		int tok = tokenize(line, tokens);
		
		
		if (tok >0) {
			//Internal command: quit
			if (strcmp(tokens[0], "quit") == 0) {
				quit(tok,tokens);
				break;
			}
			
			//Internal command: help
			else if (strcmp(tokens[0],"help") ==0) {
				help(tok, tokens);
			}
			//Internal command: history
			else if (strcmp(tokens[0],"history") ==0) {
				history(tok, tokens);
			}
			
			//Internal command: verbose
			else if (strcmp(tokens[0],"verbose") ==0) {
				verb = verbose(tok, tokens);
				
			}
			
			//External commands
			else {
				//handle verbose printing
				if (verb) {
					printf("\tcommand: %s\n",line);
					printf("\tinput command tokens:\n");
					
					for (int i = 0; i< tok; i++) {
						printf("\t%d: %s.\n",i,tokens[i]);
					}
				}
				
				//run the external command
				external(tok, tokens);	

	
			}
			
			
	
			for (int i=0; i<tok; i++) {
				free(tokens[i]);
			}
			
			//Update the command history
			commandHistory[count] = malloc(strlen(line)+1);
			strcpy(commandHistory[count],line);
			count++;
		}

	}
	
}

//Break the string into tokens while preserving text in single or double quotes
//param line: the line of text to read in
//param tokens: the array of tokens to store into
//return int: the number of tokens found
int tokenize(char *line, char *tokens[]) {
	int len = strlen(line);
	int pos = 0;
	int nextQuote = -1;
	int nextApos = -1;
	int count = 0;


	
	while (pos < len) {
		//Break up all text leading up to the first " or '
		if (line[pos] != '\"' && line[pos] !='\'') {
			
			char *nQuote = strchr(line + pos, '\"');
			if (nQuote) {
				nextQuote = (int)(nQuote - (line + pos));
				
			}

			
			char *nApos = strchr(line + pos, '\'');
			if (nApos) {
				nextApos = (int)(nApos - (line + pos));
			}
			

			int stop = len;
			
			if (nextQuote >= 0 && (nextQuote < nextApos || nextApos < 0)) {
				stop = nextQuote;
			}
			
			else if (nextApos >= 0 && (nextApos < nextQuote || nextQuote < 0)) {
				stop = nextApos;
			}
			char *substring;
			substring = malloc(stop + 1);
			strncpy(substring, line+pos, stop);
			substring[stop] = '\0';

			
			char *token;
			token = strtok(substring," \n");
			while (token != NULL) {
				tokens[count] = malloc(strlen(token) + 1);
				token[strlen(token)] = '\0';

				tokens[count] = strdup(token);
				count++;
				
				token = strtok (NULL, " \n");
			}
			
			//free the memory
			free(substring);
			//update the position
			pos += stop;
			
			//reset these so the function can run again properly
			nextQuote = -1;
			nextApos = -1;
					
		
		}
		
		//line[pos] is a double or single quote
		else {
			//skip the first character since we don't want the " or '
			int start = pos + 1;
			//The location of the corresponding charcter (" or ') relative to pos
			int stop;
			//Found quote
			if (line[pos] == '\"') {
				//Find the next " and mark that as the stop point
				char *nQuote = strchr(line + pos + 1, '\"');
				if (nQuote) {
					nextQuote = (int)(nQuote - (line + pos));
					stop = nextQuote;

				}
				
				else {
					for (int i = 0; i<count; i++) {
						free(tokens[i]);
					}
					perror("ERROR: out of quotes\n");
					return -1;
				}
			}
			//Found single quote
			else {
				char *nApos = strchr(line + pos + 1, '\'');
				if (nApos) {
					nextApos = (int)(nApos - (line + pos));
					stop = nextApos;

				}
				else {
					for (int i = 0; i<count; i++) {
						free(tokens[i]);
					}
					perror("ERROR: out of single quotes\n");
					return -1;
				}
			}
			
			char *substring;
			substring = malloc(sizeof(char) * stop);
			strncpy(substring, line + start,stop-1 );
			substring[stop-1] = '\0';
			
			tokens[count] = malloc(strlen(substring) +1);
			strcpy(tokens[count], substring);
			count++;
			pos = pos+stop + 1;
			
			free(substring);
			nextQuote = -1;
			nextApos = -1;
			
		}
		
		
				
	}
	//return the number of tokens parsed
	return count;
}

//Run external commands
//param argc: number of tokens
//param argv: tokens parsed
//return int: the status returned by the command, or -1 on error
int external( int argc, char **argv) {
	pid_t id, my_id;
	int status;
	
	id = fork();
	switch( id ) {

	//Error :(
	case -1:
		perror( "fork" );
		exit( EXIT_FAILURE );
	
	//in child process
	case 0:

		// report our identity
		my_id = getpid();
		
		//Add a null token so execvp works
		argv[argc] = NULL;
		
		if (verb) {
			printf("\twait for pid %d: %s\n",my_id, argv[0]);
			printf("\texecvp: %s\n",argv[0]);
		}

		execvp( argv[0], argv );

		//print the error
		perror( "execvp" );

		//give up and exit
		_exit( EXIT_FAILURE );

		break;

	//parent
	default:
		break;

	}

	// parent will wait for child to exit
	id = wait( &status );
	if( id < 0 ) {
		perror( "wait" );
		//wort happen, but this return keeps the compiler happy
		return -1;
	}
	else {
		if (status != 0) {
			printf("Status is: %d\n",status);
		}
		
		return status;
	}
	
	
}
//Prints the help menu
//param argc: number of tokens (unused)
//param argv: tokens parsed (unused)
//return int: unused, but the spec requires it
int help( int argc, char ** argv ) {
	printf("Mish Internal Commands\n");
	printf("\tverbose on/off: turn the shell verbose on or off.\n");
	printf("\thelp: print a list of internal commands.\n");
	printf("\thistory: print a list of commands executed so far, including their arguments.\n");
	printf("\tquit: cleans up memory and gracefully terminates mish.\n");
	
	return 0;	
}

//Toggles verbose
//param argc: number of tokens
//param argv: tokens parsed
//return int: whether verbose is on or off
int verbose( int argc, char **argv ) {
	
	if (strcmp(argv[1],"on") == 0) {
		verb = true;
	}
	
	else if (strcmp(argv[1],"off") == 0) {
		verb = false;
	}
	
	else {
		perror("Invalid state for verbose\n");
	}
	
	
	return verb;	
}
//Prints the command history
//param argc: number of tokens (unused)
//param argv: tokens parsed (unused)
//return int: unused, but the spec requires it
int history( int argc, char **argv ) {
	printf("Command History\n");
	for (int i = 0; i<count; i++) {
		printf("\t mish[%d] > %s",i, commandHistory[i]);
	}
	
	return 0;
}
//Quits mish
//param argc: number of tokens
//param argv: tokens parsed
//return int: unused, but the spec requires it
int quit( int argc, char **argv ) {
	//Clean up all the allocated memory
	for (int i=0; i<argc; i++) {
		free(argv[i]);
	}
	
	for (int i=0; i<count; i++) {
		free(commandHistory[i]);
	}
	return 0;
}
