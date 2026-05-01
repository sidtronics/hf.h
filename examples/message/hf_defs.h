// string, flag, type, field name
#define HF_FIELDS                                                              \
  X("op1", KEY_OPERAND1, HF_INTEGER_T, operand1)                               \
  X("op2", KEY_OPERAND2, HF_INTEGER_T, operand2)                               \
  X("eql", KEY_EQUALS, HF_DOUBLE_T, equals)                                    \
  X("txt", KEY_TEXT, HF_STRING_T, text)                                        \
  X("val", KEY_VALUE, HF_BOOL_T, value)

// string, identifier, valid field mask
#define HF_HEADERS                                                             \
  X("ADD", HEADER_ADDITION, KEY_OPERAND1 | KEY_OPERAND2)                       \
  X("SUB", HEADER_SUBTRACTION, KEY_OPERAND1 | KEY_OPERAND2)                    \
  X("DIV", HEADER_DIVISION, KEY_OPERAND1 | KEY_OPERAND2)                       \
  X("CAP", HEADER_CAPITALISE, KEY_TEXT)                                        \
  X("LGT", HEADER_LIGHT, KEY_VALUE)                                            \
  X("RES", HEADER_RESULT, KEY_EQUALS | KEY_TEXT)
