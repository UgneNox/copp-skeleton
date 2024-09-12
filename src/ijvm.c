#include <stdio.h>  // for getc, printf
#include <stdlib.h> // malloc, free
#include "ijvm.h"
#include "util.h" // read this file for debug prints, endianness helper functions

// see ijvm.h for descriptions of the below functions

// STACK FUNCTIONS
void push(ijvm *m, word_t value)
{
  if (m->stack->size >= m->stack->max - 1)
  {
    m->stack->max *= 2;
    word_t *new_value = realloc(m->stack->data, m->stack->max * sizeof(word_t));
    if (new_value == NULL)
    {
      fprintf(m->out, "Error: Push problem");
      destroy_ijvm(m);
      return;
    }
    m->stack->data = new_value;
  }
  m->stack->data[m->stack->size] = value;
  m->stack->size++;
  m->sp++;
}

word_t pop(ijvm *m)
{
  m->stack->size--;
  if (m->stack->size <= 0)
  {
    fprintf(m->out, "Error: Pop problem");
    destroy_ijvm(m);
    return -1;
  }
  m->sp--;
  return m->stack->data[m->stack->size];
}

word_t top(ijvm *m)
{
  if (m->stack->size <= 0)
  {
    fprintf(m->out, "Error: Top problem");
    destroy_ijvm(m);
    return -1;
  }
  return m->stack->data[m->stack->size - 1];
}

void initialize_stack(ijvm *m)
{
  for (int i = 0; i < 256; i++)
  {
    word_t value = 1;
    push(m, value);
  }
}

void destroy_stack(ijvm *m)
{
  if (m->stack->data != NULL)
  {
    free(m->stack->data);
    m->stack->data = NULL;
  }
  free(m->stack);
  m->stack = NULL;
}

// SHORT READING FUNCTIONS
int16_t shorty(ijvm *m)
{
  int16_t shawty_1 = (int16_t)get_instruction(m);
  m->pc++;
  int16_t shawty_2 = (int16_t)get_instruction(m);
  int16_t shawty = (int16_t)(shawty_1 << 8 | shawty_2);

  return shawty;
}

uint16_t unsigned_shorty(ijvm *m)
{
  uint16_t shawty_1 = (int16_t)get_instruction(m);
  m->pc++;
  uint16_t shawty_2 = (int16_t)get_instruction(m);
  uint16_t shawty = (int16_t)(shawty_1 << 8 | shawty_2);

  return shawty;
}

// HEAP FUNCTIONS
void initialize_heap(ijvm *m)
{
  m->heap = (heap *)malloc(sizeof(heap));
  m->heap->arrays = (array **)malloc(10 * sizeof(array *));
}

void destroy_heap(ijvm *m)
{
  for (int i = 0; i < m->heap->size; ++i)
  {
    if (m->heap->arrays[i] != NULL)
    {
      free(m->heap->arrays[i]->data);
      free(m->heap->arrays[i]);
    }
  }
  free(m->heap->arrays);
  free(m->heap);
}

word_t make_array(ijvm *m, word_t count)
{
  if (m->heap->size >= m->heap->max)
  {
    m->heap->max *= 2;
    m->heap->arrays = realloc(m->heap->arrays, m->heap->max * sizeof(array *));
  }
  array *new_array = (array *)malloc(sizeof(array));
  new_array->data = (word_t *)calloc(count, sizeof(word_t));
  new_array->size = count;
  m->heap->arrays[m->heap->size] = new_array;
  word_t array_ref = (word_t)m->heap->size++;
  return array_ref;
}

// SWITCH FUNCTIONS
void error(ijvm *m)
{
  fprintf(m->out, "Error: encountered OP_ERR\n");
  m->pc = m->text_size;
}

void halt(ijvm *m)
{
  m->pc = m->text_size;
}

void bipush(ijvm *m)
{
  m->pc++;
  int8_t signed_value = (int8_t)get_instruction(m);
  word_t value = (word_t)signed_value;
  push(m, value);
  m->pc++;
}

