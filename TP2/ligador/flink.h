#ifndef FUN_H_
#define FUN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *resolveLabels(char *aux1, int tamINST1, int offsetINST1, int offsetDADO1);
int arrumaLabelDados(int offsetINST2,FILE *tabela_de_simbolos1);
short int SearchForLabel(char *label,FILE *tabela_de_simbolos);

#endif
