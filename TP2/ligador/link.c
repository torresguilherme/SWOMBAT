#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "flink.h"

int main(int argc, char** argv){
         
    int i;
    int j;
    int tamTABE;
    int tamINST;
    int tamDADO;
    int offsetINST = 0;
    int offsetDADO = 0;
    FILE *fp;
    FILE *saida = fopen("saida.hex", "w+");
    FILE *memoria_de_instrucao = tmpfile();
    FILE *memoria_de_dados = tmpfile();
    FILE *tabela_de_simbolos = tmpfile();
    FILE *tabela = tmpfile();
    char aux[300];
    char *pnt;
    short int valor=0;
    char label[30];
    int endereco = 0;
    int complemento = 0;

//Primeira etapa: Le cada modulo.o e organiza as partes(tabela, dado e instrucao)
////////////////////////////////////////////////////////////////////////////////

    for ( i = 1; i < argc; i++ ) 
    {
       fp = fopen(argv[i], "r");
       
       //pega o tamanho de cada parte
       fscanf(fp, "%d", &tamTABE);

       fscanf(fp, "%d", &tamINST);

       fscanf(fp, "%d", &tamDADO);
       
       //Pega TABELA
       for ( j = 0; j < tamTABE; j++ ) 
       { 
              fseek(fp, 1, SEEK_CUR);
              fscanf(fp, "%[^\n]s", aux);

	      char *auxTable = resolveLabels(aux, tamINST, offsetINST,offsetDADO);
	      if(auxTable)
		{
	           fprintf(tabela_de_simbolos, "%s\n", auxTable);
		   free(auxTable);
		}

       }

       //Pega INSTRUCOES
       for ( j = 0; j < tamINST; j++ ) 
       { 
              fseek(fp, 1, SEEK_CUR);
              fscanf(fp, "%[^\n]s", aux);
	      fprintf(memoria_de_instrucao, "%s\n", aux);
       }

       //Pega DADOS
       for ( j = 0; j < tamDADO; j++ ) 
       { 
              fseek(fp, 1, SEEK_CUR);
              fscanf(fp, "%[^\n]s", aux);
	      fprintf(memoria_de_dados, "%s\n", aux);
       }

       //calcula o offset de todos os módulos pra tras
       offsetINST = offsetINST + tamINST;
       offsetDADO = offsetDADO + tamDADO;
 
       fclose( fp );
    } 

    //tudo da tabela que for dado, adiciona offsetINST
    fseek(tabela_de_simbolos, 0, SEEK_SET); //rebobina tabela_de_simbolos
    arrumaLabelDados(offsetINST,tabela_de_simbolos,tabela);

//SEGUNDA ETAPA: junta as memorias e conserta as referencias erradas
///////////////////////////////////////////////////////////////////

    fseek(memoria_de_instrucao, 0, SEEK_SET); //rebobina memoria_de_instrucao
    fseek(memoria_de_dados, 0, SEEK_SET); //rebobina memoria_de_dados

    //Printa as instruções com endereços certos na saída
    while( fscanf(memoria_de_instrucao, "%[^\n]s", aux) != EOF)
     {
       pnt = &aux[13];

           if ((*pnt) == ' ') //verifica se tem que arrumar a label
            {
                pnt++;
                i=0;
                while((*pnt) == '_' || isalpha(*pnt) || isdigit(*pnt))
	           	    {
	             	      label[i] = *pnt;
		                  i++;
		                 pnt++;
		              }
		  
                label[i] = ':';
                label[i+1] = 0;
                valor = SearchForLabel(label, tabela);

                complemento = valor + endereco;
                complemento = (~complemento);
                complemento = complemento & (255);
                sprintf(aux, ":01%04x00%02x%02x", endereco, valor, complemento);
                fprintf(saida, "%s\n", aux);
            }

       else
       {
              valor = hextoint(aux[9]) * 16 + hextoint(aux[10]);
              complemento = valor + endereco;
              complemento = (~complemento);
              complemento = complemento & (255);
              sprintf(aux, ":01%04x00%02x%02x", endereco, valor, complemento);
              fprintf(saida, "%s\n", aux);
                
       } 

       fseek(memoria_de_instrucao, 1, SEEK_CUR);
       endereco++;
    }
    
    //Printa os dados na saída
    while( fscanf(memoria_de_dados, "%[^\n]s", aux) != EOF) 
     {
                      valor = hextoint(aux[9]) * 16 + hextoint(aux[10]);
              complemento = valor + endereco;
              complemento = (~complemento);
              complemento = complemento & (255);
              sprintf(aux, ":01%04x00%02x%02x", endereco, valor, complemento);
              fprintf(saida, "%s\n", aux);
        fseek(memoria_de_dados, 1, SEEK_CUR);
        endereco++;
     }

    for(; endereco < 256; endereco++)
    {
        fprintf(saida, ":01%04x0000", endereco);
       complemento = endereco;
       complemento = (~complemento);
        fprintf(saida, "%02x\n", complemento & (255));
    }
    //end of file
    fprintf(saida, ":00000001ff");

        
        fclose(saida);
	fclose(memoria_de_instrucao);
	fclose(memoria_de_dados);
	fclose(tabela_de_simbolos);
    
     return 0;

}
