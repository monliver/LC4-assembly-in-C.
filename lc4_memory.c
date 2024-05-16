#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"


int add_to_list (row_of_memory** head,
		 short unsigned int address,
		 short unsigned int contents)
{
    row_of_memory* current_node = *head;

    /* check to see if there is already an entry for this address and update the contents.  no additional steps required in this case */
    while (current_node != NULL){
      if (current_node->address == address){
        current_node->contents = contents;
        return 0;
      }

      current_node = current_node->next;
    }

		row_of_memory* new_node = (row_of_memory*) malloc(sizeof(row_of_memory));
		// -1 if malloc fails
		if (new_node == NULL){
			return -1;
		}
		/* populate fields in newly allocated node w/ address&contents, NULL for label and assembly */
		/* do not malloc() storage for label and assembly here - do it in parse_file() and reverse_assemble() */
    new_node->address = address;
    new_node->label = NULL;
    new_node->contents = contents;
    new_node->assembly = NULL;
    new_node->next = NULL;
		
		/* if *head is NULL, node created is the new head of the list! */
    if (*head == NULL){
      *head = new_node;
      return 0;
    }
		
		/* otherwise, insert node into the list in address ascending order */
    current_node = *head;
    row_of_memory* previous_node = NULL;

    while (current_node != NULL && current_node->address < address){
      // find the location based on addresses
      previous_node = current_node;
      current_node = current_node->next;
    }

    if (previous_node == NULL){
      new_node->next = *head;
      *head = new_node;
    }
    else{
      // insert the node
      previous_node->next = new_node;
      new_node->next = current_node;
    }
		
		/* return 0 for success, -1 if malloc fails */

	return 0 ;
}


/*
 * delete entire linked list
 */
int delete_list    (row_of_memory** head )
{
	/* delete entire list node by node */
	/* set the list head pointer to NULL upon deletion */
  row_of_memory** this_node = head;
  row_of_memory* prev;

  while (*this_node != NULL){
    prev = (*this_node)->next;
    if((*this_node)->label != NULL){
      free((*this_node)->label);
    }
    if((*this_node)->assembly != NULL){
      free((*this_node)->assembly);
    }
    free(*this_node);
    *this_node = prev;
  }

	*head = NULL;

  return 0;
}


int write_asm_file (char* filename, row_of_memory* memory)
{
    // take the filename passed in the argument "filename", change the extension of the file into ".asm"
    char filename1[100];
    char filetype[100];
    sscanf(filename, "%[^.].%s", filename1, filetype);
    strcat(filename1, ".asm");
    
    // check if file can be written to, if not, print error msg and return immediately
    FILE *des_file;
    des_file = fopen(filename1, "w");
    if (des_file == NULL) {
        printf("ERROR 3: write_asm_file() failed. \n");
        return (3) ;
    }

    // initialize the prev_ADDR to make sure it is not 0x0000 - 1
    short unsigned int prev_ADDR = 0xFFFF;

    while (memory !=NULL) {
        // add .CODE and .ADDR in USER
        if (memory->address >= 0x0000 && memory->address <= 0x1FFF) {
            if (prev_ADDR + 1 != memory->address) {
                fprintf(des_file, "\n; ===== USER CODE SECTION =====\n");
                fprintf(des_file, ".CODE \n");
                fprintf(des_file, ".ADDR 0x%04X \n", memory->address);
            }
        }
        // add .DATA and .ADDR in USER
        if (memory->address >= 0x2000 && memory->address <= 0x7FFF) {
            if (prev_ADDR + 1 != memory->address) {
                fprintf(des_file, "\n; ===== USER DATA SECTION =====\n");
                fprintf(des_file, ".DATA \n");
                fprintf(des_file, ".ADDR 0x%04X \n", memory->address);
            }
        }
        // add .CODE and .ADDR in OS
        if (memory->address >= 0x8000 && memory->address <= 0x9FFF) {
            if (prev_ADDR + 1 != memory->address) {
                fprintf(des_file, "\n; ===== OS CODE SECTION =====\n");
                fprintf(des_file, ".OS \n");
                fprintf(des_file, ".CODE \n");
                fprintf(des_file, ".ADDR 0x%04X \n", memory->address);
            }
        }
        // add .DATA and .ADDR in OS
        if (memory->address >= 0xA000 && memory->address <= 0xFFFF) {
            if (prev_ADDR + 1 != memory->address) {
                fprintf(des_file, "\n; ===== OS DATA SECTION =====\n");
                fprintf(des_file, ".OS \n");
                fprintf(des_file, ".DATA \n");
                fprintf(des_file, ".ADDR 0x%04X \n", memory->address);
            }
        }
        // add LABEL
        if (memory->label != NULL) {
            fprintf(des_file, "\n%s \n", memory->label);

        }
        // add contents
        fprintf(des_file, "    %s\n", memory->assembly);
        
        // update prev_ADDR
        prev_ADDR = memory->address;

        // move to next node
        memory = memory->next;
    }

    fclose(des_file);
    return 0;
}
