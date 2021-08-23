#ifndef SOCKET_FT_H
#define SOCKET_FT_H

int get_file_length(char* filename);
void socket_file_send(int socket_fd, char* filename, int length);
void socket_file_recv(int socket_fd, char* buffer, int length);

#endif
