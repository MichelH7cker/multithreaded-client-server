#include "client.hpp"

void receiveMessage(int client_socket){
    while (true){    
        char message_rcv[1024];
        
        // RECEIVE MSG OF SERVER
        ssize_t bytes_read = recv(client_socket, message_rcv, sizeof(message_rcv) - 1, 0);

        // SERVER IS NOT RESPONDING  
        if (bytes_read <= 0) {
            cout << "[-] you have been disconnected or the server has been closed. sorry :(" << endl;
            break;
        }
        message_rcv[bytes_read] = '\0';
        printf("%s\n", message_rcv);
    }
}

void sendMessage(int client_socket){
    // COMMUNICATION WITH SERVER
    char message_send[1024];
    char message_rcv[1024];
    
    while (true){    
        cin.getline(message_send, sizeof(message_send));

        // SEND MSG TO SERVER
        send(client_socket, message_send, strlen(message_send), 0);

        // END CONNECTION
        if (strcmp(message_send, "q") == 0) {
            break;
        }
    }
}

int main(){
    // CREATE CLIENT SOCKET
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("[-] socket creation error \n");
        exit(EXIT_FAILURE);
    }

    // CREATE SOCKET ADDRESS
    struct sockaddr_in address;
    address.sin_family = AF_INET; // address family
    address.sin_port = htons(PORT); // port
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

    int connection_code = connect(client_socket, (struct sockaddr*)&address, sizeof(address));
    if (connection_code < 0){
        perror("[-] connection failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("[+] connected to server.\n");

    char username[128];
    std::cout << " * enter your username: ";
    std::cin.getline(username, sizeof(username));

    // send the username to the server
    write(client_socket, username, strlen(username));

    cout << " * enter a message in any moment (or 'q' to disconnect)." << endl;
    
    thread (receiveMessage, client_socket).detach();
    thread sendMsgThread(sendMessage, client_socket);
    sendMsgThread.join();

    close(client_socket);
}
