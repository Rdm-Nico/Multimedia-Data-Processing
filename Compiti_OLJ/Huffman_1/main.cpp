#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include<string>


int main(int argc, char* argv[]) {
	if (argc != 4)
	{
		std::cout << "Errore nel  numero di parametri passati";
		return 1;
	}

	std::string mode = argv[1];
	if (mode != "c" && mode != "d") {
		std::cout << "errore nella sintassi";
		return 1;
	}
	else if (mode == "c") {
		// apriamo il file in modalita binario 
		std::ifstream is(argv[2], std::ios::binary);
		//if (!is) {
			//std::cout << "Errore nell'apertura";
			//return 1;
		//}
		// leggiamo i valori da file

		std::istream_iterator<int32_t> is_start(is);
		std::istream_iterator <int32_t> stop;

		// inizzializziamo il vettore con tutti i valori 
		std::vector<int32_t> v(is_start, stop);

	}
	return 0;
}