void dup(ijvm *m)
{
  push(m, top(m));
  m->pc++;
}

void iadd(ijvm *m)
{
  word_t b = pop(m);
  word_t a = pop(m);
  push(m, a + b);
  m->pc++;
}

void iand(ijvm *m)
{
  word_t b = pop(m);
  word_t a = pop(m);
  push(m, a & b);
  m->pc++;
}

void ior(ijvm *m)
{
  word_t b = pop(m);
  word_t a = pop(m);
  push(m, a | b);
  m->pc++;
}

void isub(ijvm *m)
{
  word_t b = pop(m);
  word_t a = pop(m);
  push(m, a - b);
  m->pc++;
}

void nop(ijvm *m)
{
  m->pc++;
}

void pop_op(ijvm *m)
{
  pop(m);
  m->pc++;
}

void swap(ijvm *m)
{
  word_t b = pop(m);
  word_t a = pop(m);
  push(m, b);
  push(m, a);
  m->pc++;
}

void in(ijvm *m)
{
  char ch = fgetc(m->in);
  if (ch == EOF)
    ch = 0;
  push(m, ch);
  m->pc++;
}

void out(ijvm *m)
{
  word_t value = pop(m);
  fprintf(m->out, "%c", (char)value);
  m->pc++;
}

void goto_op(ijvm *m)
{
  m->pc++;
  int16_t shawty = shorty(m);
  m->pc = m->pc + shawty - 2;
}

void ifeq(ijvm *m)
{
  m->pc++;
  int16_t shawty = shorty(m);
  word_t value = pop(m);
  if (value == 0)
  {
    m->pc = m->pc + shawty - 2;
  }
  else
  {
    m->pc++;
  }
}

void iflt(ijvm *m)
{
  m->pc++;
  int16_t shawty = shorty(m);
  word_t value = pop(m);
  if (value < 0)
  {
    m->pc = m->pc + shawty - 2;
  }
  else
  {
    m->pc++;
  }
}

void if_icmpeq(ijvm *m)
{
  m->pc++;
  int16_t shawty = shorty(m);
  word_t a = pop(m);
  word_t b = pop(m);
  if (a == b)
  {
    m->pc = m->pc + shawty - 2;
  }
  else
  {
    m->pc++;
  }
}

void ldc_w(ijvm *m)
{
  m->pc++;
  int16_t i = shorty(m);
  word_t constant = m->constant_pool[i];
  push(m, constant);
  m->pc++;
}

void iload(ijvm *m)
{
  m->pc++;
  word_t i = get_instruction(m);
  word_t value = get_local_variable(m, i);
  push(m, value);
  m->pc++;
}

void istore(ijvm *m)
{
  m->pc++;
  word_t i = get_instruction(m);
  word_t value = pop(m);
  m->stack->data[m->lv + i] = value;
  m->pc++;
}

void iinc(ijvm *m)
{
  m->pc++;
  word_t i = get_instruction(m);
  m->pc++;
  int8_t signed_value = (int8_t)get_instruction(m);
  word_t value = (word_t)signed_value;
  m->stack->data[m->lv + i] += value;
  m->pc++;
}

void iload_wide(ijvm *m)
{
  m->pc++;
  uint16_t i = unsigned_shorty(m);
  word_t value = get_local_variable(m, i);
  push(m, value);
  m->pc++;
}

void istore_wide(ijvm *m)
{
  m->pc++;
  uint16_t i = unsigned_shorty(m);
  word_t value = pop(m);
  m->stack->data[m->lv + i] = value;
  m->pc++;
}

void iinc_wide(ijvm *m)
{
  m->pc++;
  uint16_t i = unsigned_shorty(m);
  m->pc++;
  int8_t signed_value = (int8_t)get_instruction(m);
  word_t value = (word_t)signed_value;
  m->stack->data[m->lv + i] += value;
  m->pc++;
}

