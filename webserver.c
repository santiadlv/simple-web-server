#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 8080
#define PROTOCOL_NUMBER 1
#define LIMIT_FILE_SIZE 15
#define MAX_CONNECTIONS 100
#define MAX_REQUEST_SIZE 1024
#define MAX_RESPONSE_SIZE 131072

char *handle_request(int fd);
long get_file_size(char* filename);


int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t clsock_len = sizeof(client_addr);
    int server_fd, client_fd;
    char buffer[MAX_RESPONSE_SIZE];
    int optval = PROTOCOL_NUMBER;    

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("webserver: fatal error getting listening socket\n");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("webserver: fatal error binding address to socket\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        perror("webserver: fatal error, server is not listening");
        exit(EXIT_FAILURE);
    }

    while(1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &clsock_len)) < 0) {
            perror("webserver: fatal error accepting connections\n");
            exit(EXIT_FAILURE);
        }
        
        if (!fork()) {
            close(server_fd);
            memset(buffer, 0, MAX_RESPONSE_SIZE);
            strcpy(buffer, handle_request(client_fd));
            send(client_fd, buffer, sizeof(buffer) - 1, 0);
            close(client_fd);
            printf("Closing connection...\n");
            exit(0);
        }
        close(client_fd);
    }
    return 0;
}

char* handle_request(int fd) {
    char request[MAX_REQUEST_SIZE];
    char response[MAX_RESPONSE_SIZE];
    int bytes_read = recv(fd, request, sizeof(request), 0);

    if (bytes_read < 0) {
        strcpy(response, "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n");
        perror("webserver: request could not be read");
        return strdup(response);
    }

    char method[8];
    char endpoint[128];
    char protocol[20];

    sscanf(request, "%s %s %s", method, endpoint, protocol);
    printf("\n%s\n%s\n%s\n", method, endpoint, protocol);

    if (strcmp(method, "GET") == 0 && (strcmp(endpoint, "/TC2025.html") == 0 || strcmp(endpoint, "/syllabus") == 0)) {
        FILE *html_data = fopen("TC2025.html", "r");
        unsigned long file_size = get_file_size("TC2025.html");
        char file_contents[file_size];
        char size[LIMIT_FILE_SIZE];

        sprintf(size, "%lu", file_contents);
        fread(file_contents, file_size, 1, html_data);
        fclose(html_data);

        char *headers = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\n\n";
        strcpy(response, headers);
        strcat(response, file_contents);
    }
    else {
        FILE *html_data = fopen("index.html", "r");
        unsigned long file_size = get_file_size("index.html");
        char file_contents[file_size];
        char size[LIMIT_FILE_SIZE];

        sprintf(size, "%lu", file_contents);
        fread(file_contents, file_size, 1, html_data);
        fclose(html_data);

        char *headers = "HTTP/1.1 404 NOT FOUND\nContent-Type: text/html; charset=UTF-8\n\n";
        strcpy(response, headers);
        strcat(response, file_contents);
        perror("webserver: requested endpoint not found");
    }
    
    return strdup(response);
}

long get_file_size(char* filename) {
    FILE *f = fopen(filename, "r");
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    fclose(f);
    return length;
}
