/************************************************************************/
/* File Name : lc4_memory.c                                             */
/* Purpose   : This file implements the linked_list helper functions    */
/*             to manage the LC4 memory                                 */
/*                                                                      */
/* Author(s) : Renjun Ma                                                */
/************************************************************************/

#include <stdio.h>
#include "lc4_memory.h"
#include <stdlib.h>
#include <string.h>

/*
 * adds a new node to a linked list pointed to by head, 
 * return 0 for success, -1 if malloc fails
 */
int add_to_list (row_of_memory** head,
         short unsigned int address,
         short unsigned int contents)
{

    /* check to see if there is already an entry for this address and update the contents.  no additional steps required in this case */
    row_of_memory* temp_ptr = *head ;
    while (temp_ptr != NULL) {
        if (temp_ptr->address == address) {
            temp_ptr->contents = contents;
            return 0;
        }
        temp_ptr = temp_ptr->next ;
    }
    
    /* allocate memory for a single node */
    row_of_memory* new_row = malloc (sizeof(row_of_memory));
    if (new_row == NULL) {
        printf("ERROR -1: Memory allocate failed. \n") ;
        return -1 ;
    }

    /* populate fields in newly allocated node w/ address&contents, NULL for label and assembly */
    /* do not malloc() storage for label and assembly here - do it in parse_file() and reverse_assemble() */    
    new_row->address = address ;
    new_row->label = NULL ;
    new_row->contents = contents ;
    new_row->assembly = NULL ;
    new_row->next = NULL ;

    /* if *head is NULL, node created is the new head of the list! */
    if (*head == NULL) {
        *head = new_row ;
        return 0;
    }

    /* otherwise, insert node into the list in address ascending order */
    temp_ptr = *head ;
    row_of_memory* prev_ptr = NULL;

    /* insert new node as the first node*/
    if (temp_ptr->address > address) {
        *head = new_row ;
        new_row->next = temp_ptr;
        return 0;
    }

    /*insert new node in address ascending order*/
    while (temp_ptr != NULL && temp_ptr->address < address) {
        prev_ptr = temp_ptr;
        temp_ptr = temp_ptr->next;
    }
    prev_ptr->next = new_row;
    new_row->next = temp_ptr;
    return 0 ;
}



/*
 * search linked list by address field, returns node if found
 */
row_of_memory* search_address (row_of_memory* head,
                   short unsigned int address )
{
    /* traverse linked list, searching each node for "address"  */

    /* return pointer to node in the list if item is found */

    /* return NULL if list is empty or if "address" isn't found */
    while ((head != NULL) && (head->address != address)) 
        head = head->next;

    return (head) ;
}

/*
 * search linked list by opcode field, returns node if found
 */
row_of_memory* search_opcode  (row_of_memory* head,
                      short unsigned int opcode  )
{
    /* opcode parameter is in the least significant 4 bits of the short int and ranges from 0-15 */
        /* see assignment instructions for a detailed description */
    
    /* traverse linked list until node is found with matching opcode in the most significant 4 bits
       AND "assembly" field of node is NULL */

    /* return pointer to node in the list if item is found */

    /* return NULL if list is empty or if no matching nodes */
    while ((head != NULL)) {
        if (((head->contents >> 12) == opcode) 
        && (head->assembly == NULL)
        && (head->address <= 0x1FFF || (head->address >= 0x8000 && head->address <= 0x9FFF)) ) {
            return (head);
        }
        head = head->next;
    }
    
    return NULL ;
}


void print_list (row_of_memory* head )
{
    /* make sure head isn't NULL */
    if (head == NULL) {
        printf("Linked list is empty");
    } else {
        /* print out a header */
        char label[] = "<label>";
        char address[] = "<address>";
        char contents[] = "<contents>";
        char assembly[] = "<assembly>";
        char empty[] = "";

        printf("%-20s %-20s %-20s %-20s \n", label, address, contents, assembly);
    
        /* don't print assembly directives for non opcode 1 instructions if you are doing extra credit */

        /* traverse linked list, print contents of each node */

        while (head != NULL) {
            // print label
            if (head->label == NULL) {
                printf("%-20s ", empty);
            } else {
                printf("%-20s ", head->label);
            }
            // print address and contents
            printf("%-20.04X %-20.04X ", head->address, head->contents);
            
            // print assembly
            int isOnlyPrintArithmetic = 1;
            if (head->assembly == NULL) {
                printf("%-20s \n", empty);
            } else if (isOnlyPrintArithmetic && ((head->contents >> 12) != 0b0001)) {
                printf("%-20s \n", empty);
            } else if (isOnlyPrintArithmetic && ((head->address >= 0x2000 && head->address <= 0x7FFF)
            || (head->address >= 0xA000 && head->address <= 0xFFFF))) {
                printf("%-20s \n", empty);
            } else {
                printf("%-20s \n", head->assembly);
            }
            head = head->next;
        }
    }
    
    return ;
}

/*
 * delete entire linked list
 */
int delete_list (row_of_memory** head )
{
    /* delete entire list node by node */
    /* set the list head pointer to NULL upon deletion */

    // make temp_ptr as head node
    row_of_memory* temp_ptr1 = *head;
    row_of_memory* temp_ptr2 = NULL;

    while (temp_ptr1 != NULL) {
        // store next node to temp_ptr2
        temp_ptr2 = temp_ptr1->next;
        // deallocate all memory of the row_of_memory
        if (temp_ptr1->label != NULL) {
            free(temp_ptr1->label);
        }
        if (temp_ptr1->assembly != NULL) {
            free(temp_ptr1->assembly);
        }
        free (temp_ptr1);
        // move temp_ptr1 to next node
        temp_ptr1 = temp_ptr2;
    }

    *head = NULL;
    return 0 ;
}



/*
 * EXTRA CREDIT: save entire linked list to .asm file
 */
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
