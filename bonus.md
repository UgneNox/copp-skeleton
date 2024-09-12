Please describe any bonuses you implemented (this file is included in your submission)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Tail Call:
Implemented the TAILCALL instruction by using the same code/logic used in my INVOKEVIRTUAL instruction. The main differences are that I did not adjust the base pointer lv and overwrote the initial INVOKEVIRTUAL by popping the values on the stack and putting it at an offset from the unchaged base pointer. Then, by comparing the stack pointer to where the TAILCALL frame neds, I realign the stack pointer by popping excess values. I the push the previous pc and previous lv and move on.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Heap Memory:
Heap memory - I spoke to my TA Vlad about this and decided to take his advice and implement an array of structs, each of which holds the metdata (size, eventually freed state) of every array on the heap. These two structs can be found in the ijvm_struct.h file, the ARRAY struct has the size, data and freed state of the array, whereas the HEAP struct has a pointer to array pointers (**arrays), which essentially creates the array of array structs. It also has the maximum capacity of the heap as well as it's current size. In the ijvm.c file, there are 3 helper functions - one to initialize the heap, one to destroy it and one to make a new array for the NEWARRAY opcode.


