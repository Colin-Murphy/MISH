#include "mish.h"

int main(int argc, char *argv[]) {
	
	int count = 0;
	char *line;
	size_t maxLine = 1024;
	line = (char *)malloc( maxLine );
	char *history[maxLine];
	int bytesRead = 0;
	bool verbose = false;
	while (!feof(stdin)) {
		printf("mish[%d]> ",count+1);
		bytesRead = getline(&line, &maxLine, stdin);
		
		//Ctrl-d were done
		if (bytesRead < 0) {
			break;
		}
				
		
		char *tokens[bytesRead];
		
		int tok = tokenize(line, tokens);
		
		
		if (tok >=0) {
			//Internal command: quit
			if (strcmp(tokens[0], "quit") == 0) {
				for (int i=0; i<tok; i++) {
					free(tokens[i]);
				}
				
				for (int i=0; i<count; i++) {
					free(history[i]);
				}
				break;
			}
			
			//Internal command: help
			else if (strcmp(tokens[0],"help") ==0) {
				printf("verbose on/off: turn the shell verbose on or off.\n");
				printf("help: print a list of internal commands.\n");
				printf("history: print a list of commands executed so far, including their arguments.\n");
				printf("quit: cleans up memory and gracefully terminate mish.\n");
			}
			//Internal command: history
			else if (strcmp(tokens[0],"history") ==0) {
				printf("Command History\n");
				for (int i = 0; i<count; i++) {
					printf("\t mish[%d] > %s",i, history[i]);
				}
			}
			
			//Internal command: verbose
			else if (strcmp(tokens[0],"verbose") ==0) {
				if (strcmp(tokens[1],"on") == 0) {
					verbose = true;
				}
				
				else if (strcmp(tokens[1],"off") == 0) {
					verbose = false;
				}
				
				else {
					perror("Invalid state for verbose\n");
				}
				
			}
			
			//External commands
			else {
				if (verbose) {
					printf("\tcommand: %s\n",line);
					printf("\tinput command tokens:\n");
					
					for (int i = 0; i< tok; i++) {
						printf("\t%d: %s.\n",i,tokens[i]);
					}
				}	
			}
			
			
	
			for (int i=0; i<tok; i++) {
				free(tokens[i]);
			}
		}
		history[count] = malloc(strlen(line)+1);
		strcpy(history[count],line);
		count++;

	}
	
	
	printf("Goodbye\n");
}

//Break the string into tokens while preserving text in single or double quotes
//param line: the line of text to read in
//param tokens: the array of tokens to store into
//return int: the number of tokens found
int tokenize(char *line, char *tokens[]) {
	
	//tokens[0] = malloc(sizeof("Hello") + 1);
	//strcpy(tokens[0], "Hello");
	
	int len = strlen(line);
	int pos = 0;
	int nextQuote = -1;
	int nextApos = -1;
	int count = 0;


	
	while (pos < len) {
		
		//printf("Working on string from %d to %d\n",pos, len);
		
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
			char *token;
			token = strtok (substring," \n");
			while (token != NULL) {
				tokens[count] = malloc(strlen(token) + 1);
				token[strlen(token)] = '\0';
				strcpy(tokens[count], token);
				count++;
				token = strtok (NULL, " \n");
			}
			
			free(substring);
			pos += stop;
			nextQuote = -1;
			nextApos = -1;
					
		
		}
		
		//line[pos] is a double or single quote
		else {
			int start = pos + 1;
			int stop;
			//Found quote
			if (line[pos] == '\"') {
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
			printf("Pos is %d\n",pos);
			
			free(substring);
			nextQuote = -1;
			nextApos = -1;
			
		}
		
		
				
	}
		
	return count;
}