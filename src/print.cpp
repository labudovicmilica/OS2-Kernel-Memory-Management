//
// Created by os on 8/11/23.
//

#include "../h/print.hpp"
#include "../lib/console.h"

void printf(const char *string) {
    while(*string != '\0'){
        __putc(*string);
        string++;
    }
}

void printf(const char *string, long x) {

    while(*string != '\0'){
        if (*string == '%') {
            string++;
            if(*string == 'd') {
                printInt(x);
            } else {
                __putc('%');
                __putc(*string);

            }
            string++;
        } else{
            __putc(*string);
            string++;
        }
    }
}

void printInt(long x) {
    char digits[] = "0123456789";

    char buf[16];
    int i = 0;

    do{
        buf[i++] = digits[x % 10];
    }while((x /= 10) != 0);

    while(--i >= 0)
        __putc(buf[i]);

}