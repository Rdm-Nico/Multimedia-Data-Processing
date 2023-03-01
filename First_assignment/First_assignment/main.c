#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

int main(int argc, char* argv[]) {
	//  zona parametri
	FILE* filein, * fileout;
	char ch;
	char* value;
	int dim_2 = 1, num = 0,dim = 1;
	int* values;
	
	
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
	if ((value = (char*)malloc(dim) == NULL))
	{
		printf("Errore nella allocazione\n");
		exit(1);
	}

	if ((values = (int*)malloc(dim * sizeof(int)) == NULL))
	{
		printf("Errore nella allocazione\n");
		exit(1);
	}

	//lettura di caratteri nel file di input 
	while ((ch = getc(filein)) != EOF)
	{
		if (isdigit(ch))
		{
			// numero decimale da inserire dentro all'array
			if (dim_2 == 1)
			{
				value[0] = ch;
			}
			else
			{
				// ri allochiamo 
				values = realloc(values, (dim_2 + 1) * sizeof(int));

				value[dim_2] = ch;
				dim_2++;
			}
		}
		if (ch == '-')
		{
			value[dim_2] = ch;
			dim_2++;
		}
		if ((ch != '\n')||(ch != ' '))
		{
			break;
			// incontrato un valore non accettabile
		}
		else
		{
			// salviamo il valore trovato nell'array 
			values = realloc(values, (dim_2 + 1) * sizeof(int));
			value[dim_2] = '\0';

			sscanf(value, "%d", &num);
			if (dim == 1)
			{
				values[0] = value;
			}
			else
			{
				// ri allochiamo 
				values = realloc(values, (dim + 1) * sizeof(int));
				values[dim] = value;
				dim++;

				// azzeriamo l'indice di valu
				dim_2 = 1;
			}
		}

	}

	for (int i = 0; i < dim; i++)
	{
		printf("%d\n", values[i]);
	}

	fclose(filein);
	fclose(fileout);
	return 0;
}