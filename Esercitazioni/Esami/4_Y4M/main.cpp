#include<fstream>
#include<iostream>
#include<algorithm>
#include<string>
#include<sstream>

#include "mat.h"
#include "types.h"
#include "utils.h"

using namespace std;

void error(string str) {
	cout << str;
}

bool y4m_extract_gray(const string& filename, vector<mat<uint8_t>>& frames) {

	ifstream is(filename, ios::binary);
	if (!is) {
		error("error in opening the file\n");
		return false;
	}

	string line;
	getline(is, line); // legge fino all'a capo '\x0A'

	stringstream ss(line);

	string magic_number;
	ss >> magic_number;
	if (magic_number != "YUV4MPEG2"){
		error("error in magic_number\n");
		return false;
	}

	int width;
	int height;
	string chroma;

	while (ss)
	{
		
		char token;
		ss >> token;

		switch (token)
		{
		case 'W': {
			
			ss >> width;
			break;
		}
		case 'H': {
			ss >> height;

			break;
		}
		case 'C': {
			
			ss >> chroma;
			if (chroma != "420jpeg"){
				error("error in chromasub\n");
				return false;
			}
			break;

		}
		default:
			break;
		}

	}


	while (true) {

		if (!is || is.eof())
			break;



		string frame;

		is >> frame;
		if (frame != "FRAME")
			break;

		// frame header
		while (true) {
			char c;
			is.get(c);
			if (c == '\x0A'){
				break;
			}
				

			switch (c) {
			case 'I': {
				string interlace;
				is >> interlace;
				break;
			}
			case 'X': {
				string application;
				is >> application;
				break;
			}
			default: {
				break;
			}
			}

		}

		mat<uint8_t> frame_i(height,width);

		// abbiamo tutti i valori di Y -->  288*352 = 101.376 
		// avremo poi Cb e Cr rispettivamente ognuno 25344 

		is.read(frame_i.rawdata(), frame_i.rawsize());

		frames.push_back(frame_i);

		// e poi avremo i frame di Cb e Cr 
		mat<uint8_t> Cb(height / 2, width / 2);
		mat<uint8_t> Cr(height / 2, width / 2);

		is.read(Cb.rawdata(), Cb.rawsize());
		is.read(Cr.rawdata(), Cr.rawsize());
	}
	


	


	
	

	
	

	return true;
}


template<typename T>
void saturate(mat<T>& vector, const int min, const int max) {
	for (auto& e : vector) {
		e = e < min ? min : e > max ? max : e;
	}
}

bool y4m_extract_color(const std::string& filename, std::vector<mat<vec3b>>& frames) {

	ifstream is(filename, ios::binary);
	if (!is) {
		error("error in opening the file\n");
		return false;
	}

	string line;
	getline(is, line); // legge fino all'a capo '\x0A'

	stringstream ss(line);

	string magic_number;
	ss >> magic_number;
	if (magic_number != "YUV4MPEG2") {
		error("error in magic_number\n");
		return false;
	}

	int width;
	int height;
	string chroma;

	while (ss)
	{

		char token;
		ss >> token;

		switch (token)
		{
		case 'W': {

			ss >> width;
			break;
		}
		case 'H': {
			ss >> height;

			break;
		}
		case 'C': {

			ss >> chroma;
			if (chroma != "420jpeg") {
				error("error in chromasub\n");
				return false;
			}
			break;

		}
		default:
			break;
		}

	}


	while (true) {

		if (!is || is.eof())
			break;



		string framestr;

		is >> framestr;
		if (framestr != "FRAME")
			break;

		// frame header
		while (true) {
			char c;
			is.get(c);
			if (c == '\x0A') {
				break;
			}


			switch (c) {
			case 'I': {
				string interlace;
				is >> interlace;
				break;
			}
			case 'X': {
				string application;
				is >> application;
				break;
			}
			default: {
				break;
			}
			}

		}

		mat<uint8_t> Y(height, width);

		// abbiamo tutti i valori di Y -->  288*352 = 101.376 
		// avremo poi Cb e Cr rispettivamente ognuno 25344 

		is.read(Y.rawdata(), Y.rawsize());

		

		// e poi avremo i frame di Cb e Cr 
		mat<uint8_t> Cb(height / 2, width / 2);
		mat<uint8_t> Cr(height / 2, width / 2);

		is.read(Cb.rawdata(), Cb.rawsize());
		is.read(Cr.rawdata(), Cr.rawsize());


		// upsampling 
		Cb.resize(Y.rows(), Y.cols());
		Cr.resize(Y.rows(), Y.cols());


		mat<vec3b> frame(Y.rows(),Y.cols());

		// facciamo adesso la saturzione 
		saturate(Y, 16, 235);
		saturate(Cb, 16, 240);
		saturate(Cr, 16, 240);


		// e poi si realizza la formula richiesta 


		// e poi si ri fa una saturazione  
		saturate(frame, 0, 255);

	}
	
	
	return true;
}

int main(void) {


	//vector<mat<uint8_t>> frames;

	//if (!y4m_extract_gray("foreman_cif.y4m", frames))
		//return EXIT_FAILURE;

	vector<mat<vec3b>> frames;

	if (!y4m_extract_color("foreman_cif.y4m", frames)) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