void wide(ijvm *m)
{
  m->pc++;
  unsigned char opcode_2 = get_instruction(m);
  switch (opcode_2)
  {
  case OP_ILOAD:
  {
    iload_wide(m);
    break;
  }
  case OP_ISTORE:
  {
    istore_wide(m);
    break;
  }
  case OP_IINC:
  {
    iinc_wide(m);
    break;
  }
  }
}

void invokevirtual(ijvm *m)
{
  m->pc++;
  int16_t i = shorty(m);

  m->prev_pc = m->pc;
  m->prev_lv = m->lv;
  m->lv = m->sp;

  word_t method_address = m->constant_pool[i];
  m->pc = method_address - 1;

  m->pc++;
  int16_t arg_num = shorty(m);

  m->lv = m->lv - arg_num;

  m->pc++;
  uint16_t var_num = unsigned_shorty(m);

  for (int i = 0; i < var_num; i++)
  {
    word_t value = 1;
    push(m, value);
  }

  push(m, m->prev_pc);
  push(m, m->prev_lv);

  m->sp = m->lv + arg_num + var_num + 2;

  m->stack->data[m->lv] = m->sp - 2;
  m->pc++;
}

void ireturn(ijvm *m)
{
  word_t return_result = pop(m);
  m->prev_lv = m->lv;

  m->pc = m->stack->data[m->stack->data[m->lv] + 0];
  m->lv = m->stack->data[m->stack->data[m->lv] + 1];

  for (int i = m->sp; i > m->prev_lv; i--)
  {
    pop(m);
  }

  m->sp = m->prev_lv;

  push(m, return_result);
  m->pc++;
}

void tailcall(ijvm *m)
{
  m->pc++;
  int16_t i = shorty(m);

  m->prev_pc = m->stack->data[m->stack->data[m->lv] + 0];
  m->prev_lv = m->stack->data[m->stack->data[m->lv] + 1];

  word_t method_address = m->constant_pool[i];
  m->pc = method_address - 1;

  m->pc++;
  int16_t arg_num = shorty(m);

  m->pc++;
  uint16_t var_num = unsigned_shorty(m);

  for (int i = 0; i < var_num; i++)
  {
    word_t value = 1;
    push(m, value);
  }

  for (int i = arg_num + var_num; i > 0; i--)
  {
    m->stack->data[m->lv + i - 1] = pop(m);
  }

  while (m->sp > m->lv + arg_num + var_num)
  {
    pop(m);
  }

  push(m, m->prev_pc);
  push(m, m->prev_lv);

  m->stack->data[m->lv] = m->sp - 2;
  m->pc++;
}

void newarray(ijvm *m)
{
  word_t count = pop(m);
  if (count <= 0)
  {
    fprintf(m->out, "Error: Invalid array size\n");
    halt(m);
    return;
  }
  word_t array_ref = make_array(m, count);
  push(m, array_ref);
  m->pc++;
}

void iaload(ijvm *m)
{
  word_t array_ref = pop(m);
  word_t i = pop(m);
  array *old_array = m->heap->arrays[array_ref];
  if (i < 0 || i >= old_array->size)
  {
    fprintf(m->out, "Error: Bad array index\n");
    halt(m);
    return;
  }
  push(m, old_array->data[i]);
  m->pc++;
}

void iastore(ijvm *m)
{
  word_t array_ref = pop(m);
  word_t i = pop(m);
  word_t value = pop(m);
  array *old_array = m->heap->arrays[array_ref];
  if (i < 0 || i >= old_array->size)
  {
    fprintf(m->out, "Error: Bad array index\n");
    halt(m);
    return;
  }
  old_array->data[i] = value;
  m->pc++;
}

