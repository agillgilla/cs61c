#ifndef _PHILPHIX_H
#define _PHILPHIX_H

#include <stdio.h>

/* The pointer to store the dictionary for replacement entries */
extern struct HashTable *dictionary;


extern unsigned int stringHash(void *string);

extern int stringEquals(void *s1, void *s2);

extern void readDictionary(char *dictName);

extern int readNextPair(FILE* inputFile);

extern void processInput();

extern int processNextWord();

#endif
