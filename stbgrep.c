///usr/bin/env cc "$0" -o a.out && ./a.out -- "$@"; exit
/*
    2025.05.14 (Wednesday)

    My own attempt at writing a small C program that counts
    stb functions usages - inspired by:
    https://youtu.be/eAhWIO1Ra6M?si=gBvIX6GAxrXObqDY&t=3348

    My approach is different tho as I'm gonna try to use just the standard C
    library as (besides the fact that it has weird interfaces) I believe that
    it's quite capable and usable.  When writing a big project you would
    probably want to separate it into a platform layer - same way you would
    your operating system API - but for small one off programs it's fine.

    I'm also not dealing with opening and reading files here.  Including that
    into the program is not "the UNIX" way as that can be handled by the
    operating system.  It makes the program less flexible because then the data
    has to come from the files.  Without that the program is not only smaller
    and simpler but also can accept the data from anywhere - just pipe it in.
    Besides iterating over files in directories would have to be imlemented
    differently depending on the operating system - just a waste of time in
    a small program.

    Usage:
        cat ~/code/nothings/stb/stb*_*.h | ./stbgrep.c | sort -n

    To be fair.  It took me longer than Sean to write this - approx 1.5h
    But not because I had problems or bugs, but rather because the mindset in C
    is different and I don't do this often so I'm not that familiar with
    standard library functions that are useful when writing this type of code.

    I wonder if with practice I could improve to a point to make it reasonable
    efficient - let's say to get down to 5-15 mins for such programs.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <search.h>
#include <assert.h>

// NOTE(arek): The only thing that bothers me with the hash table approach is
// that it requires a constant size to be set up front and cannot be regrowth
// later.  For that reason only I should switch to simply doing a dynamic array
// with a quick sort at the end.  For now I have a condition to not overfloww.
#define HTSIZ (1024*4)

int main(void) {
    size_t i, ei = 0;
    char c;
    ENTRY *e;
    char *fname;
    char *entries[HTSIZ];
    char buf[BUFSIZ];
    hcreate(HTSIZ);
    while ((c = getchar()) != EOF) {
        if (!isalpha(c)) continue; // skip until a regular character is found
        // copy the found token name into the buffer but detect if that's a function name
        for (i = 0; i < BUFSIZ; i++) {
            buf[i] = c;
            c = getchar();
            if ((!isalnum(c) && c != '_')) {
                while (isspace(c)) c = getchar();
                if (c == EOF || c != '(') i = BUFSIZ; // check if it's a function
                break;
            }
        }
        if (++i >= BUFSIZ) continue; // too long or not a function
        buf[i] = 0;
        if (i < 3 || strncmp(buf, "stb", 3)) continue; // not an stb function
        // now put the function name into the hash table and incremente it's counter
        if (!(e = hsearch((ENTRY){buf}, FIND))) {
            if (ei >= HTSIZ) {
                if (ei == HTSIZ) {
                    fprintf(stderr, "Cannot fit any more entries at %zu\n", ei);
                    ei = -1; // don't print the message next time
                }
                continue;
            }
            fname = strdup(buf);
            assert(e = hsearch((ENTRY){fname,(void*)0}, ENTER));
            entries[ei++] = fname;
        }
        if ((size_t)(e->data) != -1) (size_t)(e->data)++;
    }
    // print out all the entries
    if (ei == -1) ei = HTSIZ;
    for (i = 0; i < ei; i++) {
        if (e = hsearch((ENTRY){entries[i]}, FIND))
            printf("%zu\t%s\n", e->data, e->key);
    }
    return 0;
}
