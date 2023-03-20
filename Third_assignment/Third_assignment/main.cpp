#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include<bitset>
#include<string>
#include<cmath>




template<typename T>
std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
}


class bitwriter {
	uint8_t buffer_;
	int n_ = 0; //  numero di elementi nel buffer 
	std::ostream& os_; // memorizziamo lo stream con una reference  e lui si occupa di gestire la memoria 
	// � un riferimento perch� non vogliamo copiare lo stream, vogliamo solo accedere a quello che � gi� stato creato
	// quando verr� distrutto il bitwriter lo stream verr� distrutto anche lui

	std::ostream& write_bit(uint8_t bit) {
		buffer_ = buffer_ * 2 + bit;
		n_++;
		if (n_ == 8) { // se n � 8 scriviamo il buffer
			raw_write(os_, buffer_);
			n_ = 0; // resetto il buffer
		}
		return os_;
	}
	// rendiamo write_bit privata perch� non vogliamo che venga chiamata dall'esterno
public:

	bitwriter(std::ostream& os) : os_(os) {}// non esiste un costruttore di defalut per un output stream

	~bitwriter() {
		flush(); // quando distruggiamo il bitwriter facciamo il flush perch� potrebbero esserci dei bit rimasti nel buffer
		// prima verr� distrutto il bitwriter e poi l'output stream
	}

	std::ostream& flush(uint32_t bit = 0) {
		while (n_ > 0) {
			write_bit(bit);
		}
		return os_;
	}
	std::ostream& write(uint32_t u, uint8_t n) {
		// while (n_ --> 0) // prendo n lo decremento e lo confronto con 0
		// {
		//     write_bit(u >> n_);
		// }

		for (int i = n - 1; i >= 0; --i) {
			uint32_t cure_bit = (u >> i) & 1; // prendiamo il bit i-esimo
			write_bit(cure_bit);
		}
		return os_;
	}

	std::ostream& operator()(uint32_t u, uint8_t n) { // alternativa a chiamare la  write
		return write(u, n);
	}

};


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
		// questo possiamo prendere l'es del prof 
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

		// mappiamo gli elem da 1 a infito 
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