/* una cosa da esame piu probabile 
usare una struct elem con char type_ ('s') 
una stringa_ 
int_
list_ (che sara un vector<elem>)
dict_ (che sara map<string,elem>)


costruttore di default

elem() : type_(0) {}

ora possiamo fare un costruttore 
elem(std:istream &is){
	switch(is.peek()){
	controlliamo prima cos'e senza estrarlo 
	case 'i':
		is >> type_ >> i_ >> end; tipo , valore e 'e'
	}
	case 'l':
		is >> type_;
		while(is.peek() != 'e'){
			list_.emplace_back(is); emplace back allunga un po la lista e utilizza elem senza copiarlo 
		}
		is >> end;
	case 'd':
		is >> type_;
		while(is.peek() != 'e'){
			elem key(is);
			dict_[key.s_] = elem(is);
		}
		is >> end;
	default:
		type_ = 's'; per dire che é una striga
		size_t = len;
		char colon;
		is >> len >> colon;
		s_.resize(len);
		is.read(&s[0], len)
}


la print come si fa:
si fa un metodo print
void print(size_t tab){ tab dice quanti tab fare 
	switch(type_)
	case 'i':
		std::cout << i_;
	case 'l':
		std::cout << "[";
		for(const auto &x : list_) {
			std::cout << "\n";
			std:: cout << std::string(tab+1, '\t'); tabba di un certo numero di tab
			x.print(tab + 1)
		}
		std::cout << "\n";
		std::cout << "]";
		std:: cout << std::string(tab, '\t')
	case 'd': stessa cosa per d 
		if(x.first == "pieces") {
			const auto &p =  x.second.s_;
			for(size_t i = 0; i < p.size(); i++)
			std::cout << "\n";
			std:: cout << std::string(tab+1, '\t'); tabba di un certo numero di tab
			x.print(tab + 1)
			per convertire in base 16 devo farlo diventare un unsigned int 
			da char -> unsigned char ->  int 
		}
	case 's':
		for(const auto &x : s_){
		std::cout << s_;
		std::cout << (31 < x && x > 127 ? s_ )
		}
	default:;
}
*/