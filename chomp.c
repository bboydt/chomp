#include "chomp.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

struct source_file *sources_head = NULL;

static void add_source(const char *path)
{
    struct source_file *new_source = (struct source_file *)malloc(sizeof(struct source_file));

    memset(new_source, 0, sizeof(struct source_file));
    
    new_source->path = path;

    // add to source list
    new_source->next = sources_head;
    sources_head = new_source;
}

int main(int argc, const char **argv)
{
    printf("(chomp) " CH_VERSION "\n");

    for (int i = 1; i < argc; i++)
    {
        add_source(argv[i]);
    }

    printf("sources:");
    for (struct source_file *source = sources_head; source != NULL; source = source->next)
    {
        printf(" %s", source->path);
    }
    printf("\n");

    for (struct source_file *source = sources_head; source != NULL; source = source->next)
    {
        source->fd = open(source->path, O_RDONLY);
        if (source->fd == -1)
        {
            print_error("Failed to open '%s'.", source->path);
            exit(-1);
        }

        struct stat st;
        if (fstat(source->fd, &st) == -1)
        {
            print_error("Failed to fstat '%s'.", source->path);
            exit(-1);
        }

        source->size = st.st_size;
    }

    // produce tokens
    for (struct source_file *source = sources_head; source != NULL; source = source->next)
    {
        produce_tokens(source);
        printf("Found %d tokens in '%s'.\n", source->tokens.count, source->path);
    
        static int colors[2] = { 1, 0 };

        int i = 0;
        for (struct token *token = source->tokens.arr; token < source->tokens.arr + source->tokens.count; token++)
        {
            printf("'\e[%d;32m%.*s\e[0m'\n", colors[(i++) % 2], token->length, token->offs + source->data);
        }
    }
    

    return 0;
}

void print_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "\e[0;31m(error) ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\e[0m\n");

    va_end(args);
}

void print_warn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "\e[0;33m(warn) ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\e[0m\n");

    va_end(args);
}
