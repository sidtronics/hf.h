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

int main(void) {

  int fd;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    exit(1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1) {
    perror("inet_pton");
    exit(1);
  }

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("connect");
    exit(1);
  }

  hf_context ctx = {0};

  hf_append_header(&ctx, "MESSAGE");
  hf_append_field(&ctx, "str", "Hello from client!");
  hf_append_field(&ctx, "int", 456);
  hf_append_field(&ctx, "dbl", 3.141592);
  hf_end_message(&ctx);

  if (!hf_message_send_sync(fd, &ctx)) {
    fprintf(stderr, "[server]: %s\n", hf_get_error_string(&ctx));
    exit(1);
  }

  close(fd);
  return 0;
}
