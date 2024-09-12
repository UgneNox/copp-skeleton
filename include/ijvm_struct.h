
#ifndef IJVM_STRUCT_H
#define IJVM_STRUCT_H

#include <stdio.h> /* contains type FILE * */

#include "ijvm_types.h"
/**
 * All the state of your IJVM machine goes in this struct!
 **/

typedef struct STACK
{
  word_t *data;
  uint32_t max;
  uint32_t size;
} stack;

typedef struct ARRAY
{
  word_t *data;
  uint32_t size;
} array;

typedef struct HEAP
{
  array **arrays;
  uint32_t max;
  uint32_t size;
} heap;

typedef struct IJVM
{
  // do not changes these two variables
  FILE *in;  // use fgetc(ijvm->in) to get a character from in.
             // This will return EOF if no char is available.
  FILE *out; // use for example fprintf(ijvm->out, "%c", value); to print value to out

  // your variables go here
  uint8_t *text;
  uint32_t text_size;
  uint32_t *constant_pool;
  uint32_t constant_pool_size;
  word_t pc;
  word_t prev_pc;
  word_t lv;
  word_t prev_lv;
  word_t sp;
  stack *stack;
  heap *heap;
} ijvm;

#endif