// IJVM FUNCTIONS
ijvm *init_ijvm(char *binary_path, FILE *input, FILE *output)
{
  d5printf("INITIALIZING %s\n", binary_path);
  // do not change these first three lines
  ijvm *m = (ijvm *)malloc(sizeof(ijvm));
  m->in = input;
  m->out = output;

  // OPEN FILE
  FILE *binary_file = fopen(binary_path, "rb");
  if (binary_file == NULL)
  {
    free(m);
    return NULL;
  }

  // MAGIC NUMBER
  uint8_t numbuf[4];
  if (fread(numbuf, sizeof(uint8_t), 4, binary_file) != 4)
  {
    fclose(binary_file);
    destroy_ijvm(m);
    return NULL;
  }
  uint32_t number = read_uint32(numbuf);
  swap_uint32(number);

  if (number != MAGIC_NUMBER)
  {
    fclose(binary_file);
    free(m);
    return NULL;
  }

  // CONSTANT POOL ORIGIN - SKIP
  if (fread(numbuf, sizeof(uint8_t), 4, binary_file) != 4)
  {
    fclose(binary_file);
    destroy_ijvm(m);
    return NULL;
  }

  // CONSTANT POOL SIZE
  if (fread(numbuf, sizeof(uint8_t), 4, binary_file) != 4)
  {
    fclose(binary_file);
    destroy_ijvm(m);
    return NULL;
  }
  uint32_t constant_pool_size = read_uint32(numbuf);
  swap_uint32(constant_pool_size);
  m->constant_pool = (uint32_t *)malloc(constant_pool_size);

  // CONSTANT POOL DATA
  for (uint32_t i = 0; i < constant_pool_size / sizeof(word_t); i++)
  {
    if (fread(numbuf, sizeof(uint8_t), 4, binary_file) != 4)
    {
      fclose(binary_file);
      destroy_ijvm(m);
      return NULL;
    }
    m->constant_pool[i] = read_uint32(numbuf);
    swap_uint32(m->constant_pool[i]);
    m->constant_pool[i] = (word_t)m->constant_pool[i];
  }

  // TEXT ORIGIN - SKIP
  if (fread(numbuf, sizeof(uint8_t), 4, binary_file) != 4)
  {
    fclose(binary_file);
    destroy_ijvm(m);
    return NULL;
  }

  // TEXT SIZE
  if (fread(numbuf, sizeof(uint8_t), 4, binary_file) != 4)
  {
    fclose(binary_file);
    free(m->constant_pool);
    free(m);
    return NULL;
  }
  uint32_t text_size = read_uint32(numbuf);
  swap_uint32(text_size);
  m->text = (byte_t *)malloc(text_size);

  // TEXT DATA
  if (fread(m->text, sizeof(uint8_t), text_size, binary_file) != text_size)
  {
    fclose(binary_file);
    destroy_ijvm(m);
    return NULL;
  }

  // INITIALIZE VARIABLES FROM STRUCT
  // IJVM
  m->constant_pool_size = constant_pool_size;
  m->text_size = text_size;
  m->pc = 0;
  m->lv = 0;
  m->prev_pc = 0;
  m->prev_lv = 0;
  m->sp = 0;

  // STACK
  m->stack = (stack *)malloc(sizeof(stack));
  if (m->stack == NULL)
  {
    destroy_ijvm(m);
    return NULL;
  }
  m->stack->size = 0;
  m->stack->max = 256 * 2;
  m->stack->data = (word_t *)malloc(m->stack->max * sizeof(word_t));
  if (m->stack->data == NULL)
  {
    destroy_ijvm(m);
    return NULL;
  }
  initialize_stack(m);

  // HEAP
  initialize_heap(m);
  m->heap->size = 0;
  m->heap->max = 10;

  // Close the binary file
  fclose(binary_file);

  return m;
}

void destroy_ijvm(ijvm *m)
{
  if (m != NULL)
  {
    if (m->constant_pool != NULL)
    {
      free(m->constant_pool);
      m->constant_pool = NULL;
    }
    if (m->text != NULL)
    {
      free(m->text);
      m->text = NULL;
    }
    if (m->stack != NULL)
    {
      destroy_stack(m);
    }
    if (m->heap != NULL)
    {
      destroy_heap(m);
    }
    free(m);
    m = NULL;
  }
}

