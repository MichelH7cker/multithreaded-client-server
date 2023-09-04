#include "client.hpp"

int main(){
    // CREATE CLIENT SOCKET
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("[-] Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    // CREATE SOCKET ADDRESS
    struct sockaddr_in address;
    address.sin_family = AF_INET; // address family
    address.sin_port = htons(PORT); // port
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

    int connection_code = connect(client_socket, (struct sockaddr*)&address, sizeof(address));
    if (connection_code < 0){
        perror("[-] Connection failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[+] Connected to server.\n");

     // COMMUNICATION WITH SERVER
    char buffer[1024];
    std::string message;
    while (true) {
        cout << "\t enter a message (or 'q' to quit): ";
        cin.getline(buffer, sizeof(buffer));

        // SEND MSG TO SERVER
        send(client_socket, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "q") == 0) {
            break;
        }

        // Receive a response from the server
        ssize_t bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }
        buffer[bytesRead] = '\0';
        std::cout << "Server response: " << buffer << std::endl;
    }

    close(client_socket);
}