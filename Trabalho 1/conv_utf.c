#include <stdio.h>
#include <stdlib.h>
#include "conv_utf.h"


int utf16_8(FILE *arq_entrada, FILE *arq_saida);
unsigned int converte_bits_unicode (char c,unsigned char* uni, int n);
unsigned short int converte_unicode_utf16 (unsigned int unicode, int n);
unsigned int gera_unicode (int a1, int a2, int a3, int a4, int a5);

int utf8_16 (FILE *arq_entrada,FILE *arq_saida){

FILE* f = arq_entrada;

unsigned char c,d;  /* variáveis c d para ler bytes do arquivo. */
unsigned char *uni = (unsigned char*)malloc(4*sizeof(unsigned char*)); /*variável para armazenar bloco restantes de bytes à ler, para cada unicode*/
int n=0; /*variável para contar bytes à ler em cada unicode*/
unsigned int unicode ; /* Variável para armazenar cada unicode */
unsigned short int cunit1,cunit2;  /* Variáveis para armazenar os code units em UTF-16 */
printf ("OK3\n");

/* Coloca o marcador BOM no início do arquivo */
	fputc(0xFE,arq_saida);
	fputc(0xFF,arq_saida);
	printf ("OK4\n");

	c=fgetc(f);  /*lê o primeiro byte do unicode a ser lido, para estabelecer o total de bytes neste unicode */
	printf ("OK5, %c,%x\n",c,c);
/* Início do loop de leitura e gravação. A cada iteração: Lê o número de bytes do unicode, lê os bytes restantes, converte p/ unicode, depois para utf-16 e grava no arquivo de saída */
	while((char)c!=EOF){  
		if((c>>4) == 15) 
			n=4;
		else if((c>>4) == 14)
			n=3;
		else if(((c>>4) == 12) || ((c>>4) == 13))
			n=2;
		else      		/* estabelece o número de bytes no unicode sendo tratado. atribui em n. */
			n=1;
		printf ("OK7, n = %d\n",n);	
		
		if (n!=1){
		d = fread(uni,sizeof(char),(n-1),f);   /* lê o bloco de bytes referente ao unicode sendo tratado e armazena no vetor uni*/
		printf ("OK8, uni[0] = %x, uni[1] = %x, uni[2] = %x, uni[3] = %x \n", uni[0],uni[1],uni[2],uni[3]);}
		unicode = converte_bits_unicode (c,uni,n) ; 
		printf ("OK9, unicode = %d, %08x\n",unicode,unicode);
		if ((unicode>>16) < 1){											/* Checa se Unicode está na faixa de conversão direta */
		cunit1 = converte_unicode_utf16(unicode,0);
		d = fwrite (&cunit1,sizeof(unsigned short int),1,arq_saida);
		printf ("OK10, unicode = %d, %08x\n", cunit1,cunit1);}    /* escreve no arquivo de saída o conteúdo de cunit1, que representa diretamente o unicode no caso do if  */
		else{
		cunit1 = (unsigned short int) converte_unicode_utf16 (unicode,1); /* chama função que converte o unicode para os codeunits 1 e 2. O segundo parâmetro define qual code unit deve ser retornado */
		d = fwrite (&cunit1,sizeof(unsigned short int),1,arq_saida);
		printf ("OK11,code unit 1 = %d,%04x\n", cunit1,cunit1);
		cunit2 = (unsigned short int) converte_unicode_utf16 (unicode,2);
		d = fwrite (&cunit2,sizeof(unsigned short int),1,arq_saida);
		printf ("OK12,code unit 2 = %d, %04x\n", cunit2, cunit2);}
		
		c=fgetc(f);
		
		printf ("OK13, novo c = %c,%x\n",c,c);/* Lê byte de onde está o marcador no arquivo de entrada, pulando o marcador para o próximo byte */
		}

	return 0;
}


unsigned int gera_unicode (int a1, int a2, int a3, int a4, int a5){
	
	unsigned int unicode;
	
	unicode =  a1;	
	
	a2 = (a2<<4); 
	unicode = unicode |  a2 ;
	a3 = (a3<<8);
	unicode = unicode |  a3 ;
	a4 = (a4<<12); 
	unicode = unicode |  a4 ;
	a5 = (a5<<16); 
	unicode = unicode |  a5 ; 
	
	return unicode ;

}



