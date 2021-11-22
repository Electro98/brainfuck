//
// Created by electro98 on 13/11/2021.
//

#ifndef BRAINFUCK_BRAINFUCK_H
#define BRAINFUCK_BRAINFUCK_H

#include <stdlib.h>
#include "mystr.h"

typedef unsigned char byte_t;

typedef struct {
    size_t buff_size;
    size_t ptr;
    size_t *buff;
} arr_stack_t;

typedef struct {
    size_t mem_size;
    size_t mem_ptr;
    byte_t *memory;
    FILE *dup_output;
} env_t;

typedef enum { PS, MS, LT, BT, COMMA, DOT, LB, RB} bf_operations_t;

typedef struct {
    size_t len;
    bf_operations_t *tokens;
} bf_code_t;

#define op_char(op) CH_##op
#define CH_PS '+'
#define CH_MS '-'
#define CH_LT '<'
#define CH_BT '>'
#define CH_COMMA ','
#define CH_DOT '.'
#define CH_LB '['
#define CH_RB ']'

typedef enum {
    INC,   // '+' 1
    DEC,   // '-' 1
    PLUS,  // '+' NUM
    MINUS, // '-' NUM
    P_INC, // '>' 1
    P_DEC, // '<' 1
    SHR,   // '>' NUM
    SHL,   // '<' NUM
    IN,    // ','
    OUT,   // '.'
    WHILE, // '['
    END,   // ']'
    CLEAR, // "[-]"
} opt_operations_t;

typedef struct {
    opt_operations_t op;
    size_t value;
} opt_token_t;

typedef struct {
    size_t len;
    opt_token_t *tokens;
} opt_code_t;

size_t count_bf_ops(FILE *input);
char *get_bf_code_str(FILE *source);
void print_opt_code(opt_code_t code);
opt_code_t optimize_code(const char *code);
int interpret_opt_code(env_t env, opt_code_t code);
int compile_opt_code(env_t env, opt_code_t code, FILE *out);

#endif //BRAINFUCK_BRAINFUCK_H
