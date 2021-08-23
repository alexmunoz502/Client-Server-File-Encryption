#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include "otp_encryption.h"
#include "file_transfer.h"

/* 
* Encryption Server
* Receives plaintext files and keys from encryption clients
* and sends back the encrypted files, can handle up to 5
* concurrent connections.
*/

// Constants
const int YES = 1;
const int RUNNING = 1;

// Error function used for reporting issues
void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber)
{
    
    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);
    // Allow a client at any address to connect to this server
    address->sin_addr.s_addr = INADDR_ANY;
}

// Server
int main(int argc, char *argv[])
{
    // Local Variables
    pid_t pid;
    int connectionSocket, charsRead, charsWritten, status;
    char buffer[100000];
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Check usage & args
    if (argc < 2) 
    { 
        fprintf(stderr,"USAGE: %s port\n", argv[0]); 
        exit(EXIT_FAILURE);
    } 
  
    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) 
    {
        error("ERROR opening socket");
    }
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(int));

    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));

    // Associate the socket to the port
    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        error("ERROR on binding");
    }

    // Start listening for connetions. Allow up to 5 connections at once
    listen(listenSocket, 5); 
    
    // Accept a connection, blocking if one is not available until one connects
    while(RUNNING)
    {
        // Accept the connection request which creates a connection socket
        connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
        if (connectionSocket < 0)
        {
        error("ERROR on accept");
        }
        
        // Fork and Create Child Process
        pid = fork();

        // Process Unsuccessful
        if (pid < 0)
        {
            error("ERROR on client process fork");
        }

        // Child Process
        if (pid == 0)
        {
            // Authenticate Client
            read(connectionSocket, buffer, sizeof(buffer)-1);
            if (strcmp(buffer, "enc_client") != 0) 
            {
                // Invalid Client
                char authentication_code[] = "";
                write(connectionSocket, authentication_code, sizeof(authentication_code));
                perror("SERVER: Unable to Authenticate Client, Not of Type 'enc_client'\n");
                _exit(EXIT_FAILURE);
            } 
            else
            {
                // Valid Client, send back server authentication code
                char authentication_code[] = "enc_server";
                write(connectionSocket, authentication_code, sizeof(authentication_code));
            }

            // Read Size of Plaintext
            uint32_t temp, plaintext_length;
            read(connectionSocket, &temp, sizeof(temp));
            plaintext_length = ntohl(temp);

            // Read File to be Encrypted
            char* plaintext = NULL;
            char* key = NULL;
            char* cyphertext = NULL;

            // Read the client's message from the socket (plaintext)
            memset(buffer, '\0', sizeof(buffer));
            socket_file_recv(connectionSocket, buffer, plaintext_length);
            plaintext = calloc(strlen(buffer)+1, sizeof(char));  
            strcpy(plaintext, buffer);

            // Read the client's message from the socket (key)
            memset(buffer, '\0', sizeof(buffer));
            socket_file_recv(connectionSocket, buffer, plaintext_length);
            key = calloc(strlen(buffer)+1, sizeof(char));
            strcpy(key, buffer);
            
            // Encrypt File
            memset(buffer, '\0', sizeof(buffer));
            cyphertext = encrypt(plaintext, key);
            if (cyphertext == NULL)
            {
                perror("SERVER: Invalid Character to Encrypt");
                _exit(EXIT_FAILURE);
            }
            strcpy(buffer, cyphertext);

            // Send cyphertext back to client
            int num_to_write = plaintext_length;
            char* position = buffer;
            while (num_to_write > 0)
            {
                charsWritten = write(connectionSocket, position, num_to_write); 
                if (charsWritten < 0)
                {
                    error("SERVER: ERROR writing cyphertext to client socket");
                }
                else if (charsWritten == 0)
                {
                    break;
                }
                position += charsWritten;
                num_to_write -= charsWritten;
            }
            free(plaintext);
            free(key);
            free(cyphertext);
        }
        // Close the connection socket for this client
        close(connectionSocket); 

        // Parent Process
        while (pid > 0)
        {
            pid = waitpid(-1, &status, WNOHANG);
        }
    }
    // Close the listening socket
    close(listenSocket);
    return EXIT_SUCCESS;
}
