#include <arpa/inet.h>
#include <ctype.h>
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
#define ERROR(x) fprintf(stderr, "[server]: %s\n", (x))

hf_context ctx = {0};
#define STRING_BUFFER_SIZE 128
char text[STRING_BUFFER_SIZE];

void handle_request();
void handle_request_addition(hf_message *msg);
void handle_request_subtraction(hf_message *msg);
void handle_request_division(hf_message *msg);
void handle_request_capitalise(hf_message *msg);

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

  while (1) {

    if (!hf_message_recv_sync(client_fd, &ctx)) {
      ERROR(hf_get_error_string(&ctx));
      exit(1);
    }

    handle_request();

    if (!hf_message_send_sync(client_fd, &ctx)) {
      ERROR(hf_get_error_string(&ctx));
      exit(1);
    }
  }

  close(client_fd);
  close(server_fd);
  return 0;
}

void handle_request() {

  hf_message msg = {0};

  if (!hf_message_parse(&ctx, &msg)) {
    ERROR(hf_get_error_string(&ctx));
    exit(1);
  }

  switch (hf_message_get_header(&msg)) {

  case HEADER_ADDITION:
    handle_request_addition(&msg);
    break;

  case HEADER_SUBTRACTION:
    handle_request_subtraction(&msg);
    break;

  case HEADER_CAPITALISE:
    handle_request_capitalise(&msg);
    break;

  case HEADER_DIVISION:
    handle_request_division(&msg);
    break;

  default:
    ERROR("unknown request header");
  }

  if (!hf_message_build(&ctx, &msg)) {
    ERROR(hf_get_error_string(&ctx));
    exit(1);
  }
}

void handle_request_addition(hf_message *msg) {

  long op1, op2;

  if (!hf_message_mask_has_all(msg, KEY_OPERAND1 | KEY_OPERAND2)) {
    ERROR("missing fields in request");
    exit(1);
  }

  op1 = hf_message_get_field_operand1(msg);
  op2 = hf_message_get_field_operand2(msg);

  printf("[server]: Request: Header: HEADER_ADDITION\t");
  printf("op1: %ld\top2: %ld\n", op1, op2);

  hf_message_set_header(msg, HEADER_RESULT);
  hf_message_set_field_equals(msg, op1 + op2);
}

void handle_request_subtraction(hf_message *msg) {

  long op1, op2;

  if (!hf_message_mask_has_all(msg, KEY_OPERAND1 | KEY_OPERAND2)) {
    ERROR("missing fields in request");
    exit(1);
  }

  op1 = hf_message_get_field_operand1(msg);
  op2 = hf_message_get_field_operand2(msg);

  printf("[server]: Request: Header: HEADER_SUBTRACTION\t");
  printf("op1: %ld\top2: %ld\n", op1, op2);

  hf_message_set_header(msg, HEADER_RESULT);
  hf_message_set_field_equals(msg, op1 - op2);
}

void handle_request_division(hf_message *msg) {

  long op1, op2;

  if (!hf_message_mask_has_all(msg, KEY_OPERAND1 | KEY_OPERAND2)) {
    ERROR("missing fields in request");
    exit(1);
  }

  op1 = hf_message_get_field_operand1(msg);
  op2 = hf_message_get_field_operand2(msg);

  printf("[server]: Request: Header: HEADER_DIVISION\t");
  printf("op1: %ld\top2: %ld\n", op1, op2);

  hf_message_set_header(msg, HEADER_RESULT);

  if (op2 == 0)
    hf_message_set_field_text(msg, "error: Division by zero");
  else
    hf_message_set_field_equals(msg, (double)op1 / op2);
}

void handle_request_capitalise(hf_message *msg) {

  if (!hf_message_mask_has_all(msg, KEY_TEXT)) {
    ERROR("missing fields in request");
    exit(1);
  }

  strncpy(text, hf_message_get_field_text(msg), STRING_BUFFER_SIZE);

  printf("[server]: Request: Header: HEADER_CAPITALISE\t");
  printf("text: %s\n", text);

  for (size_t i = 0; i < strlen(text); ++i)
    text[i] = toupper(text[i]);

  hf_message_set_header(msg, HEADER_RESULT);
  hf_message_set_field_text(msg, text);
}
