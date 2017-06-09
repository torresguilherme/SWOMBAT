#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char** argv){
    FILE **arq = (FILE **) malloc (1*sizeof(FILE *));
    char aux[300];

    int argv_position = 1;
    
    //buscar nomes dos arquivos
    while (argv[argv_position] != '\0'){
        arq[argv_position-1] = fopen(argv[argv_position], "r+");
        arq = (FILE **) realloc (arq, (argv_position+1)*sizeof(FILE *));
        argv_position++;
    }

    //memórias necessárias
    FILE *memoria_de_instrucao = tmpfile();
    FILE *memoria_de_dados = tmpfile();
    FILE *tabela_de_simbolos = tmpfile();

    //contagem arquivo
    int t_tab[argv_position-1], t_ins[argv_position-1], t_dat[argv_position-1];
    int x, y;

    for (x=argv_position; x>0; x--){
        fscanf(arq[x-1], "%d", t_tab[x-1]);
        fscanf(arq[x-1], "%d", t_ins[x-1]);
        fscanf(arq[x-1], "%d", t_dat[x-1]);

        for (y=0; y<t_tab[x-1]; y++){
            fscanf(arq[x-1], "%[^\n]s", aux);
            fprintf(tabela_de_simbolos, "%s\n", aux);
        }
        fprintf(tabela_de_simbolos, "\n");

        for (y=0; y<t_ins[x-1]; y++){
            fscanf(arq[x-1], "%[^\n]s", aux);
            fprintf(memoria_de_instrucao, "%s\n", aux);
        }

        for (y=0; y<t_dat[x-1]; y++){
            fscanf(arq[x-1], "%[^\n]s", aux);
            fprintf(memoria_de_dados, "%s\n", aux);
        }
    }

    

return 0;
}

