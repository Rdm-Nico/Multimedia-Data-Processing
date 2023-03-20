#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include<bitset>


template<typename T>
std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
}

void toElias(std::vector<int32_t>& v,std::ostream& os)
{
	// mappiamo in gamma di Elias 
	for (const  auto &x:v)
	{
		auto N = (int)log2(x);
		// rempiamo un buffer di 2(log_2(x)) + 1 bits
		auto dim = 2 * N + 1;

		uint8_t** arr = new uint8_t * [dim];
		for (size_t i = 0; i < N; i++)
		{
			arr[i] = 0;
		}


	}

}


void map(std::vector<int32_t>& v)
{
	for (size_t i = 0; i < v.size(); i++)
	{
		if (v[i] == 0)
		{
			v[i] = 1;
		}
		else if (v[i] < 0)
		{
			auto k = abs(v[i]);

			v[i] = k * 2;
		}
		else 
		{

			v[i] = (v[i]* 2)+1;
		}
	}
}


int main(int argc, char* argv[]) {

	if (argc != 4)
	{
		std::cout << "Usage: elias [c/d] <filein> <fileout>\n"; // operatore insert su uno stream, questo produce la printf( in pratica)
		return 1;
	}

	// guardiamo se il file deve essere compresso oppure decompresso 
	if (*argv[1] != 'c' & 'd')
	{
		std::cout << "errore nella sintassi [c\d]\n"; 
		return 1;
	}
	else if(*argv[1] == 'c')
	{
		// questo possiamo prendere l'es del prof 
		// apriamo il primo file in modalità testo 
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

		// mappiamo gli elem da 1 a infito 
		map(v);

		// apriamo un file output in modalità scrittura bin
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