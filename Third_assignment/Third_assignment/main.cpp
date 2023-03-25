#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include<string>
#include<cmath>

import bit;





void toElias(std::vector<int32_t>& v,std::ostream& os)
{
	bitwriter rw(os);
	// mappiamo in gamma di Elias 
	for (const  auto &x :v)
	{
		//mappiamo in gamma di Elias
		// calcoliamo il logaritmo in base 2 di x
		auto N = (int)log2(x);
		
		// calcoliamo la dimensione in bit 
		auto dim = 2 * N + 1;
		// scriviamo N zero bits 
		rw(0, N);
		// scriviamo il numero x in binario 

	}

}


void map(std::vector<int32_t> v)
{
	for (auto &x : v)
	{
		if (x == 0)
		{
			x = 1;
		}
		else if (x < 0)
		{
			auto k = abs(x);

			x = k * 2;
		}
		else 
		{

			x = (x* 2)+1;
		}
		std::cout << x;
		std::cout << '\n';
	}
}


int main(int argc, char* argv[]) {

	if (argc != 4)
	{
		std::cout << "Usage: elias [c/d] <filein> <fileout>\n"; // operatore insert su uno stream, questo produce la printf( in pratica)
		return 1;
	}

	std::string mode = argv[1];

	// guardiamo se il file deve essere compresso oppure decompresso 
	if (mode != "c" && mode != "d")
	{
		std::cout << "errore nella sintassi [c\d]\n"; 
		return 1;
	}
	else if(mode == "c")
	{
		// apriamo il primo file in modalit� testo 
		std::ifstream is(argv[2]);

		if (!is) {
			std::cout << "Error opening input file.\n";
			return 1;
		}

		// leggiamo  i valori nel file 
		std::istream_iterator<int32_t> is_start(is);
		std::istream_iterator<int32_t> stop;

		// init the vector with all the values:

		std::vector<int32_t> v(is_start, stop);

		// mappiamo gli elem da 1 a infinito 
		map(v);

		

		// apriamo un file output in modalit� scrittura bin
		std::ofstream out(argv[3], std::ios::binary);
		if (!out) {
			std::cout << "Error opening output  file.\n";
			return 1;
		}
		
		toElias(v,out);


	}
	else
	{
		// decodifica 
		std::ifstream is(argv[2], std::ios::binary);
		std::ofstream out(argv[3]);
		if (!is || !out)
		{
			std::cout << "Error opening input/output file.\n";
			return 1;
		}

		int32_t num;
		while (1)
		{
			is >> num;
			int count = 0;
			while (num > 0) {
				++count;
				num >>= 1; // scinfto di uno 


			}
		}

	}


	return 0;
}