byte_t *get_text(ijvm *m)
{
  return m->text;
}

unsigned int get_text_size(ijvm *m)
{
  return m->text_size;
}

word_t get_constant(ijvm *m, int i)
{
  if (i < 0 || i >= m->constant_pool_size / 4)
  {
    fprintf(m->out, "Index out of bounds: %d\n", i);
    return 0;
  }
  return m->constant_pool[i];
}

unsigned int get_program_counter(ijvm *m)
{
  return m->pc;
}

word_t tos(ijvm *m)
{
  return top(m);
}

bool finished(ijvm *m)
{
  return m->pc >= m->text_size;
}

word_t get_local_variable(ijvm *m, int i)
{
  return m->stack->data[m->lv + i];
}

void step(ijvm *m)
{
  byte_t opcode = get_instruction(m);
  switch (opcode)
  {
  case OP_ERR:
  {
    error(m);
    break;
  }
  case OP_HALT:
  {
    halt(m);
    break;
  }
  case OP_BIPUSH:
  {
    bipush(m);
    break;
  }
  case OP_DUP:
  {
    dup(m);
    break;
  }
  case OP_IADD:
  {
    iadd(m);
    break;
  }
  case OP_IAND:
  {
    iand(m);
    break;
  }
  case OP_IOR:
  {
    ior(m);
    break;
  }
  case OP_ISUB:
  {
    isub(m);
    break;
  }
  case OP_NOP:
  {
    nop(m);
    break;
  }
  case OP_POP:
  {
    pop_op(m);
    break;
  }
  case OP_SWAP:
  {
    swap(m);
    break;
  }
  case OP_IN:
  {
    in(m);
    break;
  }
  case OP_OUT:
  {
    out(m);
    break;
  }
  case OP_GOTO:
  {
    goto_op(m);
    break;
  }
  case OP_IFEQ:
  {
    ifeq(m);
    break;
  }
  case OP_IFLT:
  {
    iflt(m);
    break;
  }
  case OP_IF_ICMPEQ:
  {
    if_icmpeq(m);
    break;
  }
  case OP_LDC_W:
  {
    ldc_w(m);
    break;
  }
  case OP_ILOAD:
  {
    iload(m);
    break;
  }
  case OP_ISTORE:
  {
    istore(m);
    break;
  }
  case OP_IINC:
  {
    iinc(m);
    break;
  }
  case OP_WIDE:
  {
    wide(m);
    break;
  }
  case OP_INVOKEVIRTUAL:
  {
    invokevirtual(m);
    break;
  }
  case OP_IRETURN:
  {
    ireturn(m);
    break;
  }
  case OP_TAILCALL:
  {
    tailcall(m);
    break;
  }
  case OP_NEWARRAY:
  {
    newarray(m);
    break;
  }
  case OP_IALOAD:
  {
    iaload(m);
    break;
  }
  case OP_IASTORE:
  {
    iastore(m);
    break;
  }
  default:
    fprintf(m->out, "Error: unknown opcode %d\n", opcode);
    m->pc = m->text_size;
    return;
  }
  return;
}

byte_t get_instruction(ijvm *m)
{
  return get_text(m)[get_program_counter(m)];
}

ijvm *init_ijvm_std(char *binary_path)
{
  return init_ijvm(binary_path, stdin, stdout);
}

void run(ijvm *m)
{
  while (!finished(m))
  {
    step(m);
  }
}

// BONUS ASSIGNMENT FUNCTIONS
int get_call_stack_size(ijvm *m)
{
  return m->stack->size;
}

bool is_heap_freed(ijvm *m, word_t reference)
{
  // TODO: IMPLEMENT ME
  return 0;
}
