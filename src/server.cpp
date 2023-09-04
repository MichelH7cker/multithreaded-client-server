#include "server.hpp"

#define MAX_CONNECTIONS 50

void handle_client(int client_socket){
    // Communication with the client
    char buffer[1024];
    string message;
    while (true) {
        // Receive data from the client
        ssize_t bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }

        // receive msg
        buffer[bytesRead] = '\0';
        printf("\t - Server side received: %s\n", buffer);

        // send msg
        message = "Server received message";
        send(client_socket, message.c_str(), message.length(), 0);
    }
}

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

    // Listen on the socket, with MAX_CONNECTIONS
    if (listen(server_socket, MAX_CONNECTIONS) == 0){
        printf("[+] Socket server is listening.\n");
    } else {
        printf("[-] Error in listening\n");
    }
    
    vector<thread> clients_threads;
    
    while (true) {
        // Accept incoming connections
        int new_client_socket = accept(server_socket, nullptr, nullptr);
        if (new_client_socket == -1) {
            cerr << "[-] Error accepting connection" << endl;
            continue;
        }

        cout << "[+] Client connected: " << new_client_socket << endl;

        // add new client to threads vector
        // handle new client
        clients_threads.emplace_back(handle_client, new_client_socket);
    }


    // CLOSE SOCKET
    close(server_socket);
    
    return 0;
}