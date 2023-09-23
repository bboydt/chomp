#pragma once

#include <stddef.h>
#include <stdint.h>

/*
 * Defs
 */

#define CH_VERSION "2023.09.23-dev"

/* 
 * Helper Macros
 */

#define __CH_STR(x) #x
#define CH_STR(x) __CH_STR(x)

/*
 * Types
 */

struct source_file;
struct token;
enum token_type;
struct token_bucket;

enum token_type
{
    TOKEN_TYPE_UNKNOWN = 0,
    TOKEN_TYPE_IDENT = 0x80,
};

struct token
{
    // @note we'll need to string together tokens of great length

    int8_t type;
    int64_t offs;
    int16_t length;
};

struct token_bucket
{
    struct token *arr;
    uint32_t capacity;
    uint32_t count;
};

struct source_file
{
    const char *path;
    int fd;
    size_t size;
    const uint8_t *data;

    struct token_bucket tokens;

    struct source_file *next;
};

/*
 * Functions
 */

/* Interface */
void print_error(const char *fmt, ...);
void print_warn(const char *fmt, ...);

/* Lexer */
void add_token(struct token *token, struct token_bucket *bucket);
void produce_tokens(struct source_file *source);

/*
 * Globals
 */

extern struct source_file *sources_head;

