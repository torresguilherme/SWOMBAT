//FORMATO DA SAIDA DO ARQUIVO OBJETO:
//x -> TAMANHO DA TABELA DE LABELS
//y -> TAMANHO DAS INSTRUÇÕES EM LINHAS HEX
//z -> TAMANHO DOS DADOS (.DATA) EM LINHAS HEX
//
//- TABELA HEX
//(CADA LABEL (COM _ ATRÁS E O : NA FRENTE) ESTÁ SEGUIDA DE SEU ENDEREÇO) EM DECIMAL
// EXTERNS SÃO SEGUIDOS PELA LETRA 'e'
//- HEX CONTENDO INSTRUÇÕES E DADOS ALOCADOS COM .DATA
//(TODOS OS ENDEREÇOS DE HEX ESTAO ZERADOS)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "func.h"

int main(int argc, char** argv)
{
	FILE *entrada = fopen(argv[1], "r+");
	FILE *objeto = fopen(argv[2], "w+");
	FILE *memoria_principal = tmpfile();
	FILE *memoria_de_dados = tmpfile();
	FILE *tabela_de_simbolos = tmpfile();
	char aux[300];

	// variaveis para auxiliar na leitura dos arquivos
	short int instrucao;
	int complemento;
	short int endereco = 0;
	short int numlinha = 0;
	short int numBytes = 0;
	short int contador = 0;
	int data;
	char *pnt = &aux[0];
	long long int temp =0;
	short int hexlinha =0;
	int dot_externs = 0;

	//contadores para o numero de linhas que o arquivo terá.
	int contador_de_labels = 0;
	int contador_de_instrucoes = 0;
	int contador_de_dados = 0;

	char label[100];
	//PASSAGEM ZERO
	fscanf(entrada, "%[^\n]s", aux);
	while(aux[0] == '.')
	{
		dot_externs++;
		pnt = &aux[0];

		while(*pnt != '_')
		{
			pnt++;
		}

		contador = 0;
		while((*pnt == '_') || isalpha(*pnt) || isdigit(*pnt))
		{
			label[contador] = *pnt;
			contador++;
			pnt++;
		}
		label[contador] = ':';
		contador++;
		label[contador] = 0;

		fprintf(tabela_de_simbolos, "%s 0\n", label); 
		fprintf(memoria_principal, "%s e\n", label); 
		fseek(entrada, 1, SEEK_CUR);
		fscanf(entrada, "%[^\n]s", aux);
		contador_de_labels++;
	}

	//PASSAGEM 1
	while(fscanf(entrada, "%[^\n]s", aux) != EOF)
	{ 
		//montar tabela de simbolos e definir tamanho do codigo
		// Le a linha.
		// é codigo? não: ignora     
		//           numlinha++, proxima iterac
		// é código? sim: tem label?
		// tem label? nao: numlinha++, proxima iteracao 
		// tem label? sim: salva a string da label. é .data?
		// é .data? não: escreve no file ela e o endereço(numlinha) na tabela LABEL ADDR ex.: _LOOP: 23
		//                                                        numlinha++, proxima iteracao
		// é .data? sim: escreve no file ela e o endereço(numlinha) na tabela
		//                                                        incrementa o numlinha pro tanto de bytes que estamos alocando
		//                                                        escreve no arquivo memoria_de_dados a data sequencialmente    
		//                                                        proxima iteracao    
		pnt = &aux[0];

		if((*pnt!='\n')||(*pnt!=';'))
		{
			if(*pnt == '_') //verifica se é uma label
			{
				//salva a label
				contador =0;
				while(*pnt != ':')
				{
					label[contador] = *pnt;
					contador++;
					pnt++;
				}

				label[contador] = *pnt;
				contador++;
				label[contador] = 0;
				pnt++;

				fprintf(tabela_de_simbolos, "%s %d\n", label, numlinha); 
				fprintf(memoria_principal, "%s %d\n", label, numlinha); 
				contador_de_labels++;

				//procura a proxima letra ou ponto (.data)
				while(!isalpha(*pnt) && (*pnt) != '.')
				{
					pnt++;
				}

				if (*pnt == '.') //verifica se é .data
				{

					//procura proximo número (numBytes a ser alocado)
					while(!isdigit(*pnt))
					{
						pnt++;
					}

					temp =0;
					while(isdigit(*pnt))
					{
						temp *= 10;
						temp += ((*pnt)-48);
						pnt++;
					}
					numBytes =temp;

					//procura proximo número (data a ser alocada)
					temp =0;
					while(!isdigit(*pnt) && (*pnt) != '-')
					{
						pnt++;
					}
					//numero positivo
					if(isdigit(*pnt))
					{
						while(isdigit(*pnt))
						{
							temp *= 10;
							temp += ((*pnt)-48);
							pnt++;
						}
						data = temp;
					}

					//numero negativo -> faz o complemento de dois
					else if((*pnt) == '-')
					{
						pnt++;
						while(isdigit(*pnt))
						{
							temp *= 10;
							temp += ((*pnt)-48);
							pnt++;
						}
						temp--;
						temp = (~temp);
						data = temp;
					}

					// Colocar em HEX bonitinho e printar no memoria_de_dados
					int iz = 0;
					complemento = 0;
					for (iz = numBytes-1; iz>=0; iz--)
					{
						fprintf(memoria_de_dados,":01%04x00", numlinha);
						hexlinha = (data&(255<<(iz*8)))>>(iz*8);
						fprintf(memoria_de_dados, "%02x", hexlinha & (255));
						//complemento de dois da soma dos hexes
						complemento = hexlinha + numlinha;
						complemento = (~complemento);
						fprintf(memoria_de_dados, "%02x\n", complemento & (255));
						numlinha++;
					}


				}
				else {numlinha = numlinha+2;}
			}
			else {numlinha = numlinha+2;}
		}

		int ia;
		for(ia=0; ia < 300; ia++)
		{
			aux[ia] = 0;
		}
		fseek(entrada, 1, SEEK_CUR);
	}

	fseek(memoria_de_dados, 0, SEEK_SET); //rebobina memoria_de_dados
	fseek(entrada, 0, SEEK_SET);//rebobina entrada

	//PASSAGEM 2
	while(fscanf(entrada, "%[^\n]s", aux) != EOF)
	{
		instrucao = TextToBinary(aux, entrada, tabela_de_simbolos);
		if(instrucao != -1)
		{
			//start code, tamanho dos dados em hex, offset do endereco e tipo do dado
			fprintf(memoria_principal, ":01");

			//offset do endereco
			fprintf(memoria_principal, "%04x", endereco++);

			//tipo do dado
			fprintf(memoria_principal, "00");

			//dado da instrucao - primeira parte
			fprintf(memoria_principal, "%02x", (instrucao >> 8) & (255));

			//complemento de dois da soma dos hexes
			complemento = (instrucao >> 8);
			complemento += endereco;
			complemento--;
			complemento = (~complemento);
			fprintf(memoria_principal, "%02x\n", complemento & (255));

			//segunda linha: mesmo inicio
			fprintf(memoria_principal, ":01");

			//offset do endereco
			fprintf(memoria_principal, "%04x", endereco++);

			//tipo do dado
			fprintf(memoria_principal, "00");

			//dado da instrucao - segunda parte
			fprintf(memoria_principal, "%02x", instrucao & (255));

			//complemento de dois da soma dos hexes
			complemento = (instrucao & (255));
			complemento += endereco;
			complemento--;
			complemento = (~complemento);
			fprintf(memoria_principal, "%02x", complemento & (255));

			//ve se tem label e bota na frente
			int current_opcode = (instrucao >> 11) & 31; // 00011111
			if(current_opcode == 1 //loadi
			|| current_opcode == 2 //storei
			|| current_opcode == 7 //jump
			|| current_opcode == 8 //jmpz
			|| current_opcode == 9 //jmpn
			|| current_opcode == 19) //call
			{
				char *extra_label = TemLabel(aux);
				if(extra_label)
				{
					fprintf(memoria_principal, " %s", extra_label);
					free(extra_label);
				}
			}

			fprintf(memoria_principal, "\n");
			contador_de_instrucoes += 2;
		}

		fseek(entrada, 1, SEEK_CUR);
	}

	//coloca a memória de dados no fim do código
	fseek(memoria_de_dados, 0, SEEK_SET);
	while(fscanf(memoria_de_dados, "%[^\n]s", aux) != EOF)
	{
		fprintf(memoria_principal, "%s\n", aux);
		endereco++;
		fseek(memoria_de_dados, 1, SEEK_CUR);
		contador_de_dados ++;
	}	 
/* (isso é inútil aqui, mas deixei de referência para o ligador) - Guilherme
 * o objetivo desse código é escrever endereços hex zerados até o fim do arquivo e depois botar o EOF.
	for(; endereco < 256; endereco++)
	{
		fprintf(memoria_principal, ":01%04x0000", endereco);
		complemento = endereco;
		complemento = (~complemento);
		fprintf(memoria_principal, "%02x\n", complemento & (255));
	}

	//end of file
	fprintf(memoria_principal, ":00000001ff");
*/

	//escreve no arquivo objeto todos os dados necessários
	//rebobina os arquivos
	fseek(memoria_principal, 0, SEEK_SET);
	fseek(tabela_de_simbolos, 0, SEEK_SET);

	//imprime os valores das linhas
	fprintf(objeto, "%d\n", contador_de_labels);
	fprintf(objeto, "%d\n", contador_de_instrucoes);
	fprintf(objeto, "%d\n", contador_de_dados);

	// escreve os dados da memoria principal e da tabela
	while(fscanf(memoria_principal, "%[^\n]s", aux) != EOF)
	{
		fprintf(objeto, "%s\n", aux);
		fseek(memoria_principal, 1, SEEK_CUR);
	}

	fclose(entrada);
	fclose(memoria_principal);
	fclose(memoria_de_dados);
	fclose(tabela_de_simbolos);
	fclose(objeto);
	return 0;
}
