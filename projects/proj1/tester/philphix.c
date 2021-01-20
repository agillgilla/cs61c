
#include "hashtable.h"
#include "philphix.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

HashTable *dictionary;

int main(int argc, char **argv){
  if(argc != 2){
    /* Note, you can print everything you want to standard error, it will be
       ignored by the autograder */
    fprintf(stderr, "Specify a dictionary\n");
    return 0;
  }
  fprintf(stderr, "Creating hashtable\n");
  dictionary = createHashTable(2255, &stringHash, &stringEquals);
  fprintf(stderr, "Loading dictionary %s\n", argv[1]);
  readDictionary(argv[1]);
  fprintf(stderr, "Dictionary loaded\n");
  fprintf(stderr, "Processing stdin\n");
  processInput();
  return 0;
}

/* Define a hash function */
unsigned int stringHash(void *s){
  char *string = (void *) s;
  /* Printing string is to prevent a compiler warning until you
     actually implement this function 
  fprintf(stderr, "need to implement stringHash%s\n", string);*/

  unsigned int hash = 5381;
  int c;

  while ((c = *string++)) {
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
    
  return hash;

  /*CITE: Hash function by Daniel Bernstein, found here: http://www.cse.yorku.ca/~oz/hash.html*/

  /* To suppress compiler warning until you implement this function, */
  /*return 0;*/
}


int stringEquals(void *s1, void *s2){
  /*fprintf(stderr, "You need to implement stringEquals");*/
  /* To suppress compiler warning until you implement this function */
  char *str1 = (void *) s1;
  char *str2 = (void *) s2;
  int diff;

  diff = strcmp(str1, str2);

  if (diff == 0) { /* The strings are equal. */
    return 1;
  } else { /* The strings are not equal. */
    return 0;
  }

}

void readDictionary(char *name){
  FILE *inputFile = fopen(name, "r");

  if (inputFile == NULL) {
    fprintf(stderr, "Error opening dictionary file.");
    return;
  }

  while (readNextPair(inputFile) != 0) {
    fprintf(stderr, "\n");
    fprintf(stderr, "NEW LINE CALL");
    fprintf(stderr, "\n");
  }
  fclose(inputFile);
  /* Printing the address of inputFile is to suppress compiler warning
     until you implement this function */
  /*fprintf(stderr, "You need to implement readDictionary %x\n", (unsigned int) inputFile);*/

}

int readNextPair(FILE *inputFile) { /* Read next key, value pair and enter into hashtable.  Return -1 if ERROR, 0 if EOF, and 1 if successful */
  /*FILE *inputFile = fopen(name, "r");*/
  char *key = NULL;
  char *value = NULL;

  const int BUFF_LEN = 2;
  int large_buff_len = 16;

  int curr_buff_pos = 0;

  char charbuff[BUFF_LEN];
  char *large_charbuff = malloc(sizeof(char) * large_buff_len);

  int task = 0; /*Different tasks.  0 = looking for key, 1 = parsing whitespace, 2 = looking for value 3 = found key and value, done*/

  char *status;
  while (task != 3) {
    status = fgets(charbuff, BUFF_LEN, inputFile);

    if (status == NULL) {
      if (task == 2) {
        value = malloc(sizeof(char) * (curr_buff_pos + 1));
        strncpy(value, large_charbuff, curr_buff_pos + 1);
        value[curr_buff_pos] = '\0';
        /*fprintf(stderr, "VALUE: ");
        fprintf(stderr, value);
        fprintf(stderr, "\n");*/
        /*TODO: BUILD VALUE STRING AND ENTER PAIR INTO DICTIONARY*/
        task++;
      }
      fprintf(stderr, "End of File");
      fprintf(stderr, "\n");
      return 0;
    } else {
      if (task == 0) {
        if (isalnum(charbuff[0])) {
          large_charbuff[curr_buff_pos] = charbuff[0];
          
          if (curr_buff_pos >= large_buff_len - 2) { 
            char *tmp = realloc(large_charbuff, sizeof(char) * large_buff_len * 2);
            if (tmp != NULL) {
                large_buff_len = large_buff_len * 2;
                large_charbuff = tmp;
            } else {
              fprintf(stderr, "Error allocating memory for character buffer.  Aborting...");
              exit(-1);
            }
          }
          
          /*fprintf(stderr, charbuff);
          fprintf(stderr, "\n");*/
          curr_buff_pos++;
        } else if (charbuff[0] == ' ') {
          if (curr_buff_pos == 0) {
            fprintf(stderr, "Unexpected space before key. Aborting...");
            exit(-1);
          }
          /*TODO: BUILD KEY STRING*/
          key = malloc(sizeof(char) * (curr_buff_pos + 1));
          strncpy(key, large_charbuff, curr_buff_pos + 1);
          key[curr_buff_pos] = '\0';
          /*fprintf(stderr, "KEY: ");
          fprintf(stderr, key);
          fprintf(stderr, "\n");*/
          
          curr_buff_pos = 0;
          task++;
        } else {
          fprintf(stderr, "Unexpected character.  Aborting...");
          exit(-1);
        }
      } else if (task == 1) {
        if (charbuff[0] == ' ') {
          /* ITERATE THROUGH WHITESPACE */         
        } else {
          if (charbuff[0] == '\n') {
            fprintf(stderr, "Unexpected newline before value.  Aborting...");
            exit(-1);
          } else {
            /*fprintf(stderr, charbuff);
            fprintf(stderr, "\n");*/
            large_charbuff[curr_buff_pos] = charbuff[0];
            
            if (curr_buff_pos >= large_buff_len - 2) { 
              char *tmp = realloc(large_charbuff, sizeof(char) * large_buff_len * 2);
              if (tmp != NULL) {
                large_buff_len = large_buff_len * 2;
                large_charbuff = tmp;
              } else {
                fprintf(stderr, "Error allocating memory for character buffer.  Aborting...");
                exit(-1);
              }
            }
            
            curr_buff_pos++;
            task++;
          }
        } 

      } else if (task == 2) {
        if (charbuff[0] == '\n') {
          value = malloc(sizeof(char) * (curr_buff_pos + 1));
          strncpy(value, large_charbuff, curr_buff_pos + 1);
          value[curr_buff_pos] = '\0';
          /*fprintf(stderr, "VALUE: ");
          fprintf(stderr, value);
          fprintf(stderr, "\n");*/
          /*TODO: BUILD VALUE STRING AND ENTER PAIR INTO DICTIONARY*/
          task++;
        } else if (charbuff[0] == ' ') {
          fprintf(stderr, "Unexpected whitespace in value.  Aborting...");
          exit(-1);
        } else {
          large_charbuff[curr_buff_pos] = charbuff[0];
          
          if (curr_buff_pos >= large_buff_len - 2) { 
            char *tmp = realloc(large_charbuff, sizeof(char) * large_buff_len * 2);
            if (tmp != NULL) {
              large_buff_len = large_buff_len * 2;
              large_charbuff = tmp;
            } else {
              fprintf(stderr, "Error allocating memory for character buffer.  Aborting...");
              exit(-1);
            }
          }
          
          /*fprintf(stderr, charbuff);
          fprintf(stderr, "\n");*/
          curr_buff_pos++;
        }
      } else {
        fprintf(stderr, "Unrecognized Task.  Aborting readNextPair...");
        return -1;
      }
    }

  }
  /*fprintf(stderr, "PRINTING KEY:");
  fprintf(stderr, key);
  fprintf(stderr, "\n");
  fprintf(stderr, "PRINTING VALUE:");
  fprintf(stderr, value);*/
  insertData(dictionary, key, value);

  free(large_charbuff);

  return 1;
}


void processInput() {
  fprintf(stderr, "OUTPUT:");
  fprintf(stderr, "\n");
  while (processNextWord() != 0) {
      /*fprintf(stderr, "PROCCESSED A WORD/CHAR");
      fprintf(stderr, "\n");*/
  }
}

int processNextWord() { /* Read next word and handle according to replace rules.  Return -1 if ERROR, 0 if EOF, and 1 if successful */
  const int BUFF_LEN = 2;
  int large_buff_len = 16;

  int curr_buff_pos = 0;

  char charbuff[BUFF_LEN];
  char *large_charbuff = malloc(sizeof(char) * large_buff_len);

  char *status = fgets(charbuff, BUFF_LEN, stdin);

  if (status == NULL) { /* End of File. */
    return 0;
  }

  if (!isalnum(charbuff[0])) { /* Output non alphanumeric chars to stdout immediately. */
    fprintf(stdout, charbuff);
    return 1;
  } else { /* Begin building word. */
      large_charbuff[curr_buff_pos] = charbuff[0];
      curr_buff_pos++;

      while (status != NULL && isalnum(charbuff[0])) {
        status = fgets(charbuff, BUFF_LEN, stdin);

        large_charbuff[curr_buff_pos] = charbuff[0];
          
        if (curr_buff_pos >= large_buff_len - 2) { 
          char *tmp = realloc(large_charbuff, sizeof(char) * large_buff_len * 2);
          if (tmp != NULL) {
              large_buff_len = large_buff_len * 2;
              large_charbuff = tmp;
          } else {
            fprintf(stderr, "Error allocating memory for character buffer.  Aborting...");
            exit(-1);
          }
        }

        curr_buff_pos++;
      }

      curr_buff_pos--;

      char *word = malloc(sizeof(char) * (curr_buff_pos + 1));
      strncpy(word, large_charbuff, curr_buff_pos + 1);
      char *word_original = malloc(sizeof(char) * (curr_buff_pos + 1));
      strncpy(word_original, large_charbuff, curr_buff_pos + 1);
      word[curr_buff_pos] = '\0';
      word_original[curr_buff_pos] = '\0';

      free(large_charbuff);

      if (findData(dictionary, word) != NULL) { /* Immediate match */
        fprintf(stdout, findData(dictionary, word));
      } else { /* Check word with all but first converted to lowercase */
        size_t i;

        for(i = 1; i < curr_buff_pos; i++) {
          word[i] = tolower(word[i]);
        }

        if (findData(dictionary, word) != NULL) { /* Match after first change */
          fprintf(stdout, findData(dictionary, word));
        } else { /* Check word entirely lowercase */
          word[0] = tolower(word[0]);

          if (findData(dictionary, word) != NULL) { /* Match after second change */
            fprintf(stdout, findData(dictionary, word));
          } else { /* No matches */
            /* PRINT ORIGINAL! */
            fprintf(stdout, word_original);
            free(word_original);
          }
        }
      }


      free(word);

      if (status == NULL) { /* End of File. */
        return 0;
      } else if (!isalnum(charbuff[0])) {
        fprintf(stdout, charbuff); /* Print the extra non-alhpanumeric char we found. */
        return 1;
      } else {
        fprintf(stderr, "Unrecognized input error.  Aborting...");
        exit(-1);
      }
  }

}
