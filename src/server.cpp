#include "server.hpp"

int main(){
    // CREATE SOCKET DESCRIPTOR 
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // 0 to tcp protocol
    if (server_socket < 0){
        perror("[-] Error in socket creation.\n");
        exit(EXIT_FAILURE);
    }
    printf("[+] Server socket created.\n");
    
    // CREATE SOCKET ADDRESS
    struct sockaddr_in address;
    address.sin_family = AF_INET; // address family
    address.sin_addr.s_addr = INADDR_ANY; // accept any msg
    address.sin_port = htons(PORT); // port

    // BIND SOCKET 
    int bind_code = bind(server_socket, (struct sockaddr*)&address, sizeof(sockaddr));
    if (bind_code < 0){
        perror("[-] Fail to bind socket.\n");
        exit(EXIT_FAILURE);
    }
    printf("[+] Socket binded.\n");

    // Listen on the socket, with 40 max connection
    if (listen(server_socket, 50) == 0){
        printf("[+] Socket server is listening.\n");
    } else {
        printf("[-] Error in listening\n");
    }

    int new_socket = accept(server_socket, nullptr, nullptr);
    if (new_socket < 0){
        perror("[-] Connection failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[+] Connected.\n");
        

    // Communication with the client
    char buffer[1024];
    string message;
    while (true) {
        // Receive data from the client
        ssize_t bytesRead = recv(new_socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }

        // receive msg
        buffer[bytesRead] = '\0';
        printf("\t - Server side received: %s\n", buffer);

        // send msg
        message = "Server received message";
        send(new_socket, message.c_str(), message.length(), 0);
    }


    // CLOSE SOCKET
    close(server_socket);
    close(new_socket);

    return 0;
}