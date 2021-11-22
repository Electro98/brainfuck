//
// Created by electro98 on 13/11/2021.
//

#include "brainfuck.h"

void push(arr_stack_t *stack, size_t value) {
    stack->buff[stack->ptr] = value;
    if (stack->ptr+1 >= stack->buff_size)
        puts("STACK OVERFLOW: Generated opt code will work incorrect.");
    else
        ++stack->ptr;
}

size_t pop(arr_stack_t *stack) {
    if ((int) stack->ptr-1 < 0)
        puts("STACK UNDERFLOW: Generated opt code will work incorrect.");
    else
        --stack->ptr;
    return stack->buff[stack->ptr];
}

size_t count_bf_ops(FILE *input) {
    if (!input)
        return 0;
    size_t counter = 0;
    int buff;
    while ((buff = fgetc(input)) != EOF)
        switch ((char) buff) {
            case op_char(PS):
            case op_char(MS):
            case op_char(LT):
            case op_char(BT):
            case op_char(COMMA):
            case op_char(DOT):
            case op_char(LB):
            case op_char(RB):
                ++counter;
        }
    return counter;
}

char *get_bf_code_str(FILE *source) {
    if (!source)
        return 0;
    size_t str_size = count_bf_ops(source);
    char *code = malloc(str_size+1);
    code[str_size] = 0;
    fseek(source, 0, SEEK_SET);
    static char buff[256];
    for (size_t i = 0; i < str_size;) {
        fread(buff, 1, 256, source);
        for (int j = 0; j < 256 && i < str_size; ++j)
            switch (buff[j]) {
                case op_char(PS):
                case op_char(MS):
                case op_char(LT):
                case op_char(BT):
                case op_char(COMMA):
                case op_char(DOT):
                case op_char(LB):
                case op_char(RB):
                    code[i] = buff[j];
                    ++i;
            }
    }
    return code;
}

void print_opt_code(opt_code_t code) {
    for (size_t i = 0; i < code.len; ++i) {
        switch (code.tokens[i].op) {
            case INC:
                puts("OP INC");
                break;
            case DEC:
                puts("OP DEC");
                break;
            case PLUS:
                puts("OP PLUS");
                break;
            case MINUS:
                puts("OP MINUS");
                break;
            case P_INC:
                puts("OP P_INC");
                break;
            case P_DEC:
                puts("OP P_DEC");
                break;
            case SHR:
                puts("OP SHR");
                break;
            case SHL:
                puts("OP SHL");
                break;
            case IN:
                puts("OP IN");
                break;
            case OUT:
                puts("OP OUT");
                break;
            case WHILE:
                puts("OP WHILE");
                break;
            case END:
                puts("OP END");
                break;
            case CLEAR:
                puts("OP CLEAR");
                break;
        }
        printf(" - value = %zu\n", code.tokens[i].value);
    }
}

