#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT (8080)
#define BACKLOG (3)

int setup_socket();

int main() {
  int socket_accept_return = setup_socket();
  char buffer[1024] = { 0 };
  int valread = read(socket_accept_return, buffer, 1024);
  printf("%s\n", buffer);
  close(socket_accept_return);
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
  address.sin_addr.s_addr = INADDR_ANY;
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
  
  int server_fd = accept(socket_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

  if (server_fd < 0) {
    printf("accept failed");
    exit(EXIT_FAILURE);
  }
  return server_fd;

}