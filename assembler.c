#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

short int TextToBinary(char* instruction, FILE *f, short int *pointer);
short int GetOpcode(char* inst);
short int SearchForLabel(char *label, FILE *f);

int main(int argc, char** argv)
{
	FILE *entrada = fopen(argv[1], "r+");
	FILE *memoria_de_instrucao = fopen("mem.hex", "w+");
	char aux[100];

	//o ponteiro que tem o endereco das constantes comeca na ultima posicao
	//(excluíndo a entrada e saída)
	short int pointer = 252;
	short int instrucao;
	short int complemento;
	short int endereco = 0;

	while(fscanf(entrada, "%[^\n]s", aux) != EOF)
	{
		instrucao = TextToBinary(aux, entrada, &pointer);
		if(instrucao != -1)
		{
			//printf("%i %i %i\n", instrucao, (instrucao >> 8), instrucao & (255));
			//start code, tamanho dos dados em hex, offset do endereco e tipo do dado
			fprintf(memoria_de_instrucao, ":01");
	
			//offset do endereco
			fprintf(memoria_de_instrucao, "%04x", endereco++);

			//tipo do dado
			fprintf(memoria_de_instrucao, "00");

			//dado da instrucao - primeira parte
			fprintf(memoria_de_instrucao, "%02x", (instrucao >> 8));

			//complemento de dois da soma dos hexes
			complemento = (instrucao >> 8);
			complemento += endereco;
			complemento--;
			complemento = (~complemento);
			fprintf(memoria_de_instrucao, "%02x\n", complemento & (255));
	
			//segunda linha: mesmo inicio
			fprintf(memoria_de_instrucao, ":01");

			//offset do endereco
			fprintf(memoria_de_instrucao, "%04x", endereco++);

			//tipo do dado
			fprintf(memoria_de_instrucao, "00");


			//dado da instrucao - segunda parte
			fprintf(memoria_de_instrucao, "%02x", instrucao & (255));

			//complemento de dois da soma dos hexes
			complemento = (instrucao & (255));
			complemento += endereco;
			complemento--;
			complemento = (~complemento);
			fprintf(memoria_de_instrucao, "%02x\n", complemento & (255));
		}
		fseek(entrada, 1, SEEK_CUR);
	}
	//end of file
	fprintf(memoria_de_instrucao, ":00000001ff");

	fclose(entrada);
	fclose(memoria_de_instrucao);
	return 0;
}

short int TextToBinary(char* instruction, FILE* f, short int *pointer)
{
	// essa funcao converte o assembly pra linguagem de maquina
	short int ret = 0;
	char operation[20];
	char *p = &instruction[0];

	//salva a posicao no arquivo por precaucao
	int position_save = ftell(f);

	if(*p == '_') //verifica se é uma label
	{
		//no caso, se for, ignora a existencia dela por enquanto
		while(*p != ':')
		{
			p++;
		}
	}

	//procura a proxima letra ou ponto (.data)
	while(!isalpha(*p) && (*p) != '.')
	{
		p++;
	}

	//pega o tipo da operação
	int i = 0;
	while(isalpha(*p) || (*p) == '.')
	{
		operation[i] = (*p);
		i++;
		p++;
	}
	operation[i] = 0;
	i = 0;

	//soma o opcode na memoria de instruçao
	short int opcode = GetOpcode(operation);
	short int opcodeBits = opcode << 11;

	//variaveis auxiliares para guardar enderecos e labels, respectivamente
	short int aux = 0;
	short int constante = 0;
	char label[30];

	//operacoes reg-reg
	if(opcode == 3 //add
	|| opcode == 4 //subtract
	|| opcode == 5 //multiply
	|| opcode == 6 //divide
	|| opcode == 10 //move
	|| opcode == 11 //load
	|| opcode == 12 //store
	|| opcode == 15) //negate
	{
		// pega os números dos dois registradores
		while(!isdigit(*p))
		{
			p++;
		}

		aux += ((*p)-48) << 8;
		p++;	

		while(!isdigit(*p))
		{
			p++;
		}

		aux += ((*p)-48) << 5;
		p++;
	}

	//operacoes nos registradores com constante
	else if(opcode == 13 //loadc
	|| opcode == 18) //addi
	{
		// pega o endereco do reg
		while(!isdigit(*p))
		{
			p++;
		}

		aux += ((*p)-48) << 8;
		p++;

		//procura a constante
		while(!isdigit(*p) && (*p) != '-')
		{
			p++;
		}

		//numero positivo
		if(isdigit(*p))
		{
			while(isdigit(*p))
			{
				constante *= 10;
				constante += ((*p)-48);
				p++;
			}
			aux |= constante;

		}

		//numero negativo -> faz o complemento de dois
		else if((*p) == '-')
		{
			p++;
			while(isdigit(*p))
			{
				constante *= 10;
				constante += ((*p)-48);
				p++;
			}
			constante--;
			constante = (~constante);
			constante ^= (255 << 8);
			aux |= constante;
		}
	}

	//operacoes reg-mem com endereçamento direto
	else if(opcode == 1 //loadi
	|| opcode == 2 //storei
	|| opcode == 8 //jmpz
	|| opcode == 9) //jmpn
	{
		// pega o endereco do reg
		while(!isdigit(*p))
		{
			p++;
		}

		aux += ((*p)-48) << 8;
		p++;

		//pode receber tanto labels quanto endereços de memória
		while(!isdigit(*p) && (*p) != '_' && (*p) != 'I')
		{
			p++;
		}

		//endereço direto
		if(isdigit(*p))
		{
			while(isdigit(*p))
			{
				aux *= 10;
				aux += ((*p)-48);
				p++;
			}

		}

		//posição da memória para I/O (posição número 254)
		else if((*p) == 'I')
		{
			aux += 254;
		}

		//aqui, ele busca a label
		else
		{
			while((*p) == '_' || isalpha(*p) || isdigit(*p))
			{
				label[i] = (*p);
				i++;
				p++;
			}

			label[i] = ':';
			label[i+1] = 0;
			aux = SearchForLabel(label, f);
		}
	}

	//operacoes de pilha
	else if(opcode == 16 //push
	|| opcode == 17) //pop
	{
		// pega endereco do reg
		while(!isdigit(*p))
		{
			p++;
		}

		aux += ((*p)-48) << 8;
		p++;
	}

	//jumps incondicionais
	else if(opcode == 7 //jump
	|| opcode == 19) //call
	{
		//procura um numero de endereco ou uma label
		while(!isdigit(*p) && (*p) != '_')
		{
			p++;
		}

		// endereco
		if(isdigit(*p))
		{
			while(isdigit(*p))
			{
				aux *= 10;
				aux += ((*p)-48);
				p++;
			}
		}

		//label
		else
		{
			while((*p) == '_' || isalpha(*p) || isdigit(*p))
			{
				label[i] = (*p);
				i++;
				p++;
			}

			label[i] = ':';
			label[i+1] = 0;
			aux = SearchForLabel(label, f);
		}
	}

	else
	{
		fseek(f, position_save, SEEK_SET);
		return -1;
	}

	// faz uma operação de "or" nos bits do opcode e informaçoes no aux
	// para gerar o valor de retorno
	ret = opcodeBits | aux;

	//volta à posição original no arquivo
	fseek(f, position_save, SEEK_SET);
	return ret;
}

