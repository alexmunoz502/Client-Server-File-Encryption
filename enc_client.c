#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "file_transfer.h"

/* 
* Encryption Client
* Sends an plaintext file and key to the encryption server
* and receives back the encrypted file, which is outputted to
* stdout
*/

// Constants
const int YES = 1;

// Error function used for reporting issues
void error(const char *msg) { 
    perror(msg); 
    exit(EXIT_FAILURE); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname)
{ 
    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 

    // The address should be network capable
    address->sin_family = AF_INET;

    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname); 
    if (hostInfo == NULL) { 
        fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
        exit(0); 
    }

    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

// Client
int main(int argc, char *argv[]) {

    // Local Variables
    int socketFD, portNumber, charsWritten, charsRead, plaintext_length, key_length;
    struct sockaddr_in serverAddress;
    char* plaintext_filename = NULL;
    char* key_filename = NULL;
    char buffer[100000];

    // Check usage & args
    if (argc < 3) 
    {
        fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); 
        exit(EXIT_FAILURE); 
    }

    // Assign plaintext and key pointers
    plaintext_filename = argv[1];
    key_filename = argv[2];

    // Get plaintext and key lengths, strip last char
    plaintext_length = get_file_length(plaintext_filename);
    key_length = get_file_length(key_filename);

    if (plaintext_length > key_length)
    {
        error("CLIENT: key is too short to encrypt plaintext");
    }

    // Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketFD < 0)
    {
        error("CLIENT: Unable to Open Socket");
    }

    // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");
    setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(int));

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
        error("CLIENT: Unable to Connect");
    }

    // Send Client Authentication Code
    char authentication_code[]="enc_client";
    write(socketFD, authentication_code, sizeof(authentication_code));

    // Receive Server Authentication Code
    read(socketFD, buffer, sizeof(buffer));
    if (strcmp(buffer, "enc_server") != 0) {
        error("CLIENT: Unable to Authenticate Server, Not of Type 'enc_server'\n");
    }

    // Send Read Length to Server
    key_length = plaintext_length; // only need to read up to plaintext length
    uint32_t n = plaintext_length;
    uint32_t tmp = htonl(n);
    write(socketFD, &tmp, sizeof(tmp));

    // write to server
    socket_file_send(socketFD, plaintext_filename, plaintext_length);
    socket_file_send(socketFD, key_filename, key_length);

    // Get return message from server
    memset(buffer, '\0', sizeof(buffer));
    socket_file_recv(socketFD, buffer, plaintext_length);

    // Output Cyphertext
    if (buffer[0] != '\0')
    {
        printf("%s\n", buffer);
        fflush(stdout);
    }

    // Close the socket
    close(socketFD); 
    return 0;
}