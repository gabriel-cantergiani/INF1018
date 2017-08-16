#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "conv_utf.h"


int main (void) {

	FILE *f1,*f2;
	 char i=1;

	f1 = fopen ("C:/Users/Gabriel/Documents/Visual Studio 2012/Projects/Teste01/Teste01/utf16_peq_big.txt","rb");
	printf ("OK1\n");
	f2 = fopen ("utf16_8_peq_saida.txt","wb");
	printf ("OK2\n");
	i = utf16_8(f1,f2);

	printf ("OK10\n");
	fclose(f1);
	fclose(f2);

	return 0;



}