#pragma once

#include<fstream>
#include<iomanip>
#include<iostream>
#include<iterator>
#include<algorithm>
#include<string>
#include<vector>

using namespace std;

//bool BinaryImage::ReadFromPBMC(const string& filename);



template< typename T>
istream& raw_read(istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

struct bitreader {
	uint8_t buffer_;
	int n_ = 0;
	std::istream& is_;



	bitreader(std::istream& is) : is_(is) {}

	uint32_t read_bit() {
		if (n_ == 0) {
			raw_read(is_, buffer_);
			n_ = 8;
		}
		n_--;
		return(buffer_ >> n_) & 1; // ritorniamo il bit interessato
	}

	uint32_t read(uint8_t n_bit) { // fa la lettura di n bit e ritorna il valore 
		uint32_t u = 0;
		while (n_bit-- > 0)
		{
			u = (u << 1) | read_bit();
		}
		return u;
	}

	uint32_t operator()(uint8_t n_bit) {
		return read(n_bit);
	}

	bool fail() const { // deve essere const perche non modifica lo stato dell'oggetto
		return is_.fail();
	}

	explicit operator bool() const {
		return !fail();
	}
};


struct BinaryImage {
	int W;
	int H;
	vector<uint8_t> ImageData;


	BinaryImage(int height = 0, int width = 0): H(height), W(width), ImageData(height*width){}

	bool ReadFromPBM(const string& filename) {

		ifstream is(filename, ios::binary);
		if (!is) {
			return false;
		}

		bitreader br(is);
		if (!br) {
			return false;
		}

		string magic_number;
		getline(is,magic_number);

		if (magic_number != "P4") {
			return false;
		}

		if (is.peek() == '#') {
			is.get();
			string comment_section;
			getline(is, comment_section);
		}

		char carattere = 0;
		string width;
		while (1)
		{
			is.get(carattere);
			if (carattere == ' ')
				break;

			width.push_back(carattere);
		}

		W = stoi(width);

		int n_bytes = 0;

		bool flag = false;
		if (W % 8 != 0) {
			 n_bytes = W / 8 +1;
			flag = true;
		}
		else
		{
			n_bytes = W / 8;
		}

		

		
		string height;
		while (1)
		{
			is.get(carattere);
			if (carattere == '\n')
				break;

			height.push_back(carattere);
		}

		H = stoi(height);



		ImageData.resize(H * n_bytes);

		size_t c;
		size_t r;

		for (r = 0; r < H; r++) {
			// avremo in ogni riga W bits impacchetati in un byte 
			for (c = 0; c < n_bytes; c++) {

				//raw_read(is, ImageData[r * W + c]);
				ImageData[r * W + c] = br.read(8);

			}

			while (br.n_ > 0)
			{
				ImageData[r * W + c] = (ImageData[r * W + c] << 1) | 0;
			}
			
		}


		return true;
	}
};




