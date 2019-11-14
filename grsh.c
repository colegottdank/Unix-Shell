#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void getTerminalInput();
void getBatchFile();

int main(int argc, char *argv[]) 
{
    if(argc < 2) 
    {
	getTerminalInput(); 
    } 
    else 
    { 
	getBatchFile(); 
    }    
}

void getTerminalInput()
{
    while(1) 
    {   
	char *userInput = NULL;     
	printf("grsh> ");
	getline(&userInput, 0, stdin);

	if(strcmp(userInput, "exit") == 0)
	{
	    exit(1);
	}
    }
}

void getBatchFile(char *argv[])
{
  
}