opt_code_t optimize_code(const char *code) {
    size_t code_len = strlen(code);
    size_t buff_for_stack[256];
    arr_stack_t op_stack = {
            .buff = buff_for_stack,
            .buff_size = sizeof(buff_for_stack) / sizeof(size_t),
            .ptr = 0,
    };
    opt_code_t opt_code = {
            .len = code_len / 4, //code_len
            .tokens = malloc(sizeof(opt_token_t) * opt_code.len),
    };
    size_t buff;
    size_t opt_op_ptr = 0;
    for (size_t i = 0; i < code_len;) {
        buff = count_same_char(code, i);
        switch (code[i]) {
            case op_char(PS):
                if (buff > 2) {
                    opt_code.tokens[opt_op_ptr].op = PLUS;
                    opt_code.tokens[opt_op_ptr++].value = buff;
                    i += buff;
                } else {
                    opt_code.tokens[opt_op_ptr++].op = INC;
                    ++i;
                }
                break;
            case op_char(MS):
                if (buff > 2) {
                    opt_code.tokens[opt_op_ptr].op = MINUS;
                    opt_code.tokens[opt_op_ptr++].value = buff;
                    i += buff;
                } else {
                    opt_code.tokens[opt_op_ptr++].op = DEC;
                    ++i;
                }
                break;
            case op_char(BT):
                if (buff > 2) {
                    opt_code.tokens[opt_op_ptr].op = SHR;
                    opt_code.tokens[opt_op_ptr++].value = buff;
                    i += buff;
                } else {
                    opt_code.tokens[opt_op_ptr++].op = P_INC;
                    ++i;
                }
                break;
            case op_char(LT):
                if (buff > 2) {
                    opt_code.tokens[opt_op_ptr].op = SHL;
                    opt_code.tokens[opt_op_ptr++].value = buff;
                    i += buff;
                } else {
                    opt_code.tokens[opt_op_ptr++].op = P_DEC;
                    ++i;
                }
                break;
            case op_char(COMMA):
                opt_code.tokens[opt_op_ptr].op = IN;
                opt_code.tokens[opt_op_ptr++].value = buff;
                i += buff;
                break;
            case op_char(DOT):
                opt_code.tokens[opt_op_ptr++].op = OUT;
                ++i;
                break;
            case op_char(LB):
                if (code[i+1] == '-' && code[i+2] == ']') {
                    opt_code.tokens[opt_op_ptr++].op = CLEAR;
                    i += 3;
                } else {
                    push(&op_stack, opt_op_ptr);
                    opt_code.tokens[opt_op_ptr++].op = WHILE;
                    ++i;
                }
                break;
            case op_char(RB):
                buff = pop(&op_stack);
                opt_code.tokens[buff].value = opt_op_ptr;
                opt_code.tokens[opt_op_ptr].op = END;
                opt_code.tokens[opt_op_ptr++].value = buff;
                ++i;
                break;
        }
        if (opt_op_ptr == opt_code.len) {
            opt_code.len += 200;
            opt_code.tokens = realloc(opt_code.tokens, sizeof(opt_token_t) * opt_code.len);
        }
    }
    opt_code.len = opt_op_ptr;
    opt_code.tokens = realloc(opt_code.tokens, sizeof(opt_token_t) * opt_code.len);
    return opt_code;
}

int interpret_opt_code(env_t env, opt_code_t code) {
    env.memory = calloc(sizeof(byte_t), env.mem_size);
    for (size_t i = 0; i < code.len;) {
        switch (code.tokens[i].op) {
            case INC:
                ++env.memory[env.mem_ptr];
                ++i;
                break;
            case DEC:
                --env.memory[env.mem_ptr];
                ++i;
                break;
            case PLUS:
                env.memory[env.mem_ptr] += code.tokens[i++].value;
                break;
            case MINUS:
                env.memory[env.mem_ptr] -= code.tokens[i++].value;
                break;
            case P_INC:
                ++env.mem_ptr;
                ++i;
                break;
            case P_DEC:
                --env.mem_ptr;
                ++i;
                break;
            case SHR:
                env.mem_ptr += code.tokens[i++].value;
                break;
            case SHL:
                env.mem_ptr -= code.tokens[i++].value;
                break;
            case IN:
                env.memory[env.mem_ptr] = getc(stdin);
                ++i;
                break;
            case OUT:
                fputc(env.memory[env.mem_ptr], stdout);
                if (env.dup_output)
                    fputc(env.memory[env.mem_ptr], env.dup_output);
                i++;
                break;
            case WHILE:
                if (!env.memory[env.mem_ptr])
                    i = code.tokens[i].value;
                else
                    ++i;
                break;
            case END:
                if (env.memory[env.mem_ptr])
                    i = code.tokens[i].value;
                else
                    ++i;
                break;
            case CLEAR:
                env.memory[env.mem_ptr] = 0;
                ++i;
                break;
        }
    }
    free(env.memory);
    return 0;
}

