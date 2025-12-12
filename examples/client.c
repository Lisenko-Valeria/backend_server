#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h> 

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char *send_message = "Hello from C Client!";
    char received_message[1024] = {0};
    

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error");
        return -1;
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    printf("Подключение к серверу %s:%d...\n", SERVER_IP, SERVER_PORT);
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    send(client_socket, send_message, strlen(send_message), 0);
    
    int received = recv(client_socket, received_message, 1024 - 1, 0);
    
    received_message[received] = '\0';
    printf("%s\n", received_message);
    
    close(client_socket);
    return 0;
}