#ifndef TLT_H
#define TLT_H

#include <ctype.h>

inline bool isLetter(char ch) {
    return bool(isalpha(int(ch)));
}

inline bool isDigit(char ch) {
    return bool(isdigit(int(ch)));
}

inline bool isSymbol(char ch, int const sym) {
    return bool(int(ch)==sym);
}

#endif // TLT_H
