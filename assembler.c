#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

short int TextToBinary(char* instruction, FILE *f);
short int GetOpcode(char* inst);
short int SearchForLabel(char *label, FILE *f);

int main(int argc, char** argv)
{
	// declara os componentes fisicos da maquina
	char memoria_principal[256];
	char stack[127];

	//registradores de proposito geral
	short int reg[8];

	//aponta para a posicao superior da pilha
	short int stack_pointer = 0;

	//guarda o valor a ser empilhado
	short int sdr;

	//program counter
	short int pc = 0;

	//operandos das operacoes logico-aritmeticas
	short int buffer1, buffer2;

	//instruction register
	short int ir;

	//valor a ser escrito no load
	short int mar;

	//valor a ser escrito no store
	short int mdr;

	//halt bit: cancela a execucao do programa
	short int status = 0;

	FILE *entrada = fopen(argv[1], "r+");
	FILE *memoria_de_instrucao = fopen("mem.txt", "w+b");
	char aux[100];

	short int instrucao;

	while(fscanf(entrada, "%[^\n]s", aux) != EOF)
	{
		instrucao = TextToBinary(aux, entrada);
		fwrite(&instrucao, 1, sizeof(short int), memoria_de_instrucao);
		fseek(entrada, 1, SEEK_CUR);
	}
	fclose(entrada);

	//loop de execuçao da máquina
//	while(!status)
//	{
		//colocar aqui a execução das instruções da memória
//	}

	fclose(memoria_de_instrucao);
	return 0;
}

short int TextToBinary(char* instruction, FILE* f)
{
	// essa funcao converte o assembly pra linguagem de maquina
	// IMPLEMENTAR O .DATA
	
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

	//operacoes reg-mem ou com constante
	else if(opcode == 1 //loadi
	|| opcode == 2 //storei
	|| opcode == 8 //jmpz
	|| opcode == 9 //jmpn
	|| opcode == 13 //loadc
	|| opcode == 18) //addi
	{
		// pega o endereco do reg

		while(!isdigit(*p))
		{
			p++;
		}

		aux += ((*p)-48) << 8;
		p++;

		//pode receber tanto labels quanto endereços de memória
		while(!isdigit(*p) && (*p) != '_' && (*p) != '-')
		{
			p++;
		}

		//nesse caso, ele poe o numero do endereco ou a constante
		//numero positivo
		if(isdigit(*p))
		{
			while(isdigit(*p))
			{
				aux *= 10;
				aux += ((*p)-48);
				p++;
			}

		}

		//numero negativo -> faz o complemento de dois
		else if((*p) == '-')
		{
			p++;
			while(isdigit(*p))
			{
				aux *= 10;
				aux += ((*p)-48);
				p++;
			}
			aux += 1 << 7;
			aux--;
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

	else if(opcode == -1) // .data
	{
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
	//salva a antiga posicao no arquivo pra poder voltar depois

	//vai ate o inicio
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
