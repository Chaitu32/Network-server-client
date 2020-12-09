// Client side C/C++ program to demonstrate Socket programming
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
    struct sockaddr_in address;
    int sock = 0, valread,ret;
    struct sockaddr_in serv_addr;
    char *hello = "./test.txt";
    char buffer[10005] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    int status = 1;
    while(status){
        printf("client>");
        char *buffer3;
        size_t buffer_size = 10005;
        buffer3 = (char *) malloc(buffer_size);
        int len = getline(&buffer3,&buffer_size,stdin);
        buffer3[len-1]='\0';
        const char delim[2] = " ";
        char *tok;
        tok = strtok(buffer3,delim);
        if(strcmp(tok,"exit")==0){
            send(sock,NULL,0,MSG_CONFIRM);
            close(sock);
            printf("Connection disconnected\n");
            status = 0;
            continue;
        }
        if(strcmp(tok,"get")!=0){
            send(sock,NULL,0,MSG_CONFIRM);
            printf("Invaild command argument\n");
            continue;
        }
        while(tok = strtok(NULL,delim)){
            //printf("%s\n",tok);
            send(sock , tok , strlen(tok) , MSG_CONFIRM);  // send the message.
            printf("Requested file to download\n");
            long long int sizeof_msg;
            long long int sizeofile_downloaded=0;
            int fd = open(tok,O_RDWR | O_CREAT, 0644);
            ret = send(sock,(void *)&fd,sizeof(fd),MSG_CONFIRM);
            if(ret<0){
                perror("Failed at send");
                exit(EXIT_FAILURE);
            }
            if(fd<0){
                perror("Failed to download a file");
                continue;
            }
            valread = recv( sock ,(void *)&sizeof_msg,10005,MSG_CONFIRM);
            if(valread<0){
                perror("recv failed");
                exit(EXIT_FAILURE);
            }
            if(sizeof_msg<=0){
                printf("Oops! file doesn't exist\n");
                fflush(stdout);
                int resum=1;
                if(send(sock,&resum,sizeof(resum),MSG_CONFIRM)<0){
                    perror("Failed at send");
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            printf("Downloading file of size : %lld\n",sizeof_msg);
            printf("Progress: 00");
            while(sizeofile_downloaded<sizeof_msg){
                valread = recv( sock , buffer,10005,MSG_CONFIRM);  // receive message back from server, into the buffer
                if(valread<0){
                    perror("recv failed");
                    exit(EXIT_FAILURE);
                }
                int sd = write(fd,buffer,valread);
                if(sd<0){
                    perror("Failed to download");
                    exit(EXIT_FAILURE);
                }
                sizeofile_downloaded+=sd;
                int pro = (sizeofile_downloaded*100)/sizeof_msg;
                printf("\b\b%2d",pro);
                fflush(stdout);
            }
            printf("\n%lld \n",sizeofile_downloaded);
            int check =1;
            if(send(sock,(void *)&check,sizeof(check),MSG_CONFIRM)<0){
                perror("Failed at send");
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}
