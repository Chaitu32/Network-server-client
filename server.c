#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define PORT 8000
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread,ret;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 5) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while(server_fd){
        // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
        valread = recv(new_socket , buffer, 1024,MSG_CONFIRM);  // read infromation received into the buffer
        if(valread<=0){
            if(valread==0)
            exit(EXIT_SUCCESS);
            perror("Failed at recv");
            exit(EXIT_FAILURE);
        }
        printf("MSG: %s\n",buffer);
        int fd = open(buffer,O_RDONLY);
        if(fd<0){
            perror("Failed to open file");
            ret = send(new_socket,NULL, sizeof(NULL),MSG_CONFIRM);
            if(ret<0){
                perror("Failed at send");
                exit(EXIT_FAILURE);
            }
            recv(new_socket,&ret,sizeof(ret),MSG_CONFIRM);
            if(ret!=1){
                printf("Error at sync\n");
                exit(EXIT_FAILURE);
            }
            continue;
        }
        char buffer2[10005];
        int sd;
        int proceed;
        recv(new_socket,(void *)&proceed,sizeof(proceed),MSG_CONFIRM);
        if(proceed<0){
            printf("Error at client side couldn't sent file\n");
            continue;
        }
        long long int sizeof_file = lseek(fd,0,SEEK_END);
        printf("size of file %lld\n",sizeof_file);
        send(new_socket,(void *)&sizeof_file,sizeof(sizeof_file),MSG_CONFIRM);
        lseek(fd,0,SEEK_SET);
        long long int sizeofdata_sent=0;
        printf("Progress  00");
        while(sizeofdata_sent<sizeof_file){
            sd = read(fd,buffer2,10005);
            if(sd<0){
                perror("Failed to read");
                continue;
            }
            sizeofdata_sent+=sd;
            buffer2[sd]='\0';
            ret=send(new_socket , buffer2 , sd , MSG_CONFIRM);  // use sendto() and recvfrom() for DGRAM
            if(ret<0){
                perror("Failed at send");
                continue;
            }
            int pro = (sizeofdata_sent*100)/sizeof_file;
            printf("\b\b%2d",pro);
            lseek(fd,sizeofdata_sent,SEEK_SET);
        }
        printf("\n");
        int check;
        ret = recv(new_socket,(void *)&check,sizeof(check),0);
        if(ret <0 || check!=1){
            perror("Failed at recv");
            exit(EXIT_FAILURE);
        }
    }
    close(new_socket);
    return 0;
}
