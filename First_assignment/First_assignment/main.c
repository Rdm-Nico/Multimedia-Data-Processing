#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdbool.h>
#include<time.h>


void Scambia(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

/*void  Bubble(int dim, int vet[]){
	int i;
	bool ordinato = false;

	while (dim> 1 && !ordinato)
	{
		ordinato = true;
		for( i=0; i < dim-1;i++ )
			if(vet[i] > vet[i+1]) {
				scambia(&vet[i], &vet[i+1]);
				ordinato = false;
			}
		dim--;
	}
}*/

int main(int argc, char* argv[]) {
	//  zona parametri
	FILE* filein, * fileout;
	char ch;
	char* value;
	int dim_2 = 1, num = 0, dim = 1;
	int* values;
	clock_t t1, t2;

	t1 = clock();
	// controlliamo  che i parametri siano corretti
	if (argc != 3)
	{
		printf("errore nel numero di parametri passati");
		return 1;
	}

	// apertura dei file 
	if (!(filein = fopen(argv[1], "r"))) {
		perror(argv[1]);
		return 1;
	}
	if (!(fileout = fopen(argv[2], "w"))) {
		perror(argv[2]);
		return 1;
	}
	// allochiamo lo spazio di memoria 
	if ((value = (char*)malloc(dim_2)) == NULL)
	{
		printf("Errore nella allocazione\n");
		exit(1);
	}
	printf("valore di value: %c\n", &value[0]);

	if ((values = (int*)malloc(dim * sizeof(int))) == NULL)
	{
		printf("Errore nella allocazione\n");
		exit(1);
	}

	//lettura di caratteri nel file di input 
	while (ch = getc(filein))
	{
		if (isdigit(ch))
		{
			// numero decimale da inserire dentro all'array
			if (dim_2 == 1)
			{
				value[0] = ch;
				dim_2++;
			}
			else
			{
				// ri allochiamo 
				value = realloc(value, (dim_2));

				value[dim_2 - 1] = ch;
				dim_2++;
			}
			continue;
		}
		if (ch == '-')
		{
			value = realloc(value, (dim_2));
			value[dim_2 - 1] = ch;
			dim_2++;
			continue;
		}
		if ((ch == '\n') || (ch == ' ') || (ch == EOF))
		{
			// salviamo il valore trovato nell'array 
			value = realloc(value, (dim_2));
			value[dim_2 - 1] = '\0';

			sscanf(value, "%d", &num);

			if (dim == 1)
			{
				values[0] = num;
				dim++;
			}
			else
			{
				// ri allochiamo 
				values = realloc(values, (dim) * sizeof(int));
				values[dim - 1] = num;
				dim++;

			}
			// azzeriamo l'indice di value5
			dim_2 = 1;
			value = realloc(value, (dim_2));

			if (ch == EOF)
			{
				break;
			}

		}
		else
		{
			value = realloc(value, (dim_2));
			value[dim_2 - 1] = '\0';

			sscanf(value, "%d", &num);
			// ri allochiamo 
			values = realloc(values, (dim) * sizeof(int));
			values[dim - 1] = num;
			dim++;

			break;
			// incontrato un valore non accettabile

		}

	}
	// sistemiamo la dimensione
	dim--;

	//Bubble(dim,values);


	int i;
	bool ordinato = false;
	size_t real = dim;
	while (dim > 1 && !ordinato)
	{
		ordinato = true;
		for (i = 0; i < dim - 1; i++)
			if (values[i] > values[i + 1]) {
				Scambia(&values[i], &values[i + 1]);
				ordinato = false;
			}
		dim--;
	}

	for (int i = 0; i < real; i++)
	{
		fprintf(fileout, "%d\n", values[i]);
	}

	t2 = clock();

	printf("tempo di esecuzione: %.10f sec\n", (t2 - t1) / (double)(CLOCKS_PER_SEC));

	fclose(filein);
	fclose(fileout);
	return 0;
}