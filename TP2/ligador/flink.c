#include "flink.h"

char *resolveLabels(char *aux1, int tamINST1, int offsetINST1, int offsetDADO1)
{
        //se for .extern, ignora e não coloca na tabela
        //se o endereço for menor que tamINST, só adiciona o offsetINST
        //se for maior, é pq é dado. Subtrai tamINST. Adiciona offsetDADO. Marca com "d" de dado
	
        int counter = 0;
	char *ret = malloc(sizeof(char) * 100);
	char *p = &aux1[0];
        int labeladdr = 0;
	char label[100];
        char addr[4];

        //pega a label
        while(*p != '_')
		{
			p++;
		}

	while((*p == '_') || isalpha(*p) || isdigit(*p))
		{
			label[counter] = *p;
			counter++;
			p++;
		}
  
        label[counter] = ':';
        label[counter+1] = 0;

        //pega labeladdr
        while(!isdigit(*p))
		{
                        if (*p == 'e') // se for extern, não salva na tabela
                          {
			      free(ret);
			      return NULL;
		          }
			p++;
		}

        counter =0;

        while(isdigit(*p))
		{
			addr[counter] = *p;
			counter++;
			p++;
		}
        labeladdr = atoi(addr);
       
	if (labeladdr < tamINST1)
                {
                        labeladdr = labeladdr + offsetINST1;
 	                sprintf(ret,"%s %d", label, labeladdr);
                }
       
        else {
                     labeladdr =  labeladdr - tamINST1 + offsetDADO1;
                     sprintf(ret,"%s d%d", label, labeladdr);
             }

	return ret;
}

int arrumaLabelDados(int offsetINST2,FILE *tabela_de_simbolos1, FILE *tabela1)
{
       int counter = 0;
       char auxx[300];
       char *pnt;
       int labeladdr = 0;
       char addr[4];
       char label[30];

      fscanf(tabela_de_simbolos1, "%[^\n]s", auxx);
      while( fscanf(tabela_de_simbolos1, "%[^\n]s", auxx) != EOF)
      {
           pnt = &auxx[0];
           counter =0;
           //pega a label
           while(*pnt != '_')
		{
			pnt++;
		}

	   while((*pnt == '_') || isalpha(*pnt) || isdigit(*pnt))
		{
			label[counter] = *pnt;
			counter++;
			pnt++;
		}
  
           label[counter] = ':';
           label[counter+1] = 0;

           printf("%s\n",label);
           printf("%d\n",offsetINST2);
           pnt++;
           pnt++;

           //se for dado
           if (*pnt == 'd')
              {

                //pega labeladdr
                while(!isdigit(*pnt))
				{
					pnt++;
				}

                counter =0;

                while(isdigit(*pnt))
				{
					addr[counter] = *pnt;
					counter++;
					pnt++;
				}
                labeladdr = atoi(addr);

                //acrescenta o offset
                labeladdr =labeladdr + offsetINST2;
                
                //printa no arquivo
				fprintf(tabela1,"%s %d\n", label, labeladdr); 
              }
            else fprintf(tabela1,"%s\n", auxx); 
          
          fseek(tabela_de_simbolos1, 1, SEEK_CUR);
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

int hextoint(char hex)
{
	if(isdigit(hex))
	{
		return hex-48;
	}

	else if(isalpha(hex))
	{
		return hex-87;
	}

	return 0;
}