short int GetOpcode(char* inst)
{
	//casos para os nomes de instrução
	//o valor é retornado de acordo com o opcode dela
	if(!strcmp(inst, "exit"))
	{
		return 0;
	}
	
	else if(!strcmp(inst, "loadi"))
	{
		return 1;
	}

	else if(!strcmp(inst, "storei"))
	{
		return 2;
	}

	else if(!strcmp(inst, "add"))
	{
		return 3;
	}

	else if(!strcmp(inst, "subtract"))
	{
		return 4;
	}

	else if(!strcmp(inst, "multiply"))
	{
		return 5;
	}

	else if(!strcmp(inst, "divide"))
	{
		return 6;
	}

	else if(!strcmp(inst, "jump"))
	{
		return 7;
	}

	else if(!strcmp(inst, "jmpz"))
	{
		return 8;
	}

	else if(!strcmp(inst, "jmpn"))
	{
		return 9;
	}

	else if(!strcmp(inst, "move"))
	{
		return 10;
	}

	else if(!strcmp(inst, "load"))
	{
		return 11;
	}

	else if(!strcmp(inst, "store"))
	{
		return 12;
	}

	else if(!strcmp(inst, "loadc"))
	{
		return 13;
	}

	else if(!strcmp(inst, "clear"))
	{
		return 14;
	}

	else if(!strcmp(inst, "negate"))
	{
		return 15;
	}

	else if(!strcmp(inst, "push"))
	{
		return 16;
	}

	else if(!strcmp(inst, "pop"))
	{
		return 17;
	}

	else if(!strcmp(inst, "addi"))
	{
		return 18;
	}

	else if(!strcmp(inst, "call"))
	{
		return 19;
	}

	else if(!strcmp(inst, "return"))
	{
		return 20;
	}

	return -1;
}

short int SearchForLabel(char *label, FILE *f)
{
	//busca linearmente por uma label no arquivo de entrada comecando do inicio
	//gera o endereco na memoria de instruçoes e guarda na chamada da operação

	fseek(f, 0, SEEK_SET);

	char current[30]; //primeira palavra na linha
	char trash[100]; //guarda o resto da linha
	int found = 0; //sinaliza se a label for encontrada ou nao
	int lines = 0; //o numero da linha no codigo onde a label for encontrada

	while(!found)
	{
		fscanf(f, "%s", current);
		if(!strcmp(label, current)) //compara com a label que se quer encontrar
		{
			found = 1;
		}

		else //nesse caso, ainda nao encontrou
		{
			fscanf(f, "%[^\n]s", trash);
			lines++;
		}
	}

	//assumindo que o endereçamento das instruçoes ocorre so em numeros pares
	//o que eu acho que está certo pela especificação
	return lines * 2;
}
