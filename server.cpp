#include <netinet/in.h>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT (8080)
#define HOST ("127.0.0.1")
#define BACKLOG (3)

using namespace std;

int setup_socket();

int main() {
  printf("[*] Listening on %s:%d...\n", HOST, PORT);
  int server_fd = setup_socket();
  printf("[*] Client connected\n");
  string user_input = "";
  char cwd[256];
  int valread = read(server_fd, cwd, 256);
  while (true) {
    printf("%s> ", cwd);
    getline(cin, user_input);
    char buffer[user_input.length() + 1];
    strcpy(buffer, user_input.c_str());
    send(server_fd, buffer, strlen(buffer), 0);
    char response[1024] = { 0 };
    int valread = read(server_fd, response, 1024);
    if (strcmp(response, "") == 0) {
      continue;
    }
    printf("%s\n", response);
  }

  close(server_fd);
  return 0;

}

int setup_socket() {

  /*
   * AF_INET: Ipv4
   * SOCK_STREAM: TCP
   */
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == 0) {
    printf("socket connection faild");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  
  inet_pton(AF_INET, HOST, &address.sin_addr.s_addr);
  address.sin_port = htons(PORT);
  int addrlen = sizeof(address);
  
  int bind_return = bind(socket_fd, (struct sockaddr*)&address, sizeof(address));

  if (bind_return < 0) {
    printf("bind faild");
    exit(EXIT_FAILURE);
  }

  int listen_return = listen(socket_fd, BACKLOG);
  if (listen_return < 0) {
    printf("listen failed");
    exit(EXIT_FAILURE);
  } 
  
  /* change this socket name */
  int server_fd = accept(socket_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

  if (server_fd < 0) {
    printf("accept failed");
    exit(EXIT_FAILURE);
  }
  return server_fd;

}