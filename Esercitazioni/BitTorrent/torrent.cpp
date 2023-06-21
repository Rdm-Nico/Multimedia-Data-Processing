#include<fstream>
#include<iostream>
#include<vector>
#include<sstream>
#include<iomanip>
#include<memory>
#include<cstdint>
#include<format>
#include<functional>
#include<map>


class elem {
	char type_;
	std::string s_;
	int64_t val_;
	std::vector<elem> list_;
	std::map<std::string, elem> dict_;
public:
	elem() : type_(0) {}

	elem(std::istream& is) {
		char end;
		switch (is.peek()) // potevo usare is.peek() per leggere senza estrarre 
		{
		case 'i': { // si tratta di un numero 
			is >> type_;
			is >> val_;
			is >> end; // 'e'
			break;
		}
		case 'l': {
			is >> type_;
			// ???? list_.emplace_back(elem(is)); ???

			while (is.peek() != 'e') {
				list_.emplace_back(is);
			}
			is >> end;
			break;
		}
		case 'd': {
			is >> type_;
			while (is.peek() != 'e') {
				elem key(is); // si prende la chiave
				dict_[key.s_] = elem(is);
			}
			is >> end;
			break;
		}
		default: {
			// avremo una stringa
			type_ = 's';
			size_t len;
			char colon;

			is >> len;
			is >> colon;
			s_.resize(len);
			is.read(&s_[0], len);
			break;
		}
		}
	}


	void print(size_t tab) const { // il numero di tab
		switch (type_)
		{
		case 'i': {
			std::cout << val_;
			break;
		}
		case 's': {
			//  devono essere visualizzati solo i byte con un valore maggiore di 32 oppure minore di 126, valori fuori verrano stampati con '.'
			for (const auto& c : s_) {
				std::cout << (c < 32 || c > 126 ? '.' : c);
			}
			break;
		}
		case 'l': {
			std::cout << "[";

			for (const auto& l : list_) {
				std::cout << "\n";
				std::cout << std::string(tab + 1, '\t'); // una stringa costruita con un certo numero di atb+1 elementi di '\t'
				l.print(tab + 1);
			}
			std::cout << "\n";
			std::cout << std::string(tab, '\t');
			std::cout << "]";
			break;
		}
		case 'd': {
			std::cout << "{";

			for (const auto& d : dict_) {
				std::cout << "\n";
				std::cout << std::string(tab + 1, '\t');
				std::cout << d.first << " => ";
				// c'e' da tenere conto del campo pieces
				if (d.first == "pieces") {
					const auto& p = d.second.s_;
					std::cout << std::hex << std::setfill('0'); // incominciamo a scivere in exa

					for (size_t i = 0; i < p.size(); ++i) {
						if (i % 20 == 0) { // siamo arrivati al 20esimo elemento
							std::cout << "\n";
							std::cout << std::string(tab + 1, '\t');
						}
						printf("%02x", p[i]);
						// std::cout << std::setw(2) << int(unsigned char(p[i]));
					}
					std::cout << std::dec; // torniamo a scrivere in decimale
				}
				else {
					d.second.print(tab + 1);
				}
			}
			std::cout << "\n";
			std::cout << std::string(tab, '\t');
			std::cout << "}";
			break;
		}
		default:
			break;
		}
	}

};


int main(void) {
	using namespace std;
	string filename = "MonkeyIsland.torrent";

	ifstream is(filename, ios::binary);
	if (!is) {
		return EXIT_FAILURE;
	}
	elem root(is);
	root.print(0);

	return EXIT_SUCCESS;
}