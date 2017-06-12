#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//lembrete: Nos quatro últimos caracteres da segunda linha, os dois primeiros indicam os labels

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

short int SearchForLabel(char *label,FILE *tabela_de_simbolos)
{
  //busca linearmente por uma label na matriz de símbolos comecando do inicio
  //gera o endereco na memoria de instruçoes e guarda na chamada da operação
  
  fseek(tabela_de_simbolos, 0, SEEK_SET);
  
  char current[100]; //primeira palavra na linha
  char trash[300]; //guarda o resto da linha
  int labeladdr = 0; //endereço da label
  int found = 0; //sinaliza se a label for encontrada ou nao
  short int lines = 0; //o numero da linha no codigo onde a label for encontrada
  
  while(!found)
  {
    fscanf(tabela_de_simbolos, "%s", current);
    if(!strcmp(label, current)) //compara com a label que se quer encontrar
    {
      found = 1;
      fscanf(tabela_de_simbolos, "%d", &labeladdr);
      
    }
    
    else //nesse caso, ainda nao encontrou
    {
      fscanf(tabela_de_simbolos, "%[^\n]s", trash);
      if(isalpha(current[0]) || current[0] == '_')
      {
        lines++;
      }
    }
  }
  
  return labeladdr;
}
