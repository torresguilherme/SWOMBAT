#ifndef FUNC_H_
#define FUNC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

short int TextToBinary(char* instruction, FILE *f,FILE *tabela_de_simbolos);
short int GetOpcode(char* inst);
short int SearchForLabel(char *label,FILE *tabela_de_simbolos);
char *TemLabel(char *inst);

#endif
