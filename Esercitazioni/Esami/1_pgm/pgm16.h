#include <fstream>
#include<iostream>
#include<cmath>
#include "mat.h"
#include<string>

using namespace std;

template<typename T>
istream& raw_read(istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

class bitreader {
	uint8_t buffer_;
	int n_ = 0;
	istream& is_;

public:
	bitreader(istream& is): is_(is){}

	uint8_t read_bit() {
		if (n_ == 0) {
			raw_read(is_, buffer_);
			n_ = 8;
		}
		n_--;
		return (buffer_ >> n_) & 1;
	}

	uint32_t read(uint8_t n_bit) {
		uint32_t num = 0;
		while (n_bit-- > 0) {
			num = (num << 1) | read_bit();
		}
		return num;
	}

	bool fail() const {
		return is_.fail();
	}
	explicit operator bool() const {
		return !fail();
	}
};

template< typename T>
class pgm {
	string comment_section_;
	string magic_number_;
	const string& filename_;
	int width_ = 0;
	int height_ = 0;
	uint16_t maxvalue_;
	mat<T> data_;

public:
	pgm(const string& filename): filename_(filename){}

	bool load_pgm() {
		ifstream is(filename_, ios::binary);
		if (!is) {
			cout << "error in the opening of the file..." << endl;
			return false;
		}

		bitreader br(is);

		// incominciamo a leggere
		uint8_t c = 0;
		while (true)
		{
			c = br.read(8);
			if (c == '\n')
				break;
			magic_number_.push_back(c);
		}

		// controlliamo che cia sia la sezione commentata
		if (br.read(8) == '#') {
			while (true) {
				c = br.read(8);
				if (c == '\n')
					break;
				comment_section_.push_back(c);
			}
		}

		// leggiamo la  width
		string width_s;
		while (true) {
			c = br.read(8);
			if (c == ' ')
				break;
			width_s.push_back(c);
		}

		width_ = stoi(width_s);

		// leggiamo la height
		string height_s;
		while (true) {
			c = br.read(8);
			if (c == '\n')
				break;
			height_s.push_back(c);
		}
		height_ = stoi(height_s);

		// leggiamo maxvalue
		string maxvalue_s;
		while (true) {
			c = br.read(8);
			if (c == '\n')
				break;
			maxvalue_s.push_back(c);
		}

		maxvalue_ = stoi(maxvalue_s);
		
		if (maxvalue_ > 65535 || maxvalue_ < 0) {
			return false;
		}


		// ci prepariamo a raccogliere i dati
		data_.resize(height_, width_);

		
		for (size_t r = 0; r < data_.rows(); r++) {
			for (size_t c = 0; c < data_.cols(); c++) {
				data_(r, c) = (uint8_t)br.read(8);
			}
		}

		return true;
	}

	int maxvalue() const { return maxvalue_; }
	int width() const { return width_; }
	int height() const { return height_; }
	
	size_t size_data() const { return width_ * height_; }

	auto begin() { return data_.begin(); }
	auto end() { return data_.end(); }
	auto begin() const { return data_.begin(); }
	auto end() const { return data_.end(); }
};


bool load(const std::string& filename, mat<uint8_t>& img, uint16_t& maxvalue) {

	pgm <uint8_t>p(filename);

	if (!p.load_pgm())
		return false;
	maxvalue = p.maxvalue();
	img.resize(p.height(), p.width());
	
	copy(p.begin(), p.end(), img.begin());
	
	return true;
};