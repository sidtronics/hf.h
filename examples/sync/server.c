#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HF_IMPLEMENTATION
#include "../../hf.h"

#define PORT 9090
#define BUF_SIZE 128

int main(void) {

  int server_fd, client_fd;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    exit(1);
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  memset(&addr, 0, addrlen);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(1);
  }

  if (listen(server_fd, 1) == -1) {
    perror("listen");
    exit(1);
  }

  printf("[server]: listening on port %d\n", PORT);

  client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
  if (client_fd == -1) {
    perror("accept");
    exit(1);
  }

  hf_context ctx = {0};
  if (!hf_message_recv_sync(client_fd, &ctx)) {
    fprintf(stderr, "[server]: %s\n", hf_get_error_string(&ctx));
    exit(1);
  }

  const char *header, *key, *val;
  if (!hf_parse_header(&ctx, &header)) {
    fprintf(stderr, "[server]: %s\n", hf_get_error_string(&ctx));
    exit(1);
  }

  printf("header: %s\n", header);

  while (!hf_end_reached(&ctx)) {

    if (!hf_parse_field(&ctx, &key, &val)) {
      fprintf(stderr, "[server]: %s\n", hf_get_error_string(&ctx));
      exit(1);
    }

    printf("key: %s\tval: %s\n", key, val);
  }

  close(client_fd);
  close(server_fd);
  return 0;
}
