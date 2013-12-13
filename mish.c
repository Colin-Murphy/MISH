#include "mish.h"

int main(int argc, char *argv[]) {
	
	char *line;
	size_t maxLine = 1024;
	line = (char *)malloc( maxLine );
	int bytesRead = 0;
	while (!feof(stdin)) {
		
		bytesRead = getline(&line, &maxLine, stdin);
		
		//Ctrl-d were done
		if (bytesRead < 0) {
			break;
		}
				
		
		char *tokens[bytesRead];
		
		int i = tokenize(line, tokens);
		printf("%s\n", tokens[1]);
		
		if (strcmp(line, "quit\n") == 0) {
			break;
		}
		
		printf("%s",line);
	}
	
	
	printf("Done\n");
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
	bool apos = false;
	bool quote = false;

	
	while (pos < len -1) {
		char *nQuote = strchr(line + pos, '\"');
		if (nQuote) {
			nextQuote = (int)(nQuote - line + pos);
		}
		
		char *nApos = strchr(line + pos, '\'');
		if (nApos) {
			nextApos = (int)(nApos - line + pos);
		}
		
		int stop = len;
		if (nextQuote >= 0 && (nextQuote < nextApos || nextApos < 0)) {
			quote = true;
			apos = false;
			stop = nextQuote;
		}
		
		else if (nextApos >= 0 && (nextApos < nextQuote || nextQuote < 0)) {
			quote = false;
			apos = true;
			stop = nextApos;
		} 
		char *substring;
		substring = malloc(stop + 1);
		strncpy(substring, line+pos, stop);
		
		char *token;
		token = strtok (substring," ");
		while (token != NULL) {
			//printf ("%s\n",token);
			tokens[count] = malloc(sizeof(token));
			//printf("Hello2\n");
			strcpy(tokens[count], token);
			//printf("Hello3\n");
			count++;
			pos+=strlen(token + 1);
			token = strtok (NULL, " ");
		}
		printf("stop is %d\n",stop);
		//pos = stop;
		//Free the substring before reusing it
		free(substring);
		substring = malloc(len + 1);
		strncpy(substring, line+pos + 1, len+1);
		printf("Substring is %s\n",substring);
	
		
		if (quote) {
			nQuote = strchr(substring, '\"');
			if (nQuote) {
				nextQuote = (int)(nQuote - substring);
				printf("Next Quote %d\n",nextQuote);
				tokens[count] = malloc(nextQuote + 1);
				strncpy(tokens[count],substring, nextQuote);
				count++;
				
				//+2 because of the start and end quotes
				pos += nextQuote + 2;
				
				printf("Position is %d and len is %d\n",pos, len);
			}
		}
				
	}
	
	return count;
}