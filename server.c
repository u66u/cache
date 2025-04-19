#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdbool.h>

#include "hashmap.h"
#include "cache.h"

#define IPADDR "127.0.0.1"
#define PORT 9999
#define BUFFER_SIZE 4096

int parse_http_request(char *buffer, char *command, char *key, char *value) {
    
    char *body = strstr(buffer, "\r\n\r\n");
    if (!body) {
        return 0;
    }
    
    body += 4; 
    
    
    char *token = strtok(body, "&");
    while (token != NULL) {
        if (strncmp(token, "command=", 8) == 0) {
            strcpy(command, token + 8);
        } else if (strncmp(token, "key=", 4) == 0) {
            strcpy(key, token + 4);
        } else if (strncmp(token, "value=", 6) == 0) {
            strcpy(value, token + 6);
        }
        token = strtok(NULL, "&");
    }
    
    return 1;
}

void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if (*src == '%' && src[1] && src[2]) {
            a = tolower(src[1]);
            b = tolower(src[2]);
            
            if (isxdigit(a) && isxdigit(b)) {
                if (a >= 'a') a = a - 'a' + 10;
                else a = a - '0';
                
                if (b >= 'a') b = b - 'a' + 10;
                else b = b - '0';
                
                *dst++ = 16 * a + b;
            } else {
                *dst++ = '%';
                *dst++ = src[1];
                *dst++ = src[2];
            }
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

void create_http_response(char *response, int status, const char *content_type, const char *body) {
    sprintf(response, 
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status, 
        status == 200 ? "OK" : "Bad Request",
        content_type, 
        strlen(body), 
        body);
}

int main() {
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Failed to set socket options");
        exit(EXIT_FAILURE);
    }
    
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IPADDR);
    server_addr.sin_port = htons(PORT);
    
    
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }
    
    
    if (listen(server_fd, 10) < 0) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on %s:%d\n", IPADDR, PORT);
    
    
    struct hashmap *cache = hashmap_new(sizeof(struct item), 0, 0, 0, hash, cmp, free_item, NULL);
    if (hashmap_oom(cache)) {
        perror("Failed to create hashmap");
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_fd < 0) {
            perror("Failed to accept connection");
            continue;
        }
        
        
        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_read <= 0) {
            close(client_fd);
            continue;
        }
        
        
        char command[10] = {0};
        char key[256] = {0};
        char value[4096] = {0};
        char decoded_key[256] = {0};
        char decoded_value[4096] = {0};
        
        if (parse_http_request(buffer, command, key, value)) {
            
            url_decode(decoded_key, key);
            url_decode(decoded_value, value);
            
            
            char response[BUFFER_SIZE] = {0};
            
            if (strcmp(command, "GET") == 0) {
                
                struct item get_item;
                get_item.key = decoded_key;
                
                
                struct item *result = (struct item *)hashmap_get(cache, &get_item);
                
                if (result) {
                    char body[BUFFER_SIZE];
                    sprintf(body, "{\"status\":\"success\",\"key\":\"%s\",\"value\":\"%s\"}", 
                            result->key, result->val);
                    create_http_response(response, 200, "application/json", body);
                } else {
                    create_http_response(response, 200, "application/json", 
                                        "{\"status\":\"error\",\"message\":\"Key not found\"}");
                }
            } else if (strcmp(command, "SET") == 0) {
                
                struct item *new_item = item_from_kv(decoded_key, decoded_value);
                
                if (new_item) {
                    hashmap_set(cache, new_item);
                    free(new_item); 
                    
                    char body[BUFFER_SIZE];
                    sprintf(body, "{\"status\":\"success\",\"key\":\"%s\",\"value\":\"%s\"}", 
                            decoded_key, decoded_value);
                    create_http_response(response, 200, "application/json", body);
                } else {
                    create_http_response(response, 200, "application/json", 
                                        "{\"status\":\"error\",\"message\":\"Failed to create item\"}");
                }
            } else {
                create_http_response(response, 400, "application/json", 
                                    "{\"status\":\"error\",\"message\":\"Invalid command\"}");
            }
            
            
            send(client_fd, response, strlen(response), 0);
        } else {
            
            char response[BUFFER_SIZE];
            create_http_response(response, 400, "application/json", 
                                "{\"status\":\"error\",\"message\":\"Invalid request format\"}");
            send(client_fd, response, strlen(response), 0);
        }
        
        
        close(client_fd);
    }
    
    
    hashmap_free(cache);
    close(server_fd);
    
    return 0;
}