unsigned int converte_bits_unicode (char c, unsigned char* uni, int n){

		
	unsigned char b1,b2,b3,b4,b5,b6;  /* variáveis b1,b2,b3,b4,b5,b6 como auxiliares para manipular os bits */
	unsigned int a1=0,a2=0,a3=0,a4=0,a5=0;  /* variáveis para guardar cada parte do código (bits) de cada unicode*/
	unsigned int unicode ;
	
		if (n!=1){
		b1 = uni[n-2];}         /* Atribuição de variavel b1 auxiliar para guardar bytes restantes do unicode tratado. (subtraindo menos 2 porque o primeiro byte já está na var. c */
		else
		b1 = c;
		
	/* manipulação dos bits para obter os valores desejados de cada elemento do unicode  e atribuí-los às variáveis a1,a2,a3,a4,a5. */
	
		if (n==1){            /* Caso onde o unicode está no intervalo U+0000 a U+007F */
		b1=(b1<<4);b1=(b1>>4);
		a1 = b1;
		b2 = c;	
		b2=(b2>>4);
		a2 = b2;
		}
		else if (n==2){      /* Caso onde o unicode está no intervalo U+0080 a U+07FF */
		b1=(b1<<4);b1=(b1>>4);
		a1 = b1;	
		b1 = uni[0];  b1=(b1<<2);b1=(b1>>6);
		b2 = c; b1=(b2<<6); b1=(b2>>4);
		a2 = b1 | b2;
		b3 = c; b3=(b3<<3); b3=(b3>>5);
		a3 = b3;
		}
		else if (n==3) {     /* Caso onde o unicode está no intervalo U+0800 a U+07FF */
		b1=(b1<<4);b1=(b1>>4);
		a1 = b1;	
		b1 = uni[1];  b1=(b1<<2);b1=(b1>>6);
		b2 = uni [0]; b2=(b2<<6); b2=(b2>>4);
		a2 = b1 | b2;	
		b3 = uni [0]; b3=(b3<<2); b3=(b3>>4);
		a3 = b3;
		b4 = c; b4=(b4<<4); b4=(b4>>4);
		a4 = b4;
		}
		else if(n==4){     /* Caso onde o unicode está no intervalo U+10000 a U+10FFFF */
		b1=(b1<<4);b1=(b1>>4);
		a1 = b1;
		b1 = uni[2];  b1=(b1<<2);b1=(b1>>6);
		b2 = uni [1]; b2=(b2<<6); b2=(b2>>4);
		a2 = b1 | b2;
		b3 = uni [1]; b3=(b3<<2); b3=(b3>>4);
		a3 = b3;
		b4 = uni[0]; b4=(b4<<4); b4=(b4>>4);
		a4 = b4;
		b5 = uni[0]; b5=(b5<<2); b5=(b5>>6);
		b6 = c; b6=(b6<<6); b6=(b6>>4);
		a5 = b5 | b6;
		}
	
	/* junção das diferentes partes para gerar o unicode. */
	
	unicode = gera_unicode (a1,a2,a3,a4,a5) ;
	
	return unicode ;
	
	
}



unsigned short int converte_unicode_utf16 (unsigned int unicode, int n){
	
	unsigned int a1;
	unsigned short int cunit1,cunit2,aux1,aux2 ;
	short int b1;
	
	a1 = unicode - 0x10000 ;
	if(n==0){								/* Caso onde n=0 -> Conversão é direta. Somente inverte a ordem para Big Endian e retorna */
	cunit1 = unicode ;										
	aux1 = (cunit1<<8); aux2 = (cunit1>>8);  /*Inverte a ordem para Big Endian */
	cunit1 = aux1 | aux2 ;
	return cunit1;
	}
	else if (n == 1){
	cunit1 = ((a1>>10) + 0xD800);
	aux1 = (cunit1<<8); aux2 = (cunit1>>8);  /*Inverte a ordem para Big Endian */
	cunit1 = aux1 | aux2 ;
	return cunit1; }
	else  {
	b1 = a1; b1 = (b1<<6); b1 = (b1>>6) ;
	cunit2 = b1 + 0xDC00;
	aux1 = (cunit2<<8); aux2 = (cunit2>>8);  /*Inverte a ordem para Big Endian */
	cunit2 = aux1 | aux2 ;
	return cunit2; }
	
}
		
	
	
