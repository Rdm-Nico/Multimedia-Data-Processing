#include<fstream>
#include<iostream>
#include "mat.h"
#include "pgm.h"
#include "ppm.h"
#include "types.h"
#include "utils.h"
#include<iterator>
#include<map>
#include<sstream>
#include<iomanip>



bool Check_Magic_Number(std::ifstream& is, const std::string& magic_number) {
	std::string word;

	is >> word;
	if (word != magic_number)
		return false;
	return true;
}

bool Read_Header(std::ifstream& in, std::map < uint8_t, std::string > & header) {
	using namespace std;
	if(!Check_Magic_Number(in, "YUV4MPEG2")){
		return false;
	}
	
	string chroma = "420jpeg";
	string v;

	const char poss[] = { 'W','H','C','I','F','A','X' }; // caratteri dell'header da leggere 
	// notare che  comunque da traccia dovremmo trovare solo la W, la H e il chroma
	
	header['C'] = chroma;

	while (true) {
		bool flag = false;
		char next = in.get();
		if (next == '\n') { // siamo arrivati alla fine dell'header
			break;
		}
		if (next != ' ') { // c'e' stato un errore nella lettura del file 
			return false;
		}
		char tag = in.get();

		for (auto e : poss) {
			if (tag == e) {
				flag = true; // abbiamo trovato un tag coretto
			}
		}

		if (!flag) {
			return false;
		}
		in >> v;
		header[tag] = v;
	}
	if (header.count('W') != 1 || header.count('H') != 1)
		return false;

	return true;
}


bool readFrames(std::ifstream& in, std::map < uint8_t, std::string >& header, std::vector<mat<uint8_t>>& frames) {
	using namespace std;

	string frame_magic_number; // FRAME
	int H = stoi(header['H']);
	int W = stoi(header['W']);

	mat<uint8_t> Y(H, W); // la matrice della luminanza 
	mat<uint8_t> Cb(H / 2, W / 2);
	mat<uint8_t> Cr(H / 2, W / 2);

	/* Ricordare che non appena si supera il magic number FRAME si devono leggere:
	*						288*352 = 101376 elementi di Y
	*					e poi 101376/4 = 25344 elementi di Cb e 25344 elementi di Cr
	*/

	string interlacing, application;
	while (true) {
		if (in.fail())
			return false;
		if (in.eof())
			break;
		in >> frame_magic_number;
		if (frame_magic_number != "FRAME") {
			return false;
		}

		// ogni frame contiene interlacing(I), application(X)
		while (true) {
			char next = in.get();
			if (next == '\n') { // siamo arrivati alla fine dell'header
				break;
			}
			if (next != ' ') { // c'e' stato un errore nella lettura del file 
				return false;
			}
			char tag = in.get();

			switch (tag)
			{
			case 'I':
				in >> interlacing;
				break;
			case 'X':
				in >> application;
				break;
			default:
				return false;
			}
		}


		// adesso leggiamo i dati: 
		// " Frames are made up of the Y values of all pixels, followed by all Cb values,
		// followed by all Cr values. Cb and Cr are suitably subsampled"

		in.read(Y.rawdata(), Y.rawsize()); // rawdata e' il puntatore alla prima posizione dei dati della matrice 
		if (in.fail())
			return false;
		in.read(Cb.rawdata(), Cb.rawsize());
		if (in.fail())
			return false;
		
		in.read(Cr.rawdata(), Cr.rawsize());
		if (in.fail())
			return false;

		frames.push_back(Y); 
	}
	return true;
}


bool y4m_extract_gray(const std::string& filename, std::vector<mat<uint8_t>>& frames) {
	using namespace std;
	ifstream is(filename, std::ios::binary);
	if (!is) {
		cout << "error of the file" << std:: endl;
		return false;
	}

	map<uint8_t, string> header; // una mappa carattere, stringa per contenere i valori dell'header 
	if (!Read_Header(is, header)) {
		cout << endl << "error in file passed" << endl;
	}

	int H = stoi(header['H']);
	int W = stoi(header['W']);

	if (W < 0 || H < 0 || header['C'] != "420jpeg") {
		return false;
	}

	readFrames(is, header, frames);

	return true;
}
int main(void) {
	using namespace std;
	vector<mat<uint8_t>> frames;

	if (y4m_extract_gray("720p_stockholm_ter.y4m", frames)) {
		for (size_t f = 0; f < frames.size(); ++f) {
			stringstream ss; // stream per le stringhe 
			stringstream prova;
			ss << setfill('0');
			prova << setfill('0');
			prova << "frame" << f << ".pgm";
			cout << prova.str() << "\t";
			ss << "frame" << setw(3) << f << ".pgm";
			cout << ss.str() << endl;
			save_pgm(ss.str(), frames[f]);
		}
	}

	return 0;
}