int compile_opt_code(env_t env, opt_code_t code, FILE *out) {
    fputs("%macro inc_num 0\n"
          "    inc byte [rsi]\n"
          "%endmacro\n"
          "%macro dec_num 0\n"
          "    dec byte [rsi]\n"
          "%endmacro\n"
          "%macro add_num 1\n"
          "    add byte [rsi], %1\n"
          "%endmacro\n"
          "%macro sub_num 1\n"
          "    sub byte [rsi], %1\n"
          "%endmacro\n"
          "%macro inc_p 0\n"
          "    inc rsi\n"
          "%endmacro\n"
          "%macro dec_p 0\n"
          "    dec rsi\n"
          "%endmacro\n"
          "%macro add_p 1\n"
          "    add rsi, %1\n"
          "%endmacro\n"
          "%macro sub_p 1\n"
          "    sub rsi, %1\n"
          "%endmacro\n"
          "%macro comma_command 0\n"
          "    mov rax, 0\n"
          "    syscall\n"
          "%endmacro\n"
          "%macro dot_command 0\n"
          "    call putch\n"
          "%endmacro\n"
          "%macro begin_loop 2\n"
          "    cmp byte [rsi], 0\n"
          "    je eloop%2\n"
          "    loop%1:\n"
          "%endmacro\n"
          "%macro end_loop 2\n"
          "    cmp byte [rsi], 0\n"
          "    jne loop%2\n"
          "    eloop%1:\n"
          "%endmacro\n"
          "%macro clean 0\n"
          "    mov byte [rsi], 0\n"
          "%endmacro\n\n", out);
    fprintf(out, "segment .data\n"
          "i:     db 0\n"
          "arr: times 128 db 0\n"
          "e:  times %zu db 0\n"
          "\n"
          "segment .text\n"
          "global _start\n"
          "_start:\n"
          "    ; ecx is my pointer\n"
          "    mov rsi, e\n"
          "    ; we need to prepare register to get/set one char\n"
          "    mov rdi, 1\n"
          "    mov rdx, 1\n", env.mem_size);
    for (size_t i = 0; i < code.len; ++i) {
        switch (code.tokens[i].op) {
            case INC:
                fputs("inc_num\n", out);
                break;
            case DEC:
                fputs("dec_num\n", out);
                break;
            case PLUS:
                fprintf(out, "add_num %zu\n", code.tokens[i].value);
                break;
            case MINUS:
                fprintf(out, "sub_num %zu\n", code.tokens[i].value);
                break;
            case P_INC:
                fputs("inc_p\n", out);
                break;
            case P_DEC:
                fputs("dec_p\n", out);
                break;
            case SHR:
                fprintf(out, "add_p %zu\n", code.tokens[i].value);
                break;
            case SHL:
                fprintf(out, "sub_p %zu\n", code.tokens[i].value);
                break;
            case IN:
                fputs("comma_command\n", out);
                break;
            case OUT:
                fputs("dot_command\n", out);
                break;
            case WHILE:
                fprintf(out, "begin_loop %zu, %zu\n", i, code.tokens[i].value);
                break;
            case END:
                fprintf(out, "end_loop %zu, %zu\n", i, code.tokens[i].value);
                break;
            case CLEAR:
                fputs("clean\n", out);
                break;
        }
    }
    fputs("    ; end line for end of program\n"
          "    mov byte [rsi], 10 ; \\n\n"
          "    dot_command\n"
          "\n"
          "    mov rax,60           \n"
          "    mov rdi,0            \n"
          "    syscall\n"
          "putch:\n"
          "    push rbp\n"
          "    mov rbp, rsp\n"
          "    xor rax, rax\n"
          "    mov al, byte [rsi]\n"
          "    mov byte [rbp-4], al\n"
          "    movzx eax, byte [i]\n"
          "    movzx edx, byte [rbp-4]\n"
          "    mov byte [arr + rax], dl\n"
          "    add eax, 1\n"
          "    mov byte [i], al\n"
          "    movzx eax, byte [i]\n"
          "    cmp al, 128\n"
          "    je .L2\n"
          "    cmp byte [rbp-4], 10\n"
          "    jne .L4\n"
          ".L2:\n"
          "    push rsi\n"
          "    movzx rdx, byte [i]\n"
          "    mov rsi, arr\n"
          "    mov rax, 1\n"
          "    syscall\n"
          "    mov byte [i], 0\n"
          "    pop rsi\n"
          ".L4:\n"
          "    nop\n"
          "    pop     rbp\n"
          "    ret", out);
    return 0;
}
