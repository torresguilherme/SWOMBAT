#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

short int TextToBinary(char* instruction, FILE *f, char *dot_data, short int *dot_pointer);
short int GetOpcode(char* inst);
short int SearchForLabel(char *label, FILE *f);

int main(int argc, char** argv)
{
	// declara os componentes fisicos da maquina
	char memoria_principal[256];
	char stack[127];

	//aponta para onde serão escritos os dados com o .data
	short int dot_pointer = 252;

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
		instrucao = TextToBinary(aux, entrada, memoria_principal, &dot_pointer);
		fwrite(&instrucao, sizeof(short int), 1, memoria_de_instrucao);
		fseek(entrada, 1, SEEK_CUR);
	}
	fclose(entrada);

	//loop de execuçao da máquina
/*	while(!status)
	{
		//posiciona o ponteiro 
		fseek(memoria_de_instrucao, pc, SEEK_SET);
		fread(&ir, sizeof(short int), 1, memoria_de_instrucao);

		//pega o opcode da instrução
		short int opcode = (ir >> 11);
		short int constante = 0;
		short int address = 0;
		short int label = 0;

		switch(opcode)
		{
			case 0: //exit OK
				status = 1;
				break;

			case 1: //loadi
				buffer1 = (ir - (opcode << 11)) >> 8;
				address = ir - (opcode << 11) - (buffer1 << 8);
				fseek(memoria_de_instrucao, address, SEEK_SET);
				fread(&label, sizeof(short int), 1, memoria_de_instrucao);

				if((label >> 13) == -1)
				{
					address = ((label << 8) >> 8);
				}

				else
				{
					address = ((ir << 8) >> 8);
				}

				if(address == 254)
				{
				}

				else
				{

				}

				pc += 2;
				break;

			case 2: //storei
				buffer1 = (ir - (opcode << 11)) >> 8;
				address = ir - (opcode << 11) - (buffer1 << 8);
				fseek(memoria_de_instrucao, address, SEEK_SET);
				fread(&label, sizeof(short int), 1, memoria_de_instrucao);

				if((label >> 13) == -1)
				{
					address = ((label << 8) >> 8);
				}

				else
				{
					address = ((ir << 8) >> 8);
				}

				pc += 2;
				break;

			case 3: //add OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				buffer2 = (ir - (opcode << 11) - (buffer1 << 8)) >> 5;
				reg[buffer1] += reg[buffer2];
				pc += 2;
				break;

			case 4: //subtract OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				buffer2 = (ir - (opcode << 11) - (buffer1 << 8)) >> 5;
				reg[buffer1] -= reg[buffer2];
				pc += 2;
				break;

			case 5: //multiply OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				buffer2 = (ir - (opcode << 11) - (buffer1 << 8)) >> 5;
				reg[buffer1] *= reg[buffer2];
				pc += 2;
				break;

			case 6: //divide OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				buffer2 = (ir - (opcode << 11) - (buffer1 << 8)) >> 5;
				reg[buffer1] /= reg[buffer2];
				pc += 2;
				break;

			case 7: //jump OK
				pc = ir - (opcode << 11);
				break;

			case 8: //jmpz
				buffer1 = (ir - (opcode << 11)) >> 8;
				address = ir - (opcode << 11) - (buffer1 << 8);
				fseek(memoria_de_instrucao, address, SEEK_SET);
				fread(&label, sizeof(short int), 1, memoria_de_instrucao);

				if((label >> 13) == -1)
				{
					address = ((label << 8) >> 8);
				}

				else
				{
					address = ((ir << 8) >> 8);
				}

				break;

			case 9: //jmpn
				buffer1 = (ir - (opcode << 11)) >> 8;
				address = ir - (opcode << 11) - (buffer1 << 8);
				fseek(memoria_de_instrucao, address, SEEK_SET);
				fread(&label, sizeof(short int), 1, memoria_de_instrucao);

				if((label >> 13) == -1)
				{
					address = ((label << 8) >> 8);
				}

				else
				{
					address = ((ir << 8) >> 8);
				}

				break;

			case 10://move OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				buffer2 = (ir - (opcode << 11) - (buffer1 << 8)) >> 5;
				reg[buffer1] = reg[buffer2];
				pc += 2;
				break;

			case 11://load OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				buffer2 = (ir - (opcode << 11) - (buffer1 << 8)) >> 5;
				mar = 0;
				mar |= memoria_principal[reg[buffer2]];
				mar |= (memoria_principal[reg[buffer2]+1] << 8);
				reg[buffer1] = mar;
				pc += 2;
				break;

			case 12://store OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				buffer2 = (ir - (opcode << 11) - (buffer1 << 8)) >> 5;
				mdr = reg[buffer1];
				memoria_principal[reg[buffer2]] |= mdr;
				memoria_principal[reg[buffer2]+1] |= (mdr >> 8);
				pc += 2;
				break;

			case 13://loadc OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				constante = 0;
				constante |= ir - (opcode << 11) - (buffer1 << 8);
				reg[buffer1] = constante;
				pc += 2;
				break;

			case 14://clear OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				reg[buffer1] = 0;
				pc += 2;
				break;

			case 15://negate OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				buffer2 = (ir - (opcode << 11) - (buffer1 << 8)) >> 5;
				sdr = reg[buffer2];
				sdr--;
				sdr = (~sdr);
				reg[buffer1] = sdr;
				pc += 2;
				break;

			case 16://push OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				sdr = reg[buffer1];
				stack[stack_pointer] |= sdr;
				stack[stack_pointer+1] |= (sdr >> 8);
				stack_pointer += 2;
				pc += 2;
				break;

			case 17://pop OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				reg[buffer1] |= stack[stack_pointer-2];
				reg[buffer1] |= (stack[stack_pointer-1] << 8);
				stack_pointer -= 2;
				pc += 2;
				break;

			case 18://addi OK
				buffer1 = (ir - (opcode << 11)) >> 8;
				constante = 0;
				constante |= ir - (opcode << 11) - (buffer1 << 8);
				reg[buffer1] += constante;
				pc += 2;
				break;

			case 19://call OK
				sdr = pc;
				stack[stack_pointer] |= sdr;
				stack[stack_pointer+1] |= (sdr >> 8);
				stack_pointer += 2;
				pc = ir - (opcode << 11);
				break;

			case 20://return OK
				pc = 0;
				pc |= stack[stack_pointer-2];
				pc |= (stack[stack_pointer-1] << 8);
				stack_pointer -= 2;
				pc += 2;
				break;

			default:
				break;
		}

	}
*/
	fclose(memoria_de_instrucao);
	return 0;
}

