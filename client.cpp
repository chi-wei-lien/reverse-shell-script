#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int setup_socket();

int main() {
  int client_fd = setup_socket();
  char hello[] = "Hello from client";
  send(client_fd, hello, strlen(hello), 0);
  close(client_fd);
  return 0;
}

int setup_socket() {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    printf("socket error \n");
    exit(EXIT_FAILURE);
  }
 
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  //convert address to binary
  inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
  
  int connect_return = connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  if (connect_return < 0) {
    printf("connection Failed \n");
    exit(EXIT_FAILURE);
  }

  return socket_fd;
}