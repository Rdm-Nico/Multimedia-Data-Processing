#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>

// is necessary to create a ricorsive struct 

template <typename T>
struct bencode_sct  {
	// vedere la struct come una tabella che viene chiamata ricosivamente
	// visto che il file torrent è un grande dict, immaginare  la classe come un dict <key,valore>
	std::string key_;
	std::string value_;

	// costruttore 
	bencode_sct(std::string key, std::string value) : key_(key), value_(value) {};
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