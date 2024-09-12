#include <stdio.h>  // for getc, printf
#include <stdlib.h> // malloc, free
#include "ijvm.h"
#include "util.h" // read this file for debug prints, endianness helper functions


// see ijvm.h for descriptions of the below functions

ijvm* init_ijvm(char *binary_path, FILE* input , FILE* output) 
{
  // do not change these first three lines
  ijvm* m = (ijvm *) malloc(sizeof(ijvm));
  m->in = input;
  m->out = output;
  
  // MAGIC NUMBER
  uint8_t numbuf[4];
  fread(numbuf, sizeof(uint8_t), 4, input);
  uint32_t number = read_uint32(numbuf);
  swap_uint32(number);

  if (number != MAGIC_NUMBER)
  {
    fclose(input);
    return NULL;
  }

  // CONSTANT POOL ORIGIN - SKIP
  fread(numbuf, sizeof(uint8_t), 4, input); 

  // CONSTANT POOL SIZE
  fread(numbuf, sizeof(uint8_t), 4, input);
  uint32_t constant_pool_size = read_uint32(numbuf);
  swap_uint32(constant_pool_size);
  m->constant_pool = (word_t *)malloc(constant_pool_size);

  // CONSTANT POOL DATA
  fread(m->constant_pool, sizeof(uint8_t), constant_pool_size, input);

  // TEXT ORIGIN - SKIP
  fread(numbuf, sizeof(uint8_t), 4, input);

  // TEXT SIZE
  fread(numbuf, sizeof(uint8_t), 4, input);
  uint32_t text_size = read_uint32(numbuf);
  swap_uint32(text_size);
  m->text = (byte_t *)malloc(text_size);

  // TEXT DATA
  fread(m->text, sizeof(uint8_t), text_size, input);

  // INITIALIZE VARIABLES FROM STRUCT
  m->constant_pool_size = constant_pool_size;
  m->text_size = text_size;

  return m;
}

void destroy_ijvm(ijvm* m) 
{
  if (m->constant_pool) free(m->constant_pool);
  if (m->text) free(m->text);
  free(m);
}

byte_t *get_text(ijvm* m) 
{
  return m->text;
}

unsigned int get_text_size(ijvm* m) 
{
  return m->text_size;
}

word_t get_constant(ijvm* m,int i) 
{
  if (i < 0 || i >= m->constant_pool_size / 4) {
        fprintf(stderr, "Index out of bounds: %d\n", i);
        return 0;
    }
  return m->constant_pool[i];
}

unsigned int get_program_counter(ijvm* m) 
{
  // TODO: implement me
  return 0;
}

word_t tos(ijvm* m) 
{
  // this operation should NOT pop (remove top element from stack)
  // TODO: implement me
  return -1;
}

bool finished(ijvm* m) 
{
  // TODO: implement me
  return false;
}

word_t get_local_variable(ijvm* m, int i) 
{
  // TODO: implement me
  return 0;
}

void step(ijvm* m) 
{
  // TODO: implement me

}

byte_t get_instruction(ijvm* m) 
{ 
  return get_text(m)[get_program_counter(m)]; 
}

ijvm* init_ijvm_std(char *binary_path) 
{
  return init_ijvm(binary_path, stdin, stdout);
}

void run(ijvm* m) 
{
  while (!finished(m)) 
  {
    step(m);
  }
}


// Below: methods needed by bonus assignments, see ijvm.h
// You can leave these unimplemented if you are not doing these bonus 
// assignments.

int get_call_stack_size(ijvm* m) 
{
   // TODO: implement me if doing tail call bonus
   return 0;
}


// Checks if reference is a freed heap array. Note that this assumes that 
// 
bool is_heap_freed(ijvm* m, word_t reference) 
{
   // TODO: implement me if doing garbage collection bonus
   return 0;
}
