#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef int (*funcp) (int,int);

typedef union {
int a;
unsigned char b[4];
} U;

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

/*************************************************************************************************************************************

		INÍCIO DA FUNÇÃO COMPILA

*************************************************************************************************************************************/

funcp compila (FILE *f) {
  int line = 1;
  int  c,i=0,d;
  unsigned char *code;
  int id0, id1, id2,num;
  char var0, var1, var2, op, c0;
  U u;
  /* Variáveis para efetuar a instrução de desvio  */
  int lineindex[50],k, lineif,index = 0;	/* Vetor lineindex grava o índice do vetor code para o início de cada linha do arquivo lida  */

  code = (unsigned char*) malloc(1000 *sizeof(unsigned char));	/*	Alocando espaço para o código da função 	*/

  for(k=0;k<50;k++) lineindex[k]=0;		/*  Zerando vetor de índices para cada início de linha  */


/*****************************************************************************************************************************/
  /* Gravando instruções do prólogo/registro de ativação */
  code[0] = 0x55;code[1] = 0x48;code[2] = 0x89;code[3] = 0xe5;code[4] = 0x48;code[5] = 0x83;code[6] = 0xec;code[7] = 0x10;
  i = 7;

 /* Zerando variáveis locais */
code[i++] = 0x83;code[i++] = 0x48;code[i++] = 0x83;code[i++] = 0xec;code[i++] = 0x10;code[i++] = 0x48;
code[i++] = 0xc7;code[i++] = 0x44;code[i++] = 0x24; code[i++] = 0xf0;code[i++] = 0x00;code[i++] = 0x00;code[i++] = 0x00;code[i++] = 0x00; /* Zera v1  */
code[i++] = 0xc7;code[i++] = 0x44;code[i++] = 0x24; code[i++] = 0xf4;code[i++] = 0x00;code[i++] = 0x00;code[i++] = 0x00;code[i++] = 0x00; /* Zera v2  */
code[i++] = 0xc7;code[i++] = 0x44;code[i++] = 0x24; code[i++] = 0xf8;code[i++] = 0x00;code[i++] = 0x00;code[i++] = 0x00;code[i++] = 0x00; /* Zera v3  */
code[i++] = 0xc7;code[i++] = 0x44;code[i++] = 0x24; code[i++] = 0xfc;code[i++] = 0x00;code[i++] = 0x00;code[i++] = 0x00;code[i++] = 0x00; /* Zera v4  */

/******************************************************************************************************************************/

/*	PERCORRENDO ARQUIVO COM WHILE */

  while ((c = fgetc(f)) != EOF) {

  	lineindex[line] = i;

  /*	FAZENDO A LEITURA DA LINHA E GRAVANDO VALORES LIDOS NAS VARIÁVEIS	*/	
    switch (c) {
      case 'r': { /* retorno */
        if (fscanf(f, "et%c", &c0) != 1)
          error("comando invalido", line);
        printf("ret\n");
        break;
      }
      case 'v': 
      case 'p': {  /* atribuicao */
	var0 = c;
        if (fscanf(f, "%d = %c%d %c %c%d", &id0, &var1, &id1,
                   &op, &var2, &id2) != 6)
            error("comando invalido", line);
          printf("%c%d = %c%d %c %c%d\n",
                var0, id0, var1, id1, op, var2, id2);

        break;
      }
      case 'i': { /* desvio */
        if (fscanf(f, "f %c%d %d", &var0, &id0, &num) != 3)
            error("comando invalido", line);
          printf("if %c%d %d\n", var0, id0, num);
        break;
      }
      default: error("comando desconhecido", line);
    }

/************************************************************************************************************************************************************/

/*************************************************************************************************************************************************************/
   
/*	TRADUZINDO E GRAVANDO CÓDIGO DE MÁQUINA		*/

if (c == 'v' || c == 'p')
{	
/*********************************	CASO OPERAÇÃO	*******************************************************************************************************************************/

	/************	PRIMEIRA ETAPA - DEFININDO OPERANDO 1 (var1) E MOVENDO PARA %rbx	************************/

	switch (var1)
	{
		case '$':	/* Caso onde operando 1 é constante */
		{	code[i++] = 0xbb;
			u.a = id1;
			code[i++] = u.b[0];code[i++] = u.b[1];code[i++] = u.b[2];code[i++] = u.b[3];
			break;
		}

		case 'v':	/* Caso onde operando 1 é variável local */
		{
			code[i++] = 0x8b;code[i++] = 0x5c;code[i++] = 0x24;
			if (id1 == 1) code[i++] = 0xf0;
			else if (id1 == 2) code[i++] = 0xf4;
			else if (id1 == 3) code[i++] = 0xf8;
			else if (id1 == 4) code[i++] = 0xfc;
			break;
		}

		case 'p':	/* Caso onde operando 1 é parâmetro  */
		{
			code[i++] = 0x89;
			if (id1 == 1) code[i++] = 0xfb;
			else if (id1 == 2) code[i++] = 0xf3;
			break;
		}

	}

	/*********	SEGUNDA ETAPA - DEFININDO OPERANDO 2 (var2) E MOVENDO PARA %r12 	*************************/

	switch (var2)
	{
		case '$':	/* Caso onde operando 2 é constante */
		{
			code[i++] = 0x41; code[i++] = 0xbc;
			u.a = id2;
			code[i++] = u.b[0];code[i++] = u.b[1];code[i++] = u.b[2];code[i++] = u.b[3];
			break;
		}

		case 'v':	/* Caso onde operando 2 é variável local */
		{
			code[i++] = 0x44; code[i++] = 0x8b;code[i++] = 0x64;code[i++] = 0x24;
			if (id2 == 1) code[i++] = 0xf0;
			else if (id2 == 2) code[i++] = 0xf4;
			else if (id2 == 3) code[i++] = 0xf8;
			else if (id2 == 4) code[i++] = 0xfc;
			break;
		}

		case 'p':	/* Caso onde operando 2 é parâmetro  */
		{
			code[i++] = 0x41; code[i++] = 0x89;
			if (id2 == 1) code[i++] = 0xfc;
			else if (id2 == 2) code[i++] = 0xf4;
			break;
		}

	}

	/*********	TERCEIRA ETAPA - DEFININDO OPERADOR (op) E EFETUANDO OPERAÇÃO ENTRE %rbx e %r12	  *************************/
	
	switch (op)
	{
		case '+':	/* Caso onde operador é soma */
		{
			code[i++] = 0x44; code[i++] = 0x01; code[i++] = 0xe3;
			break;
		}

		case '-':	/* Caso onde operador é subtração */
		{
			code[i++] = 0x44; code[i++] = 0x29; code[i++] = 0xe3;
			break;
		}

		case '*':	/* Caso onde operador é multiplicação  */
		{
			code[i++] = 0x41; code[i++] = 0x0f; code[i++] = 0xaf;code[i++] = 0xdc;
			break;
		}

	}

	/*********	QUARTA ETAPA - DEFININDO VARIÁVEL QUE RECEBE O VALOR (var0) E MOVENDO VALOR DE %rbx PARA ELA *************************/

	switch (var0)
	{
		case 'v':	/* Caso onde var0 é variável local */
		{
			code[i++] = 0x89;code[i++] = 0x5c;code[i++] = 0x24;
			if (id0 == 1) code[i++] = 0xf0;
			else if (id0 == 2) code[i++] = 0xf4;
			else if (id0 == 3) code[i++] = 0xf8;
			else if (id0 == 4) code[i++] = 0xfc;
			break;
		}

		case 'p':	/* Caso onde var0 é parâmetro  */
		{
			code[i++] = 0x89;
			if (id0 == 1) code[i++] = 0xdf;
			else if (id0 == 2) code[i++] = 0xde;
			break;
		}

	}



}
/***************************** ^^^ FIM DO CASO OPERAÇÃO ^^^ *********************************************************************************************************************************/


/*************************** CASO  RETORNO  ********************************************************************************************************************************************/
else if (c == 'r')
{
/* Finalização --> Prepara retorno e libera Registro de ativação */
 code[i++] = 0x8b; code[i++] = 0x44; code[i++] = 0x24; code[i++] = 0xf0; /* move valor de v1 (retorno) para %eax */
 code[i++] = 0xc9; code[i++] = 0xc3;	/* libera registro de ativação e retorna */
}
/***********************  ^^^ FIM DO CASO RETORNO ^^^ *****************************************************************************************************************************************/


/**********************	  CASO CONDICIONAL	***************************************************************************************************************************************/
else if (c=='i')
{
	switch(var0)		/* Move variável a ser testada para %rbx  */
	{
		case 'v': /* Caso onde var0 é variável local */
		{	code[i++] = 0x8b;code[i++] = 0x5c;code[i++] = 0x24;
			if (id0 == 1) code[i++] = 0xf0;
			else if (id0 == 2) code[i++] = 0xf4;
			else if (id0 == 3) code[i++] = 0xf8;
			else if (id0 == 4) code[i++] = 0xfc;
			break;
		}
		
		case 'p':  /* Caso onde var0 é parâmetro  */
		{
			code[i++] = 0x41; code[i++] = 0x89;
			if (id2 == 1) code[i++] = 0xfc;
			else if (id2 == 2) code[i++] = 0xf4;
			break;
		}		

	}

	code[i++] = 0x83;code[i++] = 0xfb;code[i++] = 0x00;   /* Compara %ebx com 0  */

	code[i++] = 0x75;
	code[i++] = num;	/* Instrução de desvio. Colocando endereço neutro e guardando índice do vetor que será alterado posteriormente */



}
/********************* ^^^ FIM DO CASO CONDICIONAL ^^^  ****************************************************************************************************************************************/

/****************************************************************************************************************************************************************************************/

    line ++;
    fscanf(f, " ");
}

/******************************************************************************************************************************

	FIM DO PRIMEIRO WHILE (LEITURA E TRADUÇÃO)

**************************************************************************************************************************/

/****************	CORREÇÃO DAS INSTRUÇÕES DE DESVIO 	******************************************************************/


for(k=0;k<1000;k++)		/* Percorre vetor code até achar instrução de desvio  */
{
	if (code[k]==0x75)
	{
		lineif = code[k+1];		/* line2 é a linha para onde o controle será desviado, cujo úmero está armazenado na instrução seguinte do vetor code */
		line = 1;				/* inicializa o controle de linha para percorrer o arquivo novamente  */

		rewind(f);
		while ((d = fgetc(f)) != EOF)		/*	Percorre o arquivo, lendo linha por linha */
		{
			switch (d) 
			{
     			case 'r': 
     			{ /* retorno */
        			fscanf(f, "et%c", &c0);
        			break;
     			}
      			case 'v': 
      			case 'p': 
      			{  /* atribuicao */
					var0 = c;
       			 	fscanf(f, "%d = %c%d %c %c%d", &id0, &var1, &id1,
                   &op, &var2, &id2);
       				 break;
     			}
     			case 'i':
     			{ /* desvio */
       				fscanf(f, "f %c%d %d", &var0, &id0, &num);
        					break;
    			}
  			}
			if (line == lineif)		/* Se a linha lida é igual à linha procurada (para onde o controle deve ser desviado), sai do while  */
				break;

			line ++;
    		fscanf(f, " ");

		}

		index = lineindex[line];	/*	grava o índice de início da linha, armazenado no vetor lineindex  */

		code[k+1] = (int) ((void*)&code[index] - (void*)&code[k+2]);  /* Substitui a instrução de desvio pelo deslocamente entre a linha à ser desviada e a posição seguinte do vetor  */

	}

}

/*********************		FIM DA CORREÇÃO DAS INSTRUÇÕES DE DESVIO 	**********************************************************************************************/
	
  return (funcp)code;
}


/*************************************************************************************************************************************************************************

		FIM DA FUNÇÃO COMPILA

************************************************************************************************************************************************************************/
