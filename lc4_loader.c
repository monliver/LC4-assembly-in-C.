#include <stdio.h>
#include "lc4_memory.h"
#include <stdlib.h>

FILE* open_file(char* file_name)
{
    FILE *src_file;
    src_file = fopen(file_name, "rb");
    if (src_file == NULL) {
        printf("error: open_file() failed. \n");
        return NULL ;
    }
    return (src_file);
}

int parse_file (FILE* my_obj_file, row_of_memory** memory)
  
  
{    row_of_memory* temp = NULL;
    short int byte_read1;
    short int byte_read2;
    short unsigned int header;
    short unsigned int address;
    short unsigned int contents_length;
    short unsigned int contents;

    while (1) {
        // get header
        byte_read1 = fgetc(my_obj_file);
        if (byte_read1 == EOF) break;
        byte_read2 = fgetc(my_obj_file);
        if (byte_read2 == EOF) break;
        header = (byte_read1 << 8) + byte_read2;
    
        // get address
        byte_read1 = fgetc(my_obj_file);
        if (byte_read1 == EOF) break;
        byte_read2 = fgetc(my_obj_file);
        if (byte_read2 == EOF) break;
        address = (byte_read1 << 8) + byte_read2;

        // get contents_length
        byte_read1 = fgetc(my_obj_file);
        if (byte_read1 == EOF) break;
        byte_read2 = fgetc(my_obj_file);
        if (byte_read2 == EOF) break;
        contents_length = (byte_read1 << 8) + byte_read2;

        // allocate memory for a corresponding row_of_memory node for each instruction
        // check header type: CODE or DADA
        if (header == 0xCADE || header == 0xDADA ) {
            for (int i = 0; i < contents_length; i++) {
                byte_read1 = fgetc(my_obj_file);
                if (byte_read1 == EOF) break;
                byte_read2 = fgetc(my_obj_file);
                if (byte_read2 == EOF) break;
                contents = (byte_read1 << 8) + byte_read2;
                add_to_list (memory, address, contents);
                address++;
            }
        }
        // check header type: SYMBOL
        if (header == 0xC3B7) {
            temp = search_address (*memory, address);
            // if address not found in the linked list, create a new node with the address
            if (temp == NULL) {
                add_to_list (memory, address, 0);
                temp = search_address (*memory, address);
            }
            // if found the address in the linked list, free the memory space of the label
            else if (temp->label != NULL) {
                free(temp->label);
            }
            // add/update the label to the node    
            temp->label = malloc(contents_length + 1);
            for (int i = 0; i < contents_length; i++) {
                temp->label[i] = fgetc(my_obj_file);
            }

            temp->label[contents_length] = '\0';
        }
    }
    // close file
    int test = fclose(my_obj_file);
    if (test != 0) {
        printf("error: close file failed. \n");
        return 2;
    }
    return 0 ;
}

