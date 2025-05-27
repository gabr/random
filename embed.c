///usr/bin/env cc "$0" -o a.out && ./a.out -- "$@"; exit
/*
    2025.05.27 (Tuesday)

    Demonstration on how to embed files within your executable and have easy
    access to their data and size.  This macro in various shapes and forms
    circles around the internet but this is my version which conveniently
    calculates the size of the data and does null termination at the same time.

    As an example here we embed the source code itself and print it out.

    A honorable mention is this more complete solution:
    https://github.com/graphitemaster/incbin
    which might be interesting if you plan on doing that cross platform.

    Also take a look at this article with different techniques:
    https://elm-chan.org/junk/32bit/binclude.html
    The other methods are worth knowing but are less convenient
    and the examples do not work without modifications.
*/
#include <stdio.h>
#include <stddef.h>

#define EMBED(file, name)                 \
asm (                                     \
    ".section .rodata \n"                 \
    ".balign 4\n"                         \
    ".global " #name "\n"                 \
    #name ":\n"                           \
    ".incbin \"" file "\"\n"              \
    #name "_end:\n"                       \
    ".byte 0\n"  /* null termination */   \
    ".global " #name "_siz\n"             \
    #name "_siz:\n"                       \
    ".quad " #name "_end - " #name "-1\n" \
    ".balign 4\n"                         \
    /* restore section */                 \
    ".section .text\n"                    \
);                                        \
extern const char name[];                 \
extern const size_t name ## _siz;         \

// embed itself as a demonstration
EMBED("embedfile.c", selfbytes);

int main(void) {
    size_t i;
    for (i = 0; i < selfbytes_siz; i++) putchar(selfbytes[i]);
    // you can also rely on the null termination:
    //for (i = 0; selfbytes[i]; i++) putchar(selfbytes[i]);
    return 0;
}
