#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "otp_encryption.h"

/*
* Helper functions for encrypting and decrypting files
*/

// Global Constants
const int NUM_CHARS = 27;
const int SPACE_VALUE = 26;
const int ASCII_ALPHA_START = 65;
const int ASCII_SPACE_LESS_ALPHA_START = -33;

char get_char_alpha_value(char character)
{
    // Converts an ASCII code to an alpha value
    char alpha_value = 0;
    alpha_value = character - ASCII_ALPHA_START;
    if (alpha_value == ASCII_SPACE_LESS_ALPHA_START)
        alpha_value = SPACE_VALUE;
    return alpha_value;
}

char* encrypt(char* plaintext, char* key)
{
    // Local Variables
    char key_char = 0;
    char pt_char = 0;
    char ct_char = 0;
    char* cyphertext = NULL;
    size_t length = 0;
    int has_error = 0;

    // Encryption Initialization
    length = strlen(plaintext)-1;
    cyphertext = calloc(length + 1, sizeof(char));

    // Encrypt Plaintext
    for (int i = 0; i < length; i++)
    {
        if (plaintext[i] == '\n')
            break;
        // Get alpha values of plain char and key char
        pt_char = get_char_alpha_value(plaintext[i]);
        key_char = get_char_alpha_value(key[i]);

        // Check for invalid characters to encrypt
        if (pt_char < 0 || pt_char > SPACE_VALUE)
            return NULL;

        // Encrypt the plain char using the key
        ct_char = pt_char + key_char;
        if (ct_char > NUM_CHARS)
            ct_char -= NUM_CHARS;

        if (ct_char == SPACE_VALUE)
            ct_char = ASCII_SPACE_LESS_ALPHA_START;

        // Convert back to ASCII code & add to cyphertext
        ct_char += ASCII_ALPHA_START;
        cyphertext[i] = ct_char;
    }
    return cyphertext;
}

char* decrypt(char* cyphertext, char* key)
{
    // Local Variables
    char key_char = 0;
    char pt_char = 0;
    char ct_char = 0;
    char* plaintext = NULL;
    size_t length = 0;

    // Decryption Initialization
    length = strlen(cyphertext);
    plaintext = calloc(length + 1, sizeof(char));

    // Decrypt Plaintext
    for (int i = 0; i < length; i++)
    {
        if (cyphertext[i] == '\n')
            break;

        // Get alpha values of cypher char and key char
        ct_char = get_char_alpha_value(cyphertext[i]);
        key_char = get_char_alpha_value(key[i]);

        // Decrypt the cyper char using the key
        pt_char = ct_char - key_char;
        if (pt_char < 0)
            pt_char += NUM_CHARS;
        
        if (pt_char == SPACE_VALUE)
            pt_char = ASCII_SPACE_LESS_ALPHA_START;

        // Convert back to ASCII code & add to plaintext
        pt_char += ASCII_ALPHA_START;
        plaintext[i] = pt_char;
    }   
    return plaintext;
}