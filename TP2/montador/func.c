#include "func.h"

short int TextToBinary(char* instruction, FILE* f, FILE *tabela_de_simbolos)
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
	char label[100];

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
		  constante = SearchForLabel(label, tabela_de_simbolos);
		}
		
		aux |= constante;
	}
	//operacoes de pilha + clear
	else if(opcode == 16 //push
	|| opcode == 17	//pop
	|| opcode == 14) //clear
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
		  aux = SearchForLabel(label, tabela_de_simbolos);
		}
	}

	else if(opcode == 0 //exit
	|| opcode == 20) //return
	{
		aux = 0;
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

char *TemLabel(char *inst)
{
	int counter = 0;
	char *ret = malloc(sizeof(char) * 100);
	char *p = &inst[0];
	while(*p != '_')
	{
		if((*p) == 0)
		{
			free(ret);
			return NULL;
		}

		p++;
	}

	while((*p) == '_'  || isalpha(*p) || isdigit(*p))
	{
		ret[counter] = *p;
		counter++;
		p++;
	}
	ret[counter] = ':';
	ret[counter+1] = 0;

	return ret;
}
