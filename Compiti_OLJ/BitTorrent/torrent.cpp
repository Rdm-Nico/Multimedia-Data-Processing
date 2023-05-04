#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<map> // per mappare 
/*

il prof ha fatto un namespace 
poi un oggetto generico che serve per generare il backend -> è un interfaccia, classe astratta
il prof usa un factory method(pattern desing) 
elem_factory legge uno steam e torna un puntatore ad elem 

poi crea delle classi int , lista, dict e str che eredita elem factory

Per la classe int devi tenere conto che esso incomincia con un carattere 'i' e finisice con un carattere 'e' 
anche la classe str è meno complessa delle altre 
per la classe lista viene chiamata elem_factory ricorsivameente 
per la classe dict la stessa cosa di stringa , usando però la classe map


NON E' DETTO CHE DOVETE FARE QUESTA SOLUZIONE DURANTE L'ESAME -> E' UN PO DIFFICILE 
*/

// is necessary to create a ricorsive struct 

//template <typename T> // non si usa i template perchè vengono risolti a tempo di compilazione -> noi vogliamo a tempo di eseguzione


/* sotto consiglio del prof :
	fare un altra struct con questi attributi: un attributo int, un attributo string e poi un attributo tipo( che potrebbe essere un enumarete oppure un semplice valore che dice:
	0 = stringa , 1 = intero, 2 = lista, 3 = dizionario, ecc...
	*/
enum dtype {Str,Int,list,dict};
struct typo {
	std::string str_;
	int val_;
	dtype tipo_;

	// costruttore
	typo(std::string str, int val, dtype tipo) : str_(str), val_(val), tipo_(tipo) {};

	std::string str() const { return str_; }
	int val() const { return val_; }
	dtype tipo() const { return tipo_; }


};
struct bencode_sct  {
	// vedere la struct come una tabella che viene chiamata ricosivamente
	// visto che il file torrent è un grande dict, immaginare  la classe come un dict <key,valore>
	typo key_;
	typo value_;

	// costruttore 
	bencode_sct(typo key, typo value) : key_(key), value_(value) {};


};

int main(void) {
	// first we create a file 
	std::ifstream is("KickAss.torrent", std::ios::binary);
	if (!is)
		return -1;

	std::string magic;
	std::getline(is, magic);
	int i = 0;
	while (1)
	{
		std::cout << magic.at(i);
		std::cout << "\n";
		i++;
	}
	


	return 0;
}