#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include "pgm.h"

bool y4m_extract_gray(const std::string& filename, std::vector<mat<uint8_t>>& frames){
	/*
	La funzione deve salvare i piani T di tutti i frame in scala di grigi nel vettore frames. I programma dovrebbe supportare
	solo C420jpeg econding con interlacciamento progressivo e ignora il frame rate, aspect ratio e altri parametri.
	*/
	std::ifstream is(filename, std::ios::binary);
	if (!is)
		return false;

	// utlizziamo un parser 
	std::string magic_number;
	std:getline(is, magic_number);

	// nel magic number abbiamo -> YUV4MPEG2 H288 W352 C420jpeg

	int w, h; // larghezza e altezza 
	std::string c;
	
		// facciamo il parsing 
	std::stringstream ss(magic_number);
	std::string token;

	ss >> token; // estraiamo il primo token dalla stringa -> leggendo i caratteri fino al primo whitespace
	ss >> c;


	std::string line;
	std::getline(is, line);
	 // in line ora c'è -> FRAME
	if (line != "YUV4MPEG2")
		return false;




	
	
	
	return true;
}


int main()
{
	std::vector<mat<uint8_t>> frames;
	if (y4m_extract_gray("test1.y4m", frames)) { // per testare e salvare in un file pgm
		for (size_t i = 0; i < frames.size(); ++i) {
			std::stringstream ss;
			std::setfill('0'); // azzera tutto il fill  
			ss << "frame" << std::setw(3) << i << ".pgm";
			save_pgm(ss.str(), frames[i]);
		}
	}

	return 0;
}
