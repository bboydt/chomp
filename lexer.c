#include "chomp.h"

#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>

#include <stdio.h>

void add_token(struct token *token, struct token_bucket *bucket)
{
    if (bucket->count == bucket->capacity)
    {
        bucket->capacity *= 2;
        bucket->arr = (struct token *)realloc(bucket->arr, bucket->capacity * sizeof(struct token));
    }
    
    bucket->arr[bucket->count++] = *token;
}

static void prepare_source(struct source_file *source)
{
    // map file into memory
    source->data = (const uint8_t *)mmap(NULL, source->size, PROT_READ, MAP_SHARED, source->fd, 0);

    if ((intptr_t)source->data == -1)
    {
        print_error("Failed to map '%s' into memory.", source->path);
        exit(-1);
    }

    // init token bucket
    struct token_bucket *tokens = &source->tokens;
    tokens->capacity = 2;
    tokens->arr = (struct token *)malloc(tokens->capacity * sizeof(struct token));
}

static void cleanup_source(struct source_file *source)
{
}

enum lexer_state
{
    LEXER_READING_NOTHING,
    LEXER_READING_ALPHANUMERIC,
    LEXER_READING_NUMERIC,
    LEXER_READING_DOUBLE_QUOTES,
    LEXER_READING_SINGLE_QUOTES,
    LEXER_READING_SINGLE_LINE_COMMENT,
    LEXER_READING_MULTI_LINE_COMMENT,
};

static bool is_alpha(uint8_t c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static bool is_numeric(uint8_t c)
{
    return ('0' <= c && c <= '9');
}

static bool is_alpha_numeric(uint8_t c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_';
}

#define CHAR_TOKEN(c)\
    if (*cur == c)\
    {\
        token.type = c;\
        token.offs = (int64_t)(cur - source->data);\
        token.length = 1;\
        add_token(&token, &source->tokens);\
        cur++;\
    }


void produce_tokens(struct source_file *source)
{
    prepare_source(source);
    
    const uint8_t *cur = source->data;
    enum lexer_state state = LEXER_READING_NOTHING;
    struct token token;
    bool escaped = false;
    while (cur < source->data + source->size)
    {
        uint8_t pushed_cur = *cur;

        switch (state)
        {
            case LEXER_READING_NOTHING:
                CHAR_TOKEN(';')
                else CHAR_TOKEN(',')
                else CHAR_TOKEN('.')
                else CHAR_TOKEN('(')
                else CHAR_TOKEN(')')
                else CHAR_TOKEN('{')
                else CHAR_TOKEN('}')
                else CHAR_TOKEN('[')
                else CHAR_TOKEN(']')
                else CHAR_TOKEN('>')
                else CHAR_TOKEN('<')
                else CHAR_TOKEN('+')
                else CHAR_TOKEN('-')
                else CHAR_TOKEN('*')
                else CHAR_TOKEN('&')
                else CHAR_TOKEN('|')
                else CHAR_TOKEN('/')
                else CHAR_TOKEN('^')
                else CHAR_TOKEN('%')
                else CHAR_TOKEN(':')
                else CHAR_TOKEN('!')
                else CHAR_TOKEN('=')
                else CHAR_TOKEN('#')
                else if (*cur == '"')
                {
                    cur++;
                    state = LEXER_READING_DOUBLE_QUOTES;
                    token.type = TOKEN_TYPE_UNKNOWN;
                    token.offs = (int64_t)(cur - source->data);
                    token.length = 0;
                }
                else if (*cur == '\'')
                {
                    cur++;
                    state = LEXER_READING_SINGLE_QUOTES;
                    token.type = TOKEN_TYPE_UNKNOWN;
                    token.offs = (int64_t)(cur - source->data);
                    token.length = 0;
                }
                else if (is_alpha(*cur))
                {
                    state = LEXER_READING_ALPHANUMERIC;
                    token.type = TOKEN_TYPE_UNKNOWN;
                    token.offs = (int64_t)(cur - source->data);
                    token.length = 1;
                    cur++;
                }
                else if (is_numeric(*cur))
                {
                    state = LEXER_READING_NUMERIC;
                    token.type = TOKEN_TYPE_UNKNOWN;
                    token.offs = (int64_t)(cur - source->data);
                    token.length = 1;
                    cur++;
                }
                else
                {
                    cur++;
                }
                break;

            case LEXER_READING_ALPHANUMERIC:
                if (is_alpha_numeric(*cur))
                {
                    token.length++;
                    cur++;
                }
                else
                {
                    add_token(&token, &source->tokens);
                    state = LEXER_READING_NOTHING;
                }
                break;

            case LEXER_READING_NUMERIC:
                if (is_alpha_numeric(*cur))
                {
                    token.length++;
                    cur++;
                }
                else
                {
                    add_token(&token, &source->tokens);
                    state = LEXER_READING_NOTHING;
                }
                break;

            case LEXER_READING_DOUBLE_QUOTES:
                if (*cur != '"' || escaped)
                {
                    token.length++;
                    cur++;
                }
                else
                {
                    cur++;
                    add_token(&token, &source->tokens);
                    state = LEXER_READING_NOTHING;
                }
                break;

            case LEXER_READING_SINGLE_QUOTES:
                if (*cur != '\'' || escaped)
                {
                    token.length++;
                    cur++;
                }
                else
                {
                    cur++;
                    add_token(&token, &source->tokens);
                    state = LEXER_READING_NOTHING;
                }
                break;
        }

        escaped = (pushed_cur == '\\');
    }



    cleanup_source(source);
}
