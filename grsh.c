#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char * retrieve_input();
char ** split_line_into_tokens();
char ** split_line_into_tokens2();
char * path;
void set_path();
void print_pwd();
void parse();
void remove_spaces();
void cat();
int token_size;
int path_size;
char * concatenate();
void print_token();
void parse();
void cd();
void execute();
void print_error();
char * current_directory();
void print_directory();
void not_built_in();
char * deblank();

int main(int argc, char *argv[]) 
{
    int mode = 0;
    FILE *file = stdin;
    setenv("PATH", "/bin", 1);

    if(argc >= 2)
    {
        mode = 1;
        file = fopen(argv[1], "r+");
    }

    char *line;
    char **tokens;

    while (1)
    {
        if(mode == 0)
        {
            printf("grsh> ");
        }

        line = retrieve_input(file);
        tokens = split_line_into_tokens(line, " \t\r\n\a");
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

    if(strncasecmp(line, "exit", 4) == 0)
    {
        exit(0);
    }

    return line;
}

// Splits the line into tokens
char ** split_line_into_tokens(char * line, char * delim)
{
    int buffer = 64;
    char **tokens = malloc(buffer * sizeof(char*));
    char *token;
    token_size = 0;
    token = strtok(line, delim);

    int i = 0;
    while(token != NULL)
    {
        tokens[i] = token;
        i++;
        token_size++;

        if(i >= buffer)
        {
            buffer += buffer;
            tokens = realloc(tokens, buffer * sizeof(char*));
        }

        token = strtok(NULL, delim);
    }

    tokens[i] = NULL;

    return tokens;
}

char ** split_line_into_tokens2(char * line, char * delim)
{
    int buffer = 64;
    char **tokens = malloc(buffer * sizeof(char*));
    char *token;
    path_size = 0;
    token = strtok(line, delim);

    int i = 0;
    while(token != NULL)
    {
        tokens[i] = token;
        i++;
        path_size++;

        if(i >= buffer)
        {
            buffer += buffer;
            tokens = realloc(tokens, buffer * sizeof(char*));
        }

        token = strtok(NULL, delim);
    }

    tokens[i] = NULL;

    return tokens;
}
// Reads the tokens and calls related function
void parse(char * tokens[])
{
    if(strncasecmp(tokens[0], "cd", 2) == 0)
    {
        cd(tokens);
    }
    else if(strncasecmp(tokens[0], "cwd", 3) == 0)
    {
        print_directory();
    }
    else if(strncasecmp(tokens[0], "path", 4) == 0)
    {
        set_path(tokens);
    }
    else
    {
        not_built_in(tokens);
    }
}

void not_built_in(char * tokens[])
{
    pid_t pid, wpid;
    int status;
    pid = fork();

    if(pid == 0)
    {
        if(strncasecmp(tokens[0], "cat", 3) == 0)
        {
            cat(tokens);
        }
        else
        {
            execute(tokens);
        }
    }
    else if (pid < 0)
    {

    }
    else
    {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

void execute(char *tokens[])
{
    char ** paths;
    paths = split_line_into_tokens2(getenv("PATH"), ":");
    char * temp_path = malloc(sizeof(char) * 1000); 
    for(int i = 0; i < path_size; i++)
    {
        temp_path[0] = '\0';
        strcat(temp_path, paths[i]);
        strcat(temp_path, "/");
        strcat(temp_path, tokens[0]);
        
        if(access(temp_path, X_OK) != 0)
        {
            printf("why");
            print_error();
        }
        else
        {
            execv(temp_path, tokens);
            break;
        }
    }
    free(temp_path);
    free(paths);
}

// When user calls cd
void cd(char *tokens[])
{
    if(tokens[1] == NULL)
    {
        print_error();
    }
    else if(chdir(tokens[1]) != 0) 
    {
        print_error();
    }

}

// Sets shell variable PATH
void set_path(char *tokens[])
{
    char * path = malloc(sizeof(char) * 1000);

    for(int i = 1; i < token_size; i++)
    {
        strcat(path, tokens[i]);
        if(token_size-1 != i)
        {
            strcat(path, ":");
        }
    }

    setenv("PATH", path, 1);
}

void cat(char *tokens[])
{
    char * argv[token_size];
    argv[0] = "/bin/cat";

    for(int i = 1; i < token_size; i++)
    {
        argv[i] = tokens[i];
    }

    argv[token_size] = 0;

    execvp(argv[0], argv);
}

// prints the current directory
void print_directory() 
{
    char cwd[1024];
    printf("%s", getcwd(cwd, sizeof(cwd)));
}

// returns the current directory
/*char * current_directory()
  {
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  return cwd;
  }*/

// prints the general error message supplied
void print_error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
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

void print_token(char * token)
{
    for(int i = 0; i < strlen(token); i++)
    {
        printf("%c", token[i]);
    }
    printf("-");
}

char *concatenate(size_t size, char *array[size], const char *joint){
    size_t jlen, lens[size];
    size_t i, total_size = (size-1) * (jlen=strlen(joint)) + 1;
    char *result, *p;


    for(i=0;i<size;++i){
        total_size += (lens[i]=strlen(array[i]));
    }
    p = result = malloc(total_size);
    for(i=0;i<size;++i){
        memcpy(p, array[i], lens[i]);
        p += lens[i];
        if(i<size-1){
            memcpy(p, joint, jlen);
            p += jlen;
        }
    }
    *p = '\0';
    return result;
}
