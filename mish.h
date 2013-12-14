//Mish.h
//The header for cs243 project3 mish
//Author Colin L Murphy clm3888@rit.edu
//Date: 12/12/13
//////////////////////////////////////////////////

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


//External function definition, see the implementation for full comments
int tokenize(char *line, char *tokens[]);
int help( int argc, char ** argv );
int history( int argc, char ** argv );
int verbose( int argc, char **argv );
int quit( int argc, char **argv );
int external( int argc, char **argv);
