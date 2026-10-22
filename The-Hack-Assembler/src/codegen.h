#ifndef CODEGEN
#define CODEGEN
#include "parser.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char* symToBinStr(instrType instr, ParsedPacket* packet); 
// uint16_t symToBinB(instrType instr, ParsedPacket* packet);  // this is if we truly want to output a bin the above is a textual rep. of the machine code

#endif