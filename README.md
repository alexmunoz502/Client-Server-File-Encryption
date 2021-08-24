# Client-Server File Encryption
##### A C program for encrypting and decrypting files over a socket connection.

#### --- ABOUT ---
This program was part of my operating system studies at Oregon State University. 
I learned how to utilize the linux socket library as well as the one-time-pad 
encryption method.  

For more information on this encryption method, please see the [WikiPedia](https://en.wikipedia.org/wiki/One-time_pad) page.  


#### --- SYSTEM REQUIREMENTS ---
The program runs on Linux-based operating systems.

#### --- COMPILING INSTRUCTIONS ---
Included is a compileall bash script which will compile the 5 programs in this
project.

#### --- FILES ---
While there are 5 executables generated, there are 7 files (excluding headers) 
for this project in total. Below is a description of each file and its purpose.

###### keygen.c
This program compiles to create the keygen executable, which generates a key of N randomly-
generated characters, where N is an argument supplied on execution. EXAMPLE:  
`./keygen 20 > key.txt`  
This command will utilize the keygen program to create a key of 20 characters 
and write it to a file called key.txt.  

###### otp_encryption.c
This file supplies helper functions to the encryption and decryption clients
and servers. The functions are used to encrypt and decrypt plaintext and 
cyphertext files.  

###### file_transfer.c
This file supplies helper functions to the encryption and decryption clients
and servers. The functions are used to send and receive text over a socket 
connection.

###### enc_client.c
This program compiles to create the enc_client executable, which sends a 
plaintext file and key to the encryption server and receives back the encrypted
file, which is outputted to stdout. EXAMPLE:  
`./enc_client plaintext.txt key.txt 5678 > cyphertext.txt`  
This command will utilize the enc_client program to send a plaintext file and 
key to the encryption server on port 5678 and redirect the cyptertext results 
to a file called cyphertext.txt

###### enc_server.c
This program compiles to create the enc_server executable, which starts an
encryption server listening on the specified port number. The server receives
plaintext files and a key from the encryption client and sends back the 
cyphertext. EXAMPLE:  
`./enc_server 5678`  
This command will start an encryption server listening on port 5678.  

###### dec_client.c
This program compiles to create the dec_client executable, which works exactly
the same as the encryption client, but instead sends cyphertext to a decryption
server to be decrypted and returned as plaintext. EXAMPLE:  
`./dec_client cyphertext.txt key.txt 5678 > plaintext.txt`  
This command will utilize the dec_client program to send a cyphertext file and
key to the decryption server on port 5678 and redirect the plaintext results
to a file called plaintext.txt  

###### dec_server.c
This program compiles to create the dec_server executable, which works exactly 
the same as the encryption client, but instead receives cyphertext files and 
a key from a decryption client and sends back the decrypted file, which is 
outputted to stdout. EXAMPLE:  
`./dec_server 5678`  
This command will start an decryption server listening on port 5678.  

#### --- NOTES ---
The encryption server cannot be used to connect to a decryption server, and 
vice versa. An authentication error message will be printed indicating the 
wrong client/server type was used. 
