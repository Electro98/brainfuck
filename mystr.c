//
// Created by electro98 on 13/11/2021.
//
#include "mystr.h"

size_t strlen(const char *str) {
    if (!str)
        return 0;
    for (size_t i = 0; ; ++i)
        if (!str[i])
            return i;
}

int rseek(const char *str, char ch) {
    for (int i = (int) strlen(str) - 1; i >= 0; --i)
        if (str[i] == ch)
            return i;
    return -1;
}

size_t count_same_char(const char *str, size_t ptr) {
    if (!str)
        return 0;
    char ch = str[ptr];
    for (size_t i = ptr + 1;; i++)
        if (ch != str[i])
            return i - ptr;
}

void strcopy(char *dest, const char *source) {
    // NOT SAVE! BE CAREFUL IN USE.
    if (!dest || !source)
        return;
    for (size_t i = 0; i <= strlen(source); ++i)
        dest[i] = source[i];
}

int strcomp(const char *str1, const char *str2) {
    if (!str1 || !str2 || (strlen(str1) != strlen(str2)))
        return 0;
    for (size_t i = 0; i < strlen(str1); ++i)
        if (str1[i] != str2[i])
            return 0;
    return 1;
}
