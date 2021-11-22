//
// Created by electro98 on 13/11/2021.
//

#ifndef BRAINFUCK_MYSTR_H
#define BRAINFUCK_MYSTR_H

#include "stdio.h"

size_t strlen(const char *str);
int rseek(const char *str, char ch);
size_t count_same_char(const char *str, size_t ptr);
void strcopy(char *dest, const char *source);
int strcomp(const char *str1, const char *str2);

#endif //BRAINFUCK_MYSTR_H
