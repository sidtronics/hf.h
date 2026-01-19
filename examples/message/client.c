#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "hf_defs.h"
#define HF_IMPLEMENTATION
#include "../../hf.h"

#define PORT 9090

hf_context ctx = {0};

void handle_response(hf_message *msg);
void send_and_receive(int fd);

void create_request_add(hf_message *msg, unsigned long op1, unsigned long op2);
void create_request_sub(hf_message *msg, unsigned long op1, unsigned long op2);
void create_request_div(hf_message *msg, unsigned long op1, unsigned long op2);
void create_request_cap(hf_message *msg, const char *text);

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


  int input = -1;
  int op1, op2;
  char text[128];
  hf_message msg = {0};

  while (1) {

    printf("\n");
    printf("0. Addition\n");
    printf("1. Subtraction\n");
    printf("2. Division\n");
    printf("3. Capitalise\n");
    printf("4. Quit\n");
    printf("Choose operation: ");
    scanf("%d", &input);

    switch (input) {
    case 0:
      printf("Enter 2 numbers: ");
      scanf("%d %d", &op1, &op2);
      create_request_add(&msg, op1, op2);
      break;
    case 1:
      printf("Enter 2 numbers: ");
      scanf("%d %d", &op1, &op2);
      create_request_sub(&msg, op1, op2);
      break;
    case 2:
      printf("Enter 2 numbers: ");
      scanf("%d %d", &op1, &op2);
      create_request_div(&msg, op1, op2);
      break;
    case 3:
      printf("Enter string: ");
      scanf("%127s", text);
      create_request_cap(&msg, text);
      break;
    case 4:
      exit(0);
    default:
      continue;
    }

    send_and_receive(fd);
    handle_response(&msg);
  }

  close(fd);
  return 0;
}

void _create_request_arithmetic(hf_message *msg, hf_header header, int op1,
                                int op2) {

  hf_message_set_header(msg, header);
  hf_message_set_field_operand1(msg, op1);
  hf_message_set_field_operand2(msg, op2);

  if (!hf_message_build(&ctx, msg)) {
    fprintf(stderr, "[client]: %s\n", hf_get_error_string(&ctx));
    exit(1);
  }
}

void create_request_add(hf_message *msg, unsigned long op1, unsigned long op2) {
  _create_request_arithmetic(msg, HEADER_ADDITION, op1, op2);
}

void create_request_sub(hf_message *msg, unsigned long op1, unsigned long op2) {
  _create_request_arithmetic(msg, HEADER_SUBTRACTION, op1, op2);
}

void create_request_div(hf_message *msg, unsigned long op1, unsigned long op2) {
  _create_request_arithmetic(msg, HEADER_DIVISION, op1, op2);
}

void create_request_cap(hf_message *msg, const char *text) {

  hf_message_set_header(msg, HEADER_CAPITALISE);
  hf_message_set_field_text(msg, text);

  if (!hf_message_build(&ctx, msg)) {
    fprintf(stderr, "[client]: %s\n", hf_get_error_string(&ctx));
    exit(1);
  }
}

void send_and_receive(int fd) {

  if (!hf_message_send_sync(fd, &ctx)) {
    fprintf(stderr, "[client]: %s\n", hf_get_error_string(&ctx));
    exit(1);
  }

  if (!hf_message_recv_sync(fd, &ctx)) {
    fprintf(stderr, "[client]: %s\n", hf_get_error_string(&ctx));
    exit(1);
  }
}

void handle_response(hf_message *msg) {

  if (!hf_message_parse(&ctx, msg)) {
    fprintf(stderr, "[client]: %s\n", hf_get_error_string(&ctx));
    exit(1);
  }

  /* Make sure that it is result header */
  assert(hf_message_get_header(msg) == HEADER_RESULT &&
         "unknown message header");

  /* Make sure either 'equals' or 'text' field is set */
  assert(hf_message_mask_has_any(msg, KEY_EQUALS | KEY_TEXT) &&
         "no fields found");

  if (hf_message_has_field_equals(msg))
    printf("Response: %f\n", hf_message_get_field_equals(msg));

  /* 'equals' is not set. So it must be 'text' */
  else
    printf("Response: %s\n", hf_message_get_field_text(msg));
}
