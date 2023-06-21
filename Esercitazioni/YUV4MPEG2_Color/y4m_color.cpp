#include<iostream>
#include<iomanip>
#include<fstream>
#include "mat.h"
#include "pgm.h"
#include "ppm.h"
#include "types.h"
#include "utils.h"
#include<sstream>
#include<string>
#include<vector>
#include<map>


template< typename T>
void saturate(T& value, const uint8_t& min_value, const uint8_t& max_value) {
	
	value = value < min_value ? min_value : value > max_value ? max_value : value; // satura gli elementi che sono piu' bassi della soglia e quelli che sono piu' alti della soglia 
}


void compute_upsampling(mat<vec3b>& frame, std::vector<mat<uint8_t>>& YCbCr) {
	using namespace std;
	// e' necessario saturare i valori nei range giusti

	using i_vec3b = array<int, 3>; // creiamo la possibilita' di avere un array di int di 3 elem

	i_vec3b pix, pp_pix; // in pix avremo i valori salutari e divisi per 16 e 128
	// in pp_pix ci sarenno i risultati del prodotto riga per colonna 

	for (int r = 0; r < frame.rows(); r++) {
		for (int c = 0; c < frame.cols(); c++) {
			pix[0] = YCbCr[0](r, c);
			pix[1] = YCbCr[1](r/2, c/2); // prendiamo la posizione dimezzata 
			pix[2] = YCbCr[2](r / 2, c / 2);

			saturate(pix[0], 16, 235);
			saturate(pix[1], 16, 240);
			saturate(pix[2], 16, 240);

			pix[0] -= 16;
			pix[1] -= 128;
			pix[2] -= 128;

			pp_pix[0] = (1164 * pix[0] + 1596 * pix[2]) / 1000;
			pp_pix[1] = (1164 * pix[0] - 392 * pix[1] - 813 * pix[2]) / 1000;
			pp_pix[2] = (1164 * pix[0] + 2017* pix[1]) / 1000;

			for (auto& e : pp_pix)
				saturate(e, 0, 255);


			vec3b rgb;
			for (int i = 0; i < 3; i++) {
				rgb[i] = static_cast<uint8_t>(pp_pix[i]);
			}
			frame(r, c) = { rgb[0],rgb[1], rgb[2] }; // salviamo i valori nel frame 
		}
	}
}


bool Test_Magic_Number(std::ifstream& is, const std::string& magic) {

	std::string word;
	is >> word;
	if (word != magic) {
		return false;
	}
	return true;
}

bool Read_Header(std::ifstream& in, std::map<uint8_t, std::string>& header) {
	using namespace std;

	string magic_number = "YUV4MPEG2";

	if (!Test_Magic_Number(in, magic_number)) {
		cout << "error in passed the file" << endl;
		return false;
	}

	string v;
	const char tags[] = { 'W','H','C','I','F','A','X' };

	// incominciamo a leggere header
	while (true) {
		bool flag = false;
		char c = in.get();

		if (c == '\n') { // fine dell'header
			break;
		}
		if (c == ' ') {
			flag = true;
		}

		if (flag) {
			// se abbiamo trovato uno spazio
			c = in.get();
			for (auto e : tags) {
				if (c == e) {
					in >> v;
					header[e] = v;
					break;
				}
			}
		}
	}

	int H = stoi(header['H']);
	int W = stoi(header['W']);

	if (W < 0 || H < 0 || header['C'] != "420jpeg")
		return false;

	return true;
}

bool Read_Frames(std::ifstream& is, std::map<uint8_t, std::string>& header, std::vector<mat<vec3b>>& frames) {
	using namespace std;
	// leggiamo FRAME
	/* Ogni nuovo frame  terra':
		1. frame_magic_number: FRAME
		2. Tags I , X
		3. 288*352 elem del Y
		4. 144*176 elem di Cb e Cr
	*/

	string frame_number;
	string interlacing, application;

	int H = stoi(header['H']);
	int W = stoi(header['W']);

	mat<uint8_t> Y(H, W);
	mat<uint8_t> Cb(H / 2, W / 2);
	mat<uint8_t> Cr(H / 2, W / 2);

	while (true)
	{
	
		if (is.fail())
			return false;
		
		if (is.eof())
			break;


		is >> frame_number;

		if (frame_number != "FRAME")
			return false;

		// leggiamo l'header
		while (true) {
			bool flag = false;
			char c = is.get();

			if (c == '\n') { // fine dell'header
				break;
			}
			if (c == ' ') {
				flag = true;
			}

			if (flag) {
				char tag = is.get();
				switch (tag)
				{case 'I':
					is >> interlacing;
					break;
				case 'X':
					is >> application;
					break;
				default:
					return false;
				}
			}
		}

		// leggere i dati

		is.read(Y.rawdata(), Y.rawsize());
		if (is.fail())
			return false;
		is.read(Cb.rawdata(), Cb.rawsize());
		if (is.fail())
			return false;
		is.read(Cr.rawdata(), Cr.rawsize());
		if (is.fail())
			return false;

		vector<mat<uint8_t>> YCbCr;
		YCbCr.push_back(Y);
		YCbCr.push_back(Cb);
		YCbCr.push_back(Cr);

		mat<vec3b> rgb_frame(H, W);

		compute_upsampling(rgb_frame, YCbCr);


		cout << "pushing frame" << endl;

		frames.push_back(rgb_frame);
	}
	return true;
}

bool y4m_extract_color(const std::string& filename, std::vector<mat<vec3b>>& frames) {
	using namespace std;
	ifstream in(filename, ios::binary);
	if (!in) {
		return false;
	}
	map<uint8_t, string> header;
	
	if (!Read_Header(in, header)) {
		return false;
	}
	
	if (!Read_Frames(in, header, frames)) {
		return false;
	}
	
	return true;
}



int main(void) {
	using namespace std;

	vector<mat<vec3b>> frames;
	if (!y4m_extract_color("test1.y4m", frames)) {
		return -1;
	}
	for (size_t i = 0; i < frames.size(); ++i) {
		stringstream ss;
		ss << setfill('0');

		ss << "color" << setw(3) << i << ".ppm";
		save_ppm(ss.str(), frames[i]);
	}
	return true;
}