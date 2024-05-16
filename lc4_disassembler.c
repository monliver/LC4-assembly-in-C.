#include <stdio.h>
#include "lc4_memory.h"
#include <stdlib.h>
#include <string.h>

int reverse_assemble (row_of_memory* memory)
{
    /* binary constants should be proceeded by a 0b as in 0b011 for decimal 3 */

    // declare variables
    unsigned short int opcode;
    char tempAssembly[100];
    row_of_memory* tempNode;

    // disassemble .DATA section
    tempNode = memory;
    while (tempNode != NULL) {
        if ( (tempNode->address >= 0x2000 && tempNode->address <= 0x7FFF)
            || (tempNode->address >= 0xA000 && tempNode->address <= 0xFFFF) ) {
                // get IMM16
                short int IMM16 = tempNode->contents;
                // // generate the assembly string
                sprintf(tempAssembly, ".FILL #%d", IMM16);
                // update assembly field of this Node
                tempNode->assembly = malloc (strlen(tempAssembly) + 1);
                strcpy(tempNode->assembly, tempAssembly);
            }
        tempNode = tempNode->next;
    }


    // disassemble Branch (BRNZP)
    opcode = 0b0000;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int NZP = (tempNode->contents >> 9) & 0b111;
        short int IMM9 = (tempNode->contents >> 0) & 0b111111111;
        // if IMM9 is negative, sign extend to 16 bits
        if (((IMM9 >> 8) & 0b1) == 1) IMM9 = IMM9 | 0b1111111100000000;

        // get the label
        char* goToLabel = NULL;
        if (NZP != 0b000) {
            int goToAddress = tempNode->address + 1 + IMM9;
            row_of_memory* goToNode = search_address(memory, goToAddress);
            if (goToNode == NULL) {
                printf("ERROR 3: <Label> not found at address x%04X, current address at x%04X \n", goToAddress, tempNode->address);
                return 3;
            }
            goToLabel = goToNode->label;
        }

        // generate the assembly string
        switch (NZP) {
            case 0b000: sprintf(tempAssembly, "NOP"); break;
            case 0b100: sprintf(tempAssembly, "BRn   %s", goToLabel); break;
            case 0b110: sprintf(tempAssembly, "BRnz  %s", goToLabel); break;
            case 0b101: sprintf(tempAssembly, "BRnp  %s", goToLabel); break;
            case 0b010: sprintf(tempAssembly, "BRz   %s", goToLabel); break;
            case 0b011: sprintf(tempAssembly, "BRzp  %s", goToLabel); break;
            case 0b001: sprintf(tempAssembly, "BRp   %s", goToLabel); break;
            case 0b111: sprintf(tempAssembly, "BRnzp %s", goToLabel); break;
        }

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }

    // disassemble Arithmetic
    opcode = 0b0001;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rd = (tempNode->contents >> 9) & 0b111;
        unsigned short int Rs = (tempNode->contents >> 6) & 0b111;
        unsigned short int isIMM5 = (tempNode->contents >> 5) & 0b1;
        unsigned short int sub_opcode = (tempNode->contents >> 3) & 0b11;
        unsigned short int Rt = (tempNode->contents >> 0) & 0b111;
        short int IMM5 = (tempNode->contents >> 0) & 0b11111;
        // if IMM5 is negative, sign extend to 16 bits
        if (((IMM5 >> 4) & 0b1) == 1) IMM5 = IMM5 | 0b1111111111110000;

        // generate the assembly string
        if (isIMM5 == 1) {
            sprintf(tempAssembly, "ADD R%d, R%d, #%d", Rd, Rs, IMM5);
        } else {
            switch (sub_opcode) {
                case 0b00: sprintf(tempAssembly, "ADD R%d, R%d, R%d", Rd, Rs, Rt); break;
                case 0b01: sprintf(tempAssembly, "MUL R%d, R%d, R%d", Rd, Rs, Rt); break;
                case 0b10: sprintf(tempAssembly, "SUB R%d, R%d, R%d", Rd, Rs, Rt); break;
                case 0b11: sprintf(tempAssembly, "DIV R%d, R%d, R%d", Rd, Rs, Rt); break;
            }
        }

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }

    // disassemble Compare
    opcode = 0b0010;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rs = (tempNode->contents >> 9) & 0b111;
        unsigned short int Rt = (tempNode->contents >> 0) & 0b111;
        unsigned short int sub_opcode = (tempNode->contents >> 7) & 0b11;
        short int IMM7 = (tempNode->contents >> 0) & 0b1111111;
        unsigned short int UIMM7 = (tempNode->contents >> 0) & 0b1111111;
        // if IMM7 is negative, sign extend to 16 bits
        if (((IMM7 >> 6) & 0b1) == 1) IMM7 = IMM7 | 0b1111111111000000;

        // generate the assembly string
        switch (sub_opcode) {
            case 0b00: sprintf(tempAssembly, "CMP   R%d, R%d", Rs, Rt); break;
            case 0b01: sprintf(tempAssembly, "CMPU  R%d, R%d", Rs, Rt); break;
            case 0b10: sprintf(tempAssembly, "CMPI  R%d, #%d", Rs, IMM7); break;
            case 0b11: sprintf(tempAssembly, "CMPIU R%d, #%d", Rs, UIMM7); break;
        }

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }


    // disassemble JSR
    opcode = 0b0100;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rs = (tempNode->contents >> 6) & 0b111;
        unsigned short int sub_opcode = (tempNode->contents >> 11) & 0b1;
        short int IMM11 = (tempNode->contents >> 0) & 0b11111111111;
        // if IMM11 is negative, sign extend to 16 bits
        if (((IMM11 >> 10) & 0b1) == 1) IMM11 = IMM11 | 0b1111110000000000;

        // get the label
        int goToAddress = (tempNode->address & 0x8000) | (IMM11 << 4);
        row_of_memory* goToNode = search_address(memory, goToAddress);
        if (goToNode == NULL) {
            printf("ERROR 3: <Label> not found at address x%04X, current address at x%04X \n", goToAddress, tempNode->address);
            return 3;
        }
        char* goToLabel = goToNode->label;

        // generate the assembly string
        switch (sub_opcode) {
            case 0b1: sprintf(tempAssembly, "JSR %s", goToLabel); break;
            case 0b0: sprintf(tempAssembly, "JSRR R%d", Rs); break;
        }

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }


    // disassemble Logical
    opcode = 0b0101;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rd = (tempNode->contents >> 9) & 0b111;
        unsigned short int Rs = (tempNode->contents >> 6) & 0b111;
        unsigned short int isIMM5 = (tempNode->contents >> 5) & 0b1;
        unsigned short int sub_opcode = (tempNode->contents >> 3) & 0b11;
        unsigned short int Rt = (tempNode->contents >> 0) & 0b111;
        short int IMM5 = (tempNode->contents >> 0) & 0b11111;
        // if IMM5 is negative, sign extend to 16 bits
        if (((IMM5 >> 4) & 0b1) == 1) IMM5 = IMM5 | 0b1111111111110000;

        // generate the assembly string
        if (isIMM5 == 1) {
            sprintf(tempAssembly, "AND R%d, R%d, #%d", Rd, Rs, IMM5);
        } else {
            switch (sub_opcode) {
                case 0b00: sprintf(tempAssembly, "AND R%d, R%d, R%d", Rd, Rs, Rt); break;
                case 0b01: sprintf(tempAssembly, "NOT R%d, R%d", Rd, Rs); break;
                case 0b10: sprintf(tempAssembly, "OR R%d, R%d, R%d", Rd, Rs, Rt); break;
                case 0b11: sprintf(tempAssembly, "XOR R%d, R%d, R%d", Rd, Rs, Rt); break;
            }
        }
        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }


    // disassemble Load
    opcode = 0b0110;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rd = (tempNode->contents >> 9) & 0b111;
        unsigned short int Rs = (tempNode->contents >> 6) & 0b111;
        short int IMM6 = (tempNode->contents >> 0) & 0b111111;
        // if IMM6 is negative, sign extend to 16 bits
        if (((IMM6 >> 5) & 0b1) == 1) IMM6 = IMM6 | 0b1111111111100000;

        // generate the assembly string
        sprintf(tempAssembly, "LDR R%d, R%d, #%d", Rd, Rs, IMM6);

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }

    // disassemble Store
    opcode = 0b0111;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rt = (tempNode->contents >> 9) & 0b111;
        unsigned short int Rs = (tempNode->contents >> 6) & 0b111;
        short int IMM6 = (tempNode->contents >> 0) & 0b111111;
        // if IMM6 is negative, sign extend to 16 bits
        if (((IMM6 >> 5) & 0b1) == 1) IMM6 = IMM6 | 0b1111111111100000;

        // generate the assembly string
        sprintf(tempAssembly, "STR R%d, R%d, #%d", Rt, Rs, IMM6);

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }

    // disassemble Return
    opcode = 0b1000;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // generate the assembly string
        sprintf(tempAssembly, "RTI");

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }



    // disassemble Const
    opcode = 0b1001;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rd = (tempNode->contents >> 9) & 0b111;
        short int IMM9 = (tempNode->contents >> 0) & 0b111111111;
        // if IMM9 is negative, sign extend to 16 bits
        if (((IMM9 >> 8) & 0b1) == 1) IMM9 = IMM9 | 0b1111111100000000;

        // generate the assembly string
        sprintf(tempAssembly, "CONST R%d, #%d", Rd, IMM9);

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }

    // disassemble Shift
    opcode = 0b1010;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rd = (tempNode->contents >> 9) & 0b111;
        unsigned short int Rs = (tempNode->contents >> 6) & 0b111;
        unsigned short int Rt = (tempNode->contents >> 0) & 0b111;
        unsigned short int sub_opcode = (tempNode->contents >> 4) & 0b11;
        unsigned short int UIMM4 = (tempNode->contents >> 0) & 0b1111;

        // generate the assembly string
        switch (sub_opcode) {
                case 0b00: sprintf(tempAssembly, "SLL R%d, R%d, #%d", Rd, Rs, UIMM4); break;
                case 0b01: sprintf(tempAssembly, "SRA R%d, R%d, #%d", Rd, Rs, UIMM4); break;
                case 0b10: sprintf(tempAssembly, "SRL R%d, R%d, #%d", Rd, Rs, UIMM4); break;
                case 0b11: sprintf(tempAssembly, "MOD R%d, R%d, R%d", Rd, Rs, Rt); break;
            }

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }



    // disassemble Jump
    opcode = 0b1100;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rs = (tempNode->contents >> 6) & 0b111;
        unsigned short int sub_opcode = (tempNode->contents >> 11) & 0b1;
        short int IMM11 = (tempNode->contents >> 0) & 0b11111111111;
        // if IMM11 is negative, sign extend to 16 bits
        if (((IMM11 >> 10) & 0b1) == 1) IMM11 = IMM11 | 0b1111110000000000;

        // get the label
        char* goToLabel = NULL;
        if (sub_opcode != 0b0) {
            int goToAddress = tempNode->address + 1 + IMM11;
            row_of_memory* goToNode = search_address(memory, goToAddress);
            if (goToNode == NULL) {
                printf("ERROR 3: <Label> not found at address x%04X, current address at x%04X \n", goToAddress, tempNode->address);
                return 3;
            }
            goToLabel = goToNode->label;
        }

        // generate the assembly string
        switch (sub_opcode) {
                case 0b0: sprintf(tempAssembly, "JMPR R%d", Rs); break;
                case 0b1: sprintf(tempAssembly, "JMP %s", goToLabel); break;
            }

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }

    // disassemble Hiconst
    opcode = 0b1101;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int Rd = (tempNode->contents >> 9) & 0b111;
        unsigned short int UIMM8 = (tempNode->contents >> 0) & 0b11111111;

        // generate the assembly string
        sprintf(tempAssembly, "HICONST R%d, #%d", Rd, UIMM8);

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }

    // disassemble Trap
    opcode = 0b1111;
    tempNode = search_opcode (memory, opcode);
    while (tempNode != NULL) {
        // parse 16-bit binary
        unsigned short int UIMM8 = (tempNode->contents >> 0) & 0b11111111;

        // generate the assembly string
        sprintf(tempAssembly, "TRAP #%d", UIMM8);

        // update assembly field of this Node
        tempNode->assembly = malloc (strlen(tempAssembly) + 1);
        strcpy(tempNode->assembly, tempAssembly);

        // go to next available Node
        tempNode = search_opcode (memory, opcode);
    }
    
    return 0 ;
}

