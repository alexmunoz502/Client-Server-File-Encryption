#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
* A key generator used for one-time-pad encryption.
* allows user to generate key of specified length.
*/

// Constants
const int NUM_CHARS = 27; // A-Z + Space
const int ASCII_ALPHA_START = 65;
const int ASCII_ALPHA_END = 90;
const int ASCII_SPACE = 32;

// Use time-based random number generation to avoid same numbers each run
void generate_new_seed(void)
{
    time_t t;
    srand((unsigned) time(&t));
}

// Get a random alpha character, includes the space character
char get_random_char(void)
{
    char random_char = ASCII_ALPHA_START;
    random_char += rand() % NUM_CHARS;
    if (random_char > ASCII_ALPHA_END)
        random_char = ASCII_SPACE;
    return random_char;
}

// Create a key using random characters
char* generate_key(int length)
{
    char* key = calloc(length + 1, sizeof(char));
    for (int i = 0; i < length; i++)
    {
        key[i] = get_random_char();
    }
    return key;
}

// Key generator, first argument is the length of the key to generate
int main(int argc, char *argv[]) {

    int length = 0;
    char* key = NULL;

    // Check for supplied length argument
    if (argc > 1)
    {
        // Convert length arg from string to integer
        length = atoi(argv[1]);

        // Check if length is a valid, non-zero integer
        if (length <= 0)
        {
            fprintf(stderr, "ERROR: Invalid Key Length\n");
        }
        else
        {
            // Key Generated to stdout
            generate_new_seed();
            key = generate_key(length);
            printf("%s\n", key);
            free(key);
        }   
    }
    else
    {
        fprintf(stderr, "ERROR: Missing Key Length\n");
    }
  return 0;
}