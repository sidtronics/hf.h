#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
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

  srand(time(NULL));

  hf_context ctx = {0};
  int i = 0;
  while (1) {

    hf_append_header(&ctx, "RANDOM_VALUE");
    hf_append_field(&ctx, "count", i++);
    hf_append_field(&ctx, "random", ((double)rand() / RAND_MAX) * 100);
    hf_end_message(&ctx);

    if (!hf_message_send_sync(fd, &ctx)) {
      fprintf(stderr, "[server]: %s\n", hf_get_error_string(&ctx));
      exit(1);
    }

    printf("[client]: message sent!\n");
    usleep(((rand() % 5) + 5) * 1000 * 1000);
  }

  close(fd);
  return 0;
}
