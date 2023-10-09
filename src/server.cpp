#include "server.hpp"

#define MAX_CONNECTIONS 50
#define INSTANT_SERVER_RESPONSE false

vector<int> client_sockets;
vector<thread> clients_threads;
mutex mtx;

bool CONNECTION_CLOSED;

void shutdown(){
    for (int client : client_sockets){
	close(client);	
    }
            
    client_sockets.clear();
}

void broadcastMessage(int server_socket, char *message) {
    size_t message_len = strlen(message);
    for (int client : client_sockets){
        int bytes_send = send(client, message, message_len, 0);
    }
}

void broadcastServerMessage(int server_socket){
    char buffer[1024];
    int SHUTDOWN = 0;
    while (true){
        cin.getline(buffer, 1024);

	if (strcmp("EXIT", buffer) == 0){
	    char warning[] = " * Server will be disable!";
	    broadcastMessage(server_socket, warning);
	    close(server_socket);
	    exit(0);
	}
	
	// FORMAT
        string response = " - [! ADM !]: " + (string) buffer;
        char message[response.length() + 1];

        // SEND MESSAGE
        broadcastMessage(server_socket, strcpy(message, response.c_str()));
    } 
}

void handleClient(int client_socket, int server_socket){
    char username[128];
    memset(username, 0, sizeof(username));

    // RECEIVE THE USERNAME OF CLIENT
    read(client_socket, username, sizeof(username) - 1);
    cout << "[+] (" << username << ") connected" << endl;

    // ADD TO VECTOR
    mtx.lock();
    client_sockets.push_back(client_socket);
    mtx.unlock();
        
    // COMMUNICATION
    char buffer[1024];
    string message;
    while (true) {
        // RECEIVE DATA 
        ssize_t bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        
        // DELETE CLIENT FROM VECTOR
        if (bytesRead <= 0) {
            mtx.lock();
            auto it = find(client_sockets.begin(), client_sockets.end(), client_socket);
            if (it != client_sockets.end()) {
                client_sockets.erase(it);
            }
            mtx.unlock();
            
            cout << " [+] (" << username << ") disconnected.\n" << endl;
            break;
        }
        buffer[bytesRead] = '\0';

        // SEND MESSAGE OF ONE CLIENT TO ALL CLIENTS
        string response = " - (" + (string) username + "): " + buffer;
        char message[response.length() + 1];
        broadcastMessage(server_socket, strcpy(message, response.c_str()));

        // SERVER TERMINAL
        printf("%s\n", response.c_str());
    }
    close(client_socket);
}

void acceptClients(int server_socket){
    while (true) {
        // ACCEPT CONNECTIONS
        int new_client_socket = accept(server_socket, nullptr, nullptr);

        if (new_client_socket == -1) {
            cerr << "[-] error accepting connection." << endl;
            continue;
        }

        // add new client to threads vector
        // handle new client
        thread clientThread(handleClient, new_client_socket, server_socket);
        clients_threads.push_back(move(clientThread));
    }
}


int main(){
    // CREATE SOCKET DESCRIPTOR 
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // 0 to tcp protocol
    if (server_socket < 0){
        perror("[-] error in socket creation.\n");
        exit(EXIT_FAILURE);
    }
    printf("[+] server created.\n");
    
    // CREATE SOCKET ADDRESS
    struct sockaddr_in address;
    address.sin_family = AF_INET; // address family
    address.sin_addr.s_addr = INADDR_ANY; // accept any msg
    address.sin_port = htons(PORT); // port

    // BIND SOCKET 
    int bind_code = bind(server_socket, (struct sockaddr*)&address, sizeof(sockaddr));
    if (bind_code < 0){
        perror("[-] fail to bind.\n");
        exit(EXIT_FAILURE);
    }
    printf("[+] server binded.\n");

    // LISTEN TO CONNECTIONS 
    if (listen(server_socket, MAX_CONNECTIONS) < 0){
        printf("[-] error in listening\n");
    }
    printf("[+] server is listening to connections...\n");
    
    thread t_broadcast_server(broadcastServerMessage, server_socket);    

    // THREAD DO RECEIVE CONNECTIONS
    acceptClients(server_socket);

    return 0;
}
