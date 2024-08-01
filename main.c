#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *message)
{
    perror(message);
    exit(1);
}

void server(void)
{
    socklen_t clientlen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // Creates new socket using IPv4 and TCP
    if (sockfd < 0)
        error("ERROR opening socket");

    // Set up server address structure
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    clientlen = sizeof(client_addr);

    int newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &clientlen);
    if (newsockfd < 0)
        error("ERROR on accept");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytesRead = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (bytesRead < 0) error("ERROR reading from socket");
        printf("Client: %s\n", buffer);

        ssize_t bytesWritten = write(newsockfd, "This message is sent back to client", 36);
        if (bytesWritten < 0) error("ERROR writing to socket");
    }

    close(newsockfd);
    close(sockfd);
}

void client() {
    struct addrinfo hints, *res;
    char buffer[BUFFER_SIZE];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    // Resolve the server address and port
    int status = getaddrinfo("localhost", "8080", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0)
        error("ERROR opening socket");

    // Connect to server
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0)
        error("ERROR connecting");

    freeaddrinfo(res); // free the linked-list

    while (1) {
        printf("Please enter the message: ");
        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE - 1, stdin);

        ssize_t bytesWritten = write(sockfd, buffer, strlen(buffer));
        if (bytesWritten < 0)
            error("ERROR writing to socket");

        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytesRead = read(sockfd, buffer, BUFFER_SIZE - 1);
        if (bytesRead < 0)
            error("ERROR reading from socket");
        printf("%s\n", buffer);
    }

    close(sockfd);
}


int main(void) {
    int choice;
    printf("Enter 1 to run as server, 2 to run as client: ");
    scanf("%d", &choice);
    getchar(); // Consume newline character left by scanf

    if (choice == 1) {
        server();
    } else if (choice == 2) {
        client();
    } else {
        printf("Invalid choice\n");
    }

    return 0;
}

