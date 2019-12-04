#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char * retrieve_input();
char ** split_line_into_tokens();
char * path;
void set_path();
void print_pwd();
void parse();
void remove_spaces();

void parse();
void cd();
void print_error();
char * current_directory();
void print_directory();
char * deblank();

int main(int argc, char *argv[]) 
{
    int mode = 0;
    FILE *file = stdin;

    if(argc >= 2)
    {
        mode = 1;
        file = fopen(argv[1], "r+");
    }

    char *line;
    char **tokens;
    int repeat;

    while (1)
    {
        if(mode == 0)
        {
            printf("grsh> ");
        }

        line = retrieve_input(file);
        tokens = split_line_into_tokens(line);
        parse(tokens);

        free(line);
        free(tokens);
    }
}

// Either reads user input or file and puts into tokens
char * retrieve_input(FILE * file)
{
    char *line = NULL;
    size_t buffer = 0;
    size_t read;

    read = getline(&line, &buffer, file);
    if(read == '\0')
    {
        exit(0);
    }

    return line;
}

// Splits the line into tokens
char ** split_line_into_tokens(char * line)
{
    int buffer = 64;
    char **tokens = malloc(buffer * sizeof(char*));
    char *token;

    token = strtok(line, " ");

    int i = 0;
    while(token != NULL)
    {
        tokens[i] = token;
        i++;

        if(i >= buffer)
        {
            buffer += buffer;
            tokens = realloc(tokens, buffer * sizeof(char*));
        }

        token = strtok(NULL, " ");
    }

    tokens[i] = NULL;

    return tokens;
}

// Reads the tokens and calls related function
void parse(char * tokens[])
{
    int token_args = strlen(*tokens);
    if(strncasecmp(tokens[0], "cd", 2) == 0)
    {
        cd(tokens[1], token_args);
        print_directory();
    }
    else if(strncasecmp(tokens[0], "cwd", 3) == 0)
    {
        print_directory();
    }
    else if(strncasecmp(tokens[0], "path", 4) == 0)
    {
        set_path(tokens, token_args);
    }
}

// When user calls cd
void cd(char *token, int token_args)
{

    if(token == NULL)
    {
        print_error();
    }
    else
    {
        if(chdir(token) != 0) 
        {
            print_error();
        }
    }
}

// Sets shell variable PATH
void set_path(char *tokens[], int token_args)
{
    char * path = malloc(100 * sizeof(char));
    int size = 0;
    for(int i = 1; i < token_args; i++)
    {
        size += strlen(tokens[i])+1;
        if(size >= strlen(path))
        {
            path = realloc(path, 100 * sizeof(char));
        }

        strcat(path, tokens[i]);

        if(i != token_args-1)
        {
          strcat(path, ":");
        }
    }

    setenv("PATH", path, 1);
    system("echo $PATH");
}

// prints the current directory
void print_directory() 
{
    char cwd[1024];
    printf("%s", getcwd(cwd, sizeof(cwd)));
}

// returns the current directory
char * current_directory()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    return cwd;
}

// prints the general error message supplied
void print_error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void print_pwd()
{

}

void set_env(char * env)
{
    setenv(env, path, 1);
}

char * deblank(char *str)
{
    char *out = str, *put = str;

    for(; *str != '\0'; ++str)
    {
        if(*str != ' ')
            *put++ = *str;
    }
    *put = '\0';

    return out;
}

void remove_spaces (char* restrict str_trimmed, const char* restrict str_untrimmed)
{
  while (*str_untrimmed != '\0')
  {
    if(!isspace(*str_untrimmed))
    {
      *str_trimmed = *str_untrimmed;
      str_trimmed++;
    }
    str_untrimmed++;
  }
//  *str_trimmed = '\0';
}
