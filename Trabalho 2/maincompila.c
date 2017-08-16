#include <stdio.h>
#include "compila.h"
	

int main(int argc,char *argv[]) {
	FILE *f = fopen("teste3.simples","r");
	funcp p;	
	int k, p1,p2;
	p1 = atoi(argv[1]);
	p2 = atoi(argv[2]);
	p = compila (f);	
	k = (*p) (p1,p2);
	printf(" Resultado: v1 = %d\n\n", k);

return 0;
}