int utf16_8(FILE *arq_entrada, FILE *arq_saida)
{
	
FILE* f = arq_entrada;

unsigned int a,b,c,d,e,codigo;	/* variáveis c d para ler bytes do arquivo. */
unsigned short int par1,par2;  
int contaByte = 0 ; /* Variável para contar byte */
//unsigned char *uni = (unsigned char*)malloc(4*sizeof(unsigned char*)); /*variável para armazenar bloco restantes de bytes à ler, para cada unicode*/
char k,primeiroByte,segundoByte,terceiroByte,quartoByte;  /* Variáveis para armazenar cada byte para utf-8*/

	primeiroByte = fgetc(f);
	segundoByte = fgetc(f);
	par1 = primeiroByte << 8;
	par1 = par1 | segundoByte;
	if(par1 == 0xFEFF)
	{                               // checa o BOM
	printf("BOM DEFINIDO CORRETAMENTE");
	}
	primeiroByte = fgetc(f);
	
	while ((char)primeiroByte!=EOF){
		
		// Primeira etapa: ler do arquivo e converter os bits para unicode
		segundoByte = fgetc(f);
		if(segundoByte == EOF)
			break;
		par1 = (unsigned short int) primeiroByte; par2 = (unsigned short int) segundoByte;
		par1 = par1 | (par2<<8) ;
		
		if((par1>=0x0000 && par1<0xD800))    // se o codigo em hexa tiver ente 0x0000 e 0xD800 ele é igual ao unicode
			codigo = par1;
		
		else if (par1>=0xD800) {  // se o par tiver entre 0xD800 && 0xDBFF ele vai ler o terceiro e quarto byte e armazena o segundo par
		
		terceiroByte = fgetc(f);
		quartoByte = fgetc(f);
		par2 = (unsigned short int) terceiroByte; par2 = (par2<<8);
		par2 = par2 | (unsigned short int) quartoByte;
		
		par1 = (int) (par1 - 0xD800);
		par2 = (int) (par2 - 0xDC00);
		
		par1 = (par1<<10);
		codigo = (par1 | par2) + 0x10000;
		}	
	
		// Proxima etapa: converter o unicode para bits e gravar no arquivo.
		
		if (codigo>=0x0000 && codigo<=0x007F){		//Primeiro caso: Unicode representado por 1 byte
		
		k = codigo;
		fwrite(&k,sizeof(char),1,arq_saida);	
		}
	
		else if(codigo>=0x0080 && codigo<=0x07FF){    //Segundo caso: Unicode é representado por 2 bytes
			
			a = (codigo<<10); a = (a>>10); a = a | (0x80); 
			fputc(a,arq_saida);									//Separa os 6 primeiros bits e escreve no arquivo junto com a assinatura de byte de continuação
			
			b = (codigo<<4); b = (b>>10);  b = b | (0xC0); 
			fputc(b,arq_saida);									// Separa os 6 proximos bits e escreve no arquivo junto com a assinatura de contagem de bytes.
		
		}
	
		else if (codigo>=0x800 && codigo<=0xFFFF){
			
			a = (codigo<<10); a = (a>>10); a = a | (0x80); 
			fputc(a,arq_saida);									//Separa os 6 primeiros bits e escreve no arquivo junto com a assinatura de byte de continuação
			
			b = (codigo<<4); b = (b>>10);  b = b | (0x80); 
			fputc(b,arq_saida);									// Separa os 6 proximos bits e escreve no arquivo junto com a assinatura de byte de continuação
			
			c = (codigo>>12) ; c = c | 0xE0 ;					
			fputc(c,arq_saida);									//Separa os 6 proximos bits e escreve no arquivo junto com a assinatura de contagem de bytes
			
		}
		
		else if (codigo>=0x10000 && codigo<=0x10FFFF){
			
			a = (codigo<<18); a = (a>>18); a = a | (0x80); 
			fputc(a,arq_saida);									//Separa os 6 primeiros bits e escreve no arquivo junto com a assinatura de byte de continuação
			
			b = (codigo<<12); b = (b>>18);  b = b | (0x80); 
			fputc(b,arq_saida);									// Separa os 6 proximos bits e escreve no arquivo junto com a assinatura de byte de continuação
			
			c = (codigo>>10) ; c = (c>>22); c = c | 0x80 ;					
			fputc(c,arq_saida);									//Separa os 6 proximos bits e escreve no arquivo junto com a assinatura de byte de continuação
			
			d = (codigo<<2) ;  d = (d>>20); d = d | 0xF0 ;		//Separa os 6 proximos bits e escreve no arquivo junto com a assinatura de contagem de bytes
			fputc(d,arq_saida);
		
		}
		
	
	
	primeiroByte = fgetc(f);
	}
	
	
	return 0;
	
}