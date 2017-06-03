//eu peguei o arquivo do TP1 e fui "aproveitando" os codigos pra exemplificar, mas tem que reescrever e arrumar TUDO. Isso é só o algoritmo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//define as funções que vamos usar:
short int juntaobjetos(objeto.o);
short int SearchForLabel(char *label,FILE *tabela_de_simbolos);

//depois, define os arquivos e variaveis que vamos precisar(tem que ser global pras funções poderem escrever neles): 
//  temporário: A tabela completa e corrigida de simbolos (labels e enderecos) que vamos montar
//  temporário: memoria de dados onde vamos colocar todos os .data sequencialmente
//  temporário: memoria de instrucao onde vamos colocar todas as instrucoes sequancialmente
	FILE *tabela_de_simbolos2 = tmpfile();
	FILE *memoria_de_instrucao = tmpfile();
	FILE *memoria_de_dados = tmpfile();
//variáveis auxiliares 
short int numlinhaINSTRUCOES = 0;
short int numlinhaTABELA = 0;
short int numlinhaDADOS = 0;

//MAIN: recebe N objetos pra juntar. Olhar como é a chamada que o prof quer. Eles serão juntados na ordem que forem passados.
int main(char** main,char** obj2,char** obj3,char** obj4,char** obj5)
{

        //argumentos: abrir os .o de entrada na ordem recebida
	FILE *main = fopen(argv[1], "r+");
	FILE *obj1 = fopen(argv[2], "r+");
	FILE *obj2 = fopen(argv[3], "r+");
        ... etc

       //depois, define a saída que vamos escrever: executavel.hex
	char nome_do_arquivo[300];
	nome_do_arquivo[0] = 0;
	int size = strlen(argv[1]);
	strncat(nome_do_arquivo, argv[1], size - 2);
	sprintf(nome_do_arquivo, "%s.hex", nome_do_arquivo);
	FILE *executavel = fopen(nome_do_arquivo, "w+");
	char aux[300];

	
	//PASSAGEM 1
        // Aqui vamos ter um loop que le todos os objetos um a um na sequência que foram passados na chamada
        int objN = 0;
	while(enquanto nao ler todos objN.o)
	{ 
	  juntaobjetos(objeto[objN])
          objN++;
	}
        //quando acabar, as variaveis 
        //  numlinhaINSTRUCOES = tamanho total da memoria de instrucoes (todos .o juntados)
        //  numlinhaDADOS  = tamanho total da memoria de dados (todos .o juntado)
 

        //Então, tem que ler na Tabela2 que a gente montou tudo que for tipo .data e adicionar mais um offset: 
        // o tamanho da memoria de instrucoes 
        loop (ler linha a linha da tabela){
         Se a label for do tipo .data
         (offset do .data) = (offset do .data) + numlinhaINSTRUCOES
	}


	fseek(memoria_de_dados, 1, SEEK_SET); //rebobina memoria_de_dados
	fseek(memoria_de_instrucao, 1, SEEK_SET); //rebobina memoria_de_instrucao
	fseek(tabela_de_simbolos2 , 1, SEEK_SET); //rebobina tabela_de_simbolos2 

       //PASSAGEM2
       //com a tabela montada e as memórias de dados e instruções juntadas, vamos arrumar as referencias igual fizemos no montador
       //le linha a linha das instrucoes. A que tiver label, procura na tabela e adiciona o offset.

	while(fscanf(memoria_de_instrucao, "%[^\n]s", aux) != EOF)
	{
          // le linha a linha. a linha que tiver uma label escrita no final, pega ela e procura na tabela. exemplo: ":00000001ff _LABEL"
	  
          int offset =0; //valor que vamos adicionar para consertar a referencia
          pnt = &aux[11]; //porque a instrucao escrita tem 11 algarismos antes de comecar a label
	  label = (char *) malloc (20 * sizeof(char));
	  
	  if((*pnt!='\n'){
	      //salva a label
	      contador =0;
	      while(*pnt != '_')
	      {
	        label[contador] = *pnt;
	        contador++;
	        pnt++;
	      }
	      label[contador] = *pnt;
	      contador++;
	      pnt++;
              offset = SearchForLabel(label, tabela_de_simbolos2); //PROCURA A LABEL NA TABELA
           }

         // daí adicionar o offset no código .hex e arruma os sumcheck que precisar
         // Além disso, apaga a label que tava escrito no .hex tbm

         //Pula pra proxima linha

       }


        //por fim, junta os arquivos da memoria de dados e instrucao em um só .hex (o executável) e retorna. FIM.

        fprintf(executavel,memoria_de_instrucao);


	//coloca a memória de dados no fim do código
		fseek(memoria_de_dados, 0, SEEK_SET);
		while(fscanf(memoria_de_dados, "%[^\n]s", aux) != EOF)
	  {
	      fprintf(executavel, "%s\n", aux);
	      endereco++;
	      fseek(memoria_de_dados, 1, SEEK_CUR);
	  }	 
	
	for(; endereco < 256; endereco++)
	{
		fprintf(executavel, ":01%04x0000", endereco);
		complemento = endereco;
		complemento = (~complemento);
		fprintf(executavel, "%02x\n", complemento & (255));
	}
	
	//end of file
	fprintf(memoria_de_instrucao, ":00000001ff");

        //fechar tudo, todos objetos
	fclose(entrada);
	fclose(memoria_de_instrucao);
	fclose(memoria_de_dados);
	fclose(tabela_de_simbolos2);
	return 0;
}

short int short int juntaobjetos(objeto.o)
{
  //Esta é a função mais importante do código. Ela monta a tabela de símbolos e organiza tudo
  //Lembrando que a estrutura do .o é assim:
  //
  // Tamanho da parte de instrucao neste arquivo
  // Tamanho da parte da tabela neste arquivo
  // Tamanho da parte de dados neste arquivo
  // Parte da Tabela 
  // Parte das Instrucoes 
  // Parte dos Dados

  //Lembrando que a estrutura da tabela do .o é assim:
  // Label    Linha   Tipo
  // _salto   4       normal
  // _valor1  7       .data
  // _salto   0       .extern


//Primeira coisa: append a parte dos dados no memoria_de_dados
//Segunda coisa: append a parte das instrucoes na memoria_de_instrucoes

//Depois, ir append a tabela lendo linha a linha num LOOP:
 LOOP 
//          Para o tipo NORMAL: offset = numlinhaINSTRUCOES
//          Para o tipo .data: offset = - (numero da linha do primeiro .data neste objeto) + numlinhaDATA
//          Para o tipo .extern: nem precisa colocar na tabela, pq ta definido em outro modulo .o
 END

//Atualiza o numero de linhas que tem os arquivos de memoria_de_dados e memoria_de_instrucoes com todos os modulos .o ja adicionados  
numlinhaINSTRUCOES = numlinhaINSTRUCOES + Linha1;
numlinhaDADOS = numlinhaDADOS + Linha3;	

  //A estrutura da tabela2 do ligador é assim:
  // Label    Offset   Tipo
  // _salto  10       normal
  // _valor1  7       .data
  // _salto3  23       normal
  // _salto4  23       normal
  // _valor2  12       .data
  // _valor3  16       .data

return 0;
}

short int SearchForLabel(char *label,FILE *tabela_de_simbolos)
{
  //Só copiei igual o do TP1, mas tem que alterar: ela vai retornar o OFFSET a ser adicionado naquela label.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
