/************************************************************************/
/* File Name : lc4.c                                                    */
/* Purpose   : This file contains the main() for this project           */
/*             main() will call the loader and disassembler functions   */
/*                                                                      */
/* Author(s) : Renjun Ma and Yitian Hou									*/
/************************************************************************/


#include <stdio.h>
#include "lc4_memory.h"
#include "lc4_loader.h"
#include <string.h>
#include "lc4_disassembler.h"

/* program to mimic pennsim loader and disassemble object files */

int main (int argc, char** argv) {
  
/* leave plenty of room for the filename */
  
      char filename[100];

    /**
     * main() holds the linked list &
     * only calls functions in other files
     */

    /* step 1: create head pointer to linked list: memory */
    /* do not change this line - there should no be malloc calls in main() */
    FILE* src_file;
    row_of_memory* memory = NULL ;
    row_of_memory** memoryPtr = &memory ;
    int errorID;
    
    /* step 2: determine filename, then open it */
    /*   TODO: extract filename from argv, pass it to open_file() */

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
    /*   TODO: call function & check for errors */
    errorID = parse_file (src_file, memoryPtr);
    if (errorID != 0) {
        delete_list(memoryPtr);
        return (errorID);
    }

    /* step 4: call function: reverse_assemble() in lc4_disassembler.c */
    /*   TODO: call function & check for errors */
    errorID = reverse_assemble(memory);
    if (errorID != 0) {
        printf("reverse assembling failed\n");
        delete_list(memoryPtr);
        return 3;
    };

    /* step 5: call function: print_list() in lc4_memory.c */
    /*   TODO: call function */

    print_list(memory);

    /* <EXTRA CREDIT> - Save the list as a .asm file */
    /* step 5.5: call function: write_asm_file() in lc4_disassembler.c */
    errorID = write_asm_file(filename, memory);
    if (errorID != 0) {
        printf("write to .asm file failed\n");
        delete_list(memoryPtr);
        return 3;
    };

    /* step 6: call function: delete_list() in lc4_memory.c */
    /*   TODO: call function & check for errors */
    errorID = delete_list(memoryPtr);
    if (errorID != 0) {
        printf("delete list failed\n");
        return 4;
    };

    /* only return 0 if everything works properly */
    return 0 ;
}
