#include <stdio.h>
#include "lc4_memory.h"
#include "lc4_loader.h"
#include <string.h>
#include "lc4_disassembler.h"

int main (int argc, char** argv) {
  
      char filename[100];

    /* create head pointer to linked list: memory */
    FILE* src_file;
    row_of_memory* memory = NULL ;
    row_of_memory** memoryPtr = &memory ;
    int errorID;
    
    /* determine filename, then open it */
    // if not exact two args are passed in, print error msg and return immediately
    if (argc != 2) {
        errorID = 1;
        printf("error1: usage: ./lc4 <object_file.obj>\n");
        return (errorID);
    }
    
    strcpy(filename, argv[1]);
    src_file = open_file(filename);
    if (src_file == NULL) return 1;
    
    /* step 3: call function: parse_file() in lc4_loader.c */
    errorID = parse_file (src_file, memoryPtr);
    if (errorID != 0) {
        delete_list(memoryPtr);
        return (errorID);
    }

    /* call function: reverse_assemble() in lc4_disassembler.c */
    errorID = reverse_assemble(memory);
    if (errorID != 0) {
        printf("reverse assembling failed\n");
        delete_list(memoryPtr);
        return 3;
    };

    /* call function: print_list() in lc4_memory.c */
    print_list(memory);

    /* call function: write_asm_file() in lc4_disassembler.c */
    errorID = write_asm_file(filename, memory);
    if (errorID != 0) {
        printf("write to .asm file failed\n");
        delete_list(memoryPtr);
        return 3;
    };

    /* call function: delete_list() in lc4_memory.c */
    errorID = delete_list(memoryPtr);
    if (errorID != 0) {
        printf("delete list failed\n");
        return 4;
    };

    /* only return 0 if everything works properly */
    return 0 ;
}
