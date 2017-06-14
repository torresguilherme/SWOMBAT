#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//lembrete: Nos quatro últimos caracteres da segunda linha, os dois primeiros indicam os labels

int main(int argc, char** argv){
    FILE **arq = (FILE **) malloc (1*sizeof(FILE *));
    FILE *saida = fopen(argv[1], "w+");
    char aux[300];

    int argv_position = 2;
    
    //buscar nomes dos arquivos
    while (argv[argv_position] != '\0'){
        arq[argv_position-2] = fopen(argv[argv_position], "r+");
        arq = (FILE **) realloc (arq, (argv_position+1)*sizeof(FILE *));
        argv_position++;
    }
    argv_position--;

    //memórias necessárias
    FILE *memoria_de_instrucao = fopen("mem_inst.txt", "w+");
    FILE *memoria_de_dados = fopen("mem_dado.txt", "w+");
    FILE *tabela_de_simbolos = fopen("mem_simb.txt", "w+");

    //contagem arquivo
    int t_tab[argv_position-1], t_ins[argv_position-1], t_dat[argv_position-1];
    int tab_acum, ins_acum = 0, dat_acum = 0;
    int x, y, d;
    char c;

    for (x=1; x<argv_position; x++){
        fscanf(arq[x-1], "%d", &t_tab[x-1]);
        fscanf(arq[x-1], "%d", &t_ins[x-1]);
        fscanf(arq[x-1], "%d", &t_dat[x-1]);
        dat_acum += t_ins[x-1];
    }


    for (x=1; x<argv_position; x++){
        d = 0;
        for (y=0; y<t_tab[x-1]; y++){
            fscanf(arq[x-1], "%s %d", aux, &d);
            if (d >= t_ins[x-1]){ 
                fprintf(tabela_de_simbolos, "%s %d\n", aux, (d-t_ins[x-1])+dat_acum);
            }else{ 
                fprintf(tabela_de_simbolos, "%s %d\n", aux, d+ins_acum);
            }
            tab_acum++;
        }
        ins_acum += t_ins[x-1];
        dat_acum += t_dat[x-1];
    }
    fclose(tabela_de_simbolos);
    
    ins_acum = 0;
    char label[30];
    for (x=1; x<argv_position; x++){
        for (y=0; y<t_ins[x-1]; y++){
            int cont=0, pass=0, label_control=-1;
            label[0] = '\0';
            fscanf(arq[x-1], "%c", &c);

            while (c != '\n'){
                if ((c == '_')||(pass>0)){
                    if (pass<1){ pass=1; }
                    label[pass-1] = c;
                    pass++;
                }else{
                    aux[cont] = c;
                }
                cont++;
                fscanf(arq[x-1], "%c", &c);
            }
            aux[cont] = '\0';

            if (label[0] != '\0'){
                label[pass-1]='\0';
            	int w, d;
                char apoio[30];
                
                tabela_de_simbolos = fopen("mem_simb.txt", "r+");
                for (w=0; w<tab_acum; w++){
                    fscanf(tabela_de_simbolos, "%s %d", apoio, &d);
                    if ((strcmp(label, apoio)==0)&&(d!=-1)){
                        label_control = d;
                        w = tab_acum;
                    }
                }
                fclose(tabela_de_simbolos);
            }

            cont=0; pass=0;
            while((aux[cont]!='\0')&&(aux[cont]!='\n')){
                if ((cont>2)&&(cont<7)){
                    if (pass==0){
                        fprintf(memoria_de_instrucao, "%04x", ins_acum++);
                        pass=1;
                    }
                }else{
                    if (((cont>6)&&(cont<11))&&(label_control!=-1)){
                        if (pass==1){
                            fprintf(memoria_de_instrucao, "%04x", label_control);
                            pass=2;
                        }
                    }else{
                        fprintf(memoria_de_instrucao, "%c", aux[cont]);
                    }
                }
                cont++;
            }
            
            fprintf(memoria_de_instrucao, "\n");
        }
    }
    fclose(memoria_de_instrucao);

    for (x=1; x<argv_position; x++){
        for (y=0; y<t_dat[x-1]; y++){
            int cont=0, pass=0;
            fscanf(arq[x-1], "%c", &c);
            while (c != '\n'){
                if ((cont<3)||(cont>7)){
                    fprintf(memoria_de_dados, "%c", c);
                }else{
                    if (pass==0){
                        fprintf(memoria_de_dados, "%04x", ins_acum++);
                        pass=1;
                    }
                }
                cont++;
                fscanf(arq[x-1], "%c", &c);
            }
            fprintf(memoria_de_dados, "\n");
        }
    }
    fclose(memoria_de_dados);

    for (x=1; x<argv_position; x++){
        fclose(arq[x-1]);
    }
    fclose(arq);
    fclose(saida);

return 0;
}
