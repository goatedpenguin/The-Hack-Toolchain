#include "codegen.h"
#include "parser.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

static char* decToBinStr16(uint16_t dec, int forceLeadingZero) {
    const int numBits = 16;
    char* binStr = malloc(numBits + 1);
    binStr[numBits] = '\0';

    for (int i = numBits - 1; i >= 0; i--) {
        binStr[i] = (dec & 1) ? '1' : '0';
        dec >>= 1;
    }
    // Force leading bit to 0 for A-instructions
    if (forceLeadingZero)
        binStr[0] = '0';

    return binStr;
}


static void buildCinstrTable(symTable* comp, symTable* dest, symTable* jump) {
    // COMP field (7 bits: a c1..c6)
    addSym(&comp, "0",     0b0101010);
    addSym(&comp, "1",     0b0111111);
    addSym(&comp, "-1",    0b0111010);
    addSym(&comp, "D",     0b0001100);
    addSym(&comp, "A",     0b0110000);
    addSym(&comp, "!D",    0b0001101);
    addSym(&comp, "!A",    0b0110001);
    addSym(&comp, "-D",    0b0001111);
    addSym(&comp, "-A",    0b0110011);
    addSym(&comp, "D+1",   0b0011111);
    addSym(&comp, "A+1",   0b0110111);
    addSym(&comp, "D-1",   0b0001110);
    addSym(&comp, "A-1",   0b0110010);
    addSym(&comp, "D+A",   0b0000010);
    addSym(&comp, "D-A",   0b0010011);
    addSym(&comp, "A-D",   0b0000111);
    addSym(&comp, "D&A",   0b0000000);
    addSym(&comp, "D|A",   0b0010101);

    // 'a' bit = 1 (use M instead of A)
    addSym(&comp, "M",     0b1110000);
    addSym(&comp, "!M",    0b1110001);
    addSym(&comp, "-M",    0b1110011);
    addSym(&comp, "M+1",   0b1110111);
    addSym(&comp, "M-1",   0b1110010);
    addSym(&comp, "D+M",   0b1000010);
    addSym(&comp, "D-M",   0b1010011);
    addSym(&comp, "M-D",   0b1000111);
    addSym(&comp, "D&M",   0b1000000);
    addSym(&comp, "D|M",   0b1010101);

    // DEST field (3 bits)
    addSym(&dest, "null",  0b000);
    addSym(&dest, "M",     0b001);
    addSym(&dest, "D",     0b010);
    addSym(&dest, "MD",    0b011);
    addSym(&dest, "A",     0b100);
    addSym(&dest, "AM",    0b101);
    addSym(&dest, "AD",    0b110);
    addSym(&dest, "AMD",   0b111);

    // JUMP field (3 bits)
    addSym(&jump, "null",  0b000);
    addSym(&jump, "JGT",   0b001);
    addSym(&jump, "JEQ",   0b010);
    addSym(&jump, "JGE",   0b011);
    addSym(&jump, "JLT",   0b100);
    addSym(&jump, "JNE",   0b101);
    addSym(&jump, "JLE",   0b110);
    addSym(&jump, "JMP",   0b111);
}

char* symToBinStr(instrType instr, ParsedPacket* packet) {    
    static symTable* comp = NULL;
    static symTable* dest = NULL;
    static symTable* jump = NULL;
    if (!comp && !dest && !jump) {
        comp = initTable();
        dest = initTable();
        jump = initTable();
        buildCinstrTable(comp, dest, jump);
    }

    char* res;
    switch (instr) {
        case A_INSTR: {
            res = decToBinStr16(packet->value, 1);
            break;
        }
        case C_INSTR: {
            // see hack opcode sheet to for reference
            int compField = packet->comp ? getSym(comp, packet->comp) : 0; // includes a bit
            int destField = packet->dest ? getSym(dest, packet->dest) : 0;
            int jumpField = packet->jump ? getSym(jump, packet->jump) : 0;
            uint16_t instr = (0b111 << 13) | (compField << 6) | (destField << 3) | jumpField; 

            res = decToBinStr16(instr, 0);
            break;
        }
        default:
            return NULL;
    }
    
    return res;
}