short int TextToBinary(char* instruction, FILE* f, char *dot_data, short int *dot_pointer)
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
			aux--;
			aux = (~aux);
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

	else if(opcode == -1) // .data
	{
		int bits = 0;
		int bitCounter;
		int constante = 0;
		int endereco = *dot_pointer;

		//pega o número de bits que serão alocados
		while(!isdigit(*p))
		{
			p++;
		}

		while(isdigit(*p))
		{
			bits *= 10;
			bits += ((*p)-48);
			p++;
		}

		//procura uma constante
		while(!isdigit(*p) && (*p) != '-')
		{
			p++;
		}

		//trata a constante caso ela seja um número positivo
		if(isdigit(*p))
		{
			while(isdigit(*p))
			{
				constante *= 10;
				constante += ((*p)-48);
				p++;
			}

			for(bitCounter = 0; bitCounter < bits; bitCounter++)
			{
				dot_data[*dot_pointer] = (constante >> (8 * bitCounter));
				(*dot_pointer)++;
			}
		}

		//trata o número negativo caso contrário
		else
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

			for(bitCounter = 0; bitCounter < bits; bitCounter++)
			{
				dot_data[*dot_pointer] = (constante >> (8 * bitCounter));
				(*dot_pointer)++;
			}
		}

		if((*dot_pointer) % 2 == 1)
		{
			(*dot_pointer)++;
		}

		ret = (opcode << 13) | (bits << 8) | endereco;
		return ret;
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
