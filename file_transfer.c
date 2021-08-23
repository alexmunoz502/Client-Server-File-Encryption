#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "file_transfer.h"

/* 
* Helper functions for transferring data over sockets
*/

// Constants
const int STOP = 0;
const int READING = 1;
const int WRITING = 2;

// Error function used for reporting issues within file transfer
void ft_error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
} 

// Finds the end of file and returns the position as length
int get_file_length(char* filename)
{
    FILE *file = fopen(filename, "r");
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fclose(file);
    return length;
}

// Dynamic File Send 
void socket_file_send(int socket_fd, char* filename, int length)
{
    int status = READING;
    int file = open(filename, 'r');
    int num_to_read = 0;
    int num_read = 0;
    int num_to_write = 0;
    int num_written = 0;
    char buffer[100000] = "\0";
    char* position = NULL;

    num_to_read = length;
    while(num_to_read > 0)
    {
        num_read = read(file, buffer, sizeof(buffer));
        if (num_read < 0)
        {
            ft_error("FILE READ ERROR\n");
        }
        else if (num_read == 0)
        {
            break;
        }
        num_to_read -= num_read;
    }
    num_to_write = length;
    position = buffer;
    while(num_to_write > 0)
    {
        num_written = write(socket_fd, position, num_to_write);
        if (num_written < 0)
        {
            ft_error("FILE WRITE ERROR");
        }
        else if (num_written == 0)
        {
            break;
        }
        position += num_written;
        num_to_write -= num_written;
    }
    close(file);
}

// Dynamic File Recv
void socket_file_recv(int socket_fd, char* buffer, int length)
{
    // Local Variables
    int num_to_read = 0;
    int num_read = 0;
    char* position = NULL;

    // Set Variables
    num_to_read = length;
    position = buffer;

    // Read message from socket FD
    while (READING)
    {
        num_read = read(socket_fd, position, num_to_read);
        if (num_read < 0)
        {
            ft_error("ERROR Reading from socket");
        }
        if (num_read == 0)
        {
            break;
        }
        position += num_read;
        num_to_read -= num_read;
    }
}