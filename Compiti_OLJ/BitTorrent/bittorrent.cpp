/* seconda versione che produce correttamente anche i peaces */
/* fare una struttura generica che chaima una funzione in grado di fare  */
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include <exception>
#include <functional>
#include <format>
#include <memory>

namespace ben {
	struct elem {
		virtual void print(size_t tab = 0) const = 0;
		virtual ~elem() {}
	}; /* c'e da fare anche il distruttore virtuale  */

	elem *elem_factory(std::istream &is);

	struct string : public elem {
		std::string val_;

		string(std::istream &is) {
			auto error = std::runtime_error("Error while parsing string");
			size_t len;
			char colon;
			is >> len >> colon;
			if (!is || colon != ':') {
				throw error;
			}
			val_.resize(len);
			is.read(&val_[0], len);
			if (!is) {
				throw error;
			}
		}

		void print(size_t tab) const override {
			for (const auto &x : val_) {
				std::cout << (32 <= x && x <= 126 ? x : '.');
			}
		}
	};

	struct integer : public elem {
		int64_t val_;

		integer(std::istream &is) {
			char start, stop;
			is >> start >> val_ >> stop;
			if (!is || start != 'i' || stop!= 'e') {
				throw std::runtime_error("Error while parsing integer");
			}
		}

		void print(size_t tab) const override {
			std::cout << val_;
		}
	};

	struct list : public elem {
		std::vector<elem*> val_;

		list(std::istream &is) {
			auto error = std::runtime_error("Error while parsing list");
			char start;
			is >> start;
			if (!is || start != 'l') {
				throw error;
			}
			while (is.peek() != 'e') {
				val_.push_back(elem_factory(is));
			}
			is.get(); // remove 'e'
		}
		
		/* bisogna fsre il distruttore di list  */
	

		void print(size_t tab) const override {
			std::cout << "[\n";
			for (const auto &x : val_) {
				std::cout << std::string(tab + 1, '\t');
				x->print(tab + 1);
				std::cout << "\n";
			}
			std::cout << std::string(tab, '\t') << "]";
		}
	};

	struct dictionary : public elem {
		std::map<std::string, elem*> val_;

		static std::map<std::string, std::function<void(const elem*,size_t)>> special_print; /* serve per cambiare la print  */

		dictionary(std::istream &is) {
			auto error = std::runtime_error("Error while parsing dictionary");
			char start;
			is >> start;
			if (!is || start != 'd') {
				throw error;
			}
			while (is.peek() != 'e') {
				string s(is);
				val_[s.val_] = elem_factory(is);
			}
			is.get(); // remove 'e'
		}
		/* dobbiamo fare il distruttore  del  dict  */

		void print(size_t tab) const override {
			std::cout << "{\n";
			for (const auto &x : val_) {
				std::cout << std::string(tab + 1, '\t') << x.first << " => ";
				auto sp = special_print.find(x.first);/* una mappa che associa ad ogni stringa una funzione diversa, e poi cerco una chiave, é torna un iteratore di un albero, se non lo trova é uguale a end() */
				if (sp != special_print.end()) {
					sp->second(x.second, tab + 1);/* chiamo il puntatore a funzione  */
				}
				else {
					x.second->print(tab + 1);
				}
				std::cout << "\n";
			}
			std::cout << std::string(tab, '\t') << "}";
		}
	};
	std::map<std::string, std::function<void(const elem*,size_t)>> dictionary::special_print;

	elem *elem_factory(std::istream &is) {
		switch (is.peek()) {
		break; case 'i': return new integer(is);
		break; case 'l': return new list(is);
		break; case 'd': return new dictionary(is);
		break; default: return new string(is);
		}
	}
}

void print_pieces(const ben::elem* e, size_t tab) {/* ricevo un elemento generico  */
	auto pieces = dynamic_cast<const ben::string*>(e); /* come il reinterpret_cast, ma se il tipo é diverso, ritorna il null pointer, e lavora a tempo di compilazione(imp per questo es!!)
	
	runtime type information(puó verificare il tipo che chiedete se é corretto )*/
	if (!pieces) {
		throw std::runtime_error("pieces is not a string!");
	}
	if (pieces->val_.size() % 20 != 0) {
		throw std::runtime_error("pieces len is not a multiple of 20!");
	}
	for (size_t i = 0; i < pieces->val_.size(); i += 20) {
		std::cout << "\n" << std::string(tab + 1, '\t');
		for (size_t j = 0; j < 20; ++j) {
			std::cout << std::format("{:02x}", unsigned char(pieces->val_[i + j]));
		}
	}
}
/* invdce di usare i puntatorire e poi fare la delete, si puo provare a fare qualcosa di automatico:
si deve fare il reference counter(guarda come funziona) 
unice pointer e shared pointer( puntatori unice sono classi private che non può essere copiata. shared dice che )
appena l'oggetto pointer esce dallo scope fa la delete

si può evitare di fare i distuttori delle list e dei dict mettendo i puntatori unique pointer */

int main(int argc, char *argv[]) {/* con _crtBreakAlloc si blocca alla allocazione numerata  */
	if (argc != 2) {
		std::cout << "SYNTAX: " << argv[0] << "<file .torrent>\n";
		return EXIT_FAILURE;
	}

	std::ifstream is(argv[1], std::ios::binary);
	if (!is) {
		std::cout << "Cannot open " << argv[1] << "\n";
		return EXIT_FAILURE;
	}
	

	ben::dictionary::special_print["pieces"] = print_pieces;/* ad ogni nome del tipo del dict, cambiava la print */

	try {
		ben::elem *root = ben::elem_factory(is); /* questo deve diventrare unique pointer  */
		root->print(0);
		delete root; /* dobbiamo fare anche questo  */
	}
	catch (std::exception& e) {
		std::cout << e.what() << '\n';
	}
	/* però questo codice ha un problema: 
	ci sono problemi di memory leak, che non sono stati eliminati 


 */

	return 0;
}