#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __linux__
#define PYTHON27 "Python 2.7.16rc1 (default, Feb 18 2019, 11:05:09)\n[GCC 8.2.0] on linux2\nType \"help\", \"copyright\", \"credits\" or \"license\" for more information.\n"

#define PYTHON37 "Python 3.7.2+ (default, Feb 27 2019, 15:41:59)\n[GCC 8.2.0] on linux\nType \"help\", \"copyright\", \"credits\" or \"license\" for more information.\n"
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define PYTHON27 "Python 2.7.10 (default, Aug 17 2018, 19:45:58)\n[GCC 4.2.1 Compatible Apple LLVM 10.0.0 (clang-1000.0.42)] on darwin\nType \"help\", \"copyright\", \"credits\" or \"license\" for more information.\n"

// Not taken from a real system
#define PYTHON37 "Python 3.7.2+ (default, Feb 27 2019, 15:41:59)\n[GCC 4.2.1 Compatible Apple LLVM 10.0.0 (clang-1000.0.42)] on darwin\nType \"help\", \"copyright\", \"credits\" or \"license\" for more information.\n"
#endif


static void print_prompt(void)
{
    printf(">>> ");
}

static bool comment_or_blank(char *line, size_t len)
{

    if (line == NULL)
        return false;

    for (size_t idx = 0; idx < len; idx++)
    {
        switch(line[idx])
        {
        case ' ':
        case '\t':
            continue;
        case '#':
        case '\n':
        case '\0':
            return true;
        default:
            return false;
        }
    }
    return true;
}

// Reads a line from file, returns malloc'd buffer or NULL (and sets
// errno) on error.
static char *read_line(FILE *file, bool prompt, int *lineno)
{
    char *line;
    size_t len;

    line = NULL;
    len = 0;
    if (lineno != NULL)
        *lineno = 0;

    do
    {
        if (line)
        {
            free(line);
            line = NULL;
        }
        if (prompt)
            print_prompt();
        if (getline(&line, &len, file) == -1)
            return NULL;
        if (lineno != NULL)
            (*lineno)++;
    } while (comment_or_blank(line, len));

    return line;
}

static char *get_line(const char *progname, const char *filename, int *lineno)
{
    FILE *f;
    char *line;

    f = fopen(filename, "r");
    if (f == NULL)
    {
        // Cheating: could use argv[0] here, but...meh. Also, the
        // colon is supposed to go before [Errno 2] but well spotted
        // if anyone catches that.

        fprintf(stderr, "%s: can't open file '%s': [Errno %d] %s\n",
                progname, filename, errno, strerror(errno));
        return NULL;
    }
    line = read_line(f, false, lineno);
    fclose(f);
    return line;
}

static void print_error(char *filename, int lineno, char *errline)
{
    printf("  File \"%s\", line %d\n"
           "    %s\n"
           "    ^\n"
           "SyntaxError: invalid syntax\n", filename, lineno, errline);
}

static void print_version(char *name)
{
    if (strstr(name, "python3") != NULL)
        printf(PYTHON37);
    else
        printf(PYTHON27);
}

int main(int argc, char *argv[])
{
    char *filename;
    char *errline;
    int lineno;

    if (argc > 1)
    {
        filename = argv[1];
        errline = get_line(argv[0], filename, &lineno);
        if (errline != NULL)
            print_error(filename, lineno, errline);
    }
    else
    {
        filename = "<stdin>";
        lineno = 1;
        print_version(argv[0]);
        while ((errline = read_line(stdin, true, NULL)) != NULL)
        {
            print_error(filename, lineno, errline);
        }
    }

    free(errline);
    return 0;
}
