#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//lembrete: Nos quatro últimos caracteres da segunda linha, os dois primeiros indicam os labels

int main(int argc, char** argv){
    printf("Oi\n");
    FILE **arq = (FILE **) malloc (1*sizeof(FILE *));
    char aux[300];

    int argv_position = 1;
    
    //buscar nomes dos arquivos
    while (argv[argv_position] != '\0'){
        arq[argv_position-1] = fopen(argv[argv_position], "r+");
        printf("%s\n", argv[argv_position]);
        arq = (FILE **) realloc (arq, (argv_position+1)*sizeof(FILE *));
        argv_position++;
    }

    //memórias necessárias
    FILE *memoria_de_instrucao = tmpfile();
    FILE *memoria_de_dados = tmpfile();
    FILE *tabela_de_simbolos = tmpfile();

    //contagem arquivo
    int t_tab[argv_position-1], t_ins[argv_position-1], t_dat[argv_position-1];
    int tab_acum = 0, ins_acum = 0, dat_acum = 0;
    int x, y;
    char c;

    printf("Oi\n");
    for (x=1; x<argv_position; x++){
        fscanf(arq[x-1], "%d", &t_tab[x-1]);
        fscanf(arq[x-1], "%d", &t_ins[x-1]);
        fscanf(arq[x-1], "%d", &t_dat[x-1]);
        
        printf("%d %d %d\n", t_tab[x-1], t_ins[x-1], t_dat[x-1]);

        for (y=0; y<t_tab[x-1]+1; y++){
            fscanf(arq[x-1], "%c", &c);
            while (c != '\n'){
                fprintf(tabela_de_simbolos, "%c", c);
                printf("%c", c);
                fscanf(arq[x-1], "%c", &c);
            }
            printf("\n");
        }
        fprintf(tabela_de_simbolos, "\n");

        for (y=0; y<t_ins[x-1]; y++){
            int cont=0, pass=0;
            fscanf(arq[x-1], "%c", &c);
            while (c != '\n'){
                if ((cont<3)||(cont>7)){
                    fprintf(memoria_de_instrucao, "%c", c);
                    printf("%c", c);
                }else{
                    if (pass==0){
                        fprintf(memoria_de_instrucao, "%04x", ins_acum);
                        printf("%04x", ins_acum++);
                        pass=1;
                    }
                }
                cont++;
                fscanf(arq[x-1], "%c", &c);
            }
            fprintf(memoria_de_instrucao, "\n");
            printf("\n");
        }
        printf("\n");

        for (y=0; y<t_dat[x-1]; y++){
            int cont=0, pass=0;
            fscanf(arq[x-1], "%c", &c);
            while (c != '\n'){
                if ((cont<3)||(cont>7)){
                    fprintf(memoria_de_dados, "%c", c);
                    printf("%c", c);
                }else{
                    if (pass==0){
                        fprintf(memoria_de_dados, "%04x", ins_acum);
                        printf("%04x", ins_acum++);
                        pass=1;
                    }
                }
                cont++;
                fscanf(arq[x-1], "%c", &c);
            }
            fprintf(memoria_de_dados, "\n");
            printf("\n");
        }
    }

return 0;
}
