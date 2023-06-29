#include<fstream>
#include<iostream>
#include<vector>
#include<algorithm>
#include<cmath>
#include<cstdint>
#include<iostream>
#include<iomanip>
#include<string>
#include<cassert>
#include<sstream>
#include<array>

using namespace std;


template<typename T>
istream& raw_read(istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

using vec3b = array<uint8_t, 3>;

template<typename T>
struct mat {
	int rows_;
	int cols_;
	vector<T> data_;

public:
	mat(int rows = 0, int cols = 0): rows_(rows), cols_(cols), data_(rows_*cols_){}

	void resize(int r, int c) {
		rows_ = r;
		cols_ = c;
		data_.resize(r * c);
	}

	T& operator()(int r, int c) {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	const T& operator()(int r, int c) const {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	int rows() const { return rows_; }
	int cols() const { return cols_; }
	int size() const { return rows_*cols_; }
	
	size_t raw_size() const {
		return cols_ * rows_ * sizeof(T);
	}

	char* raw_data() {
		return reinterpret_cast<char*>(&data_[0]);
	}
	const char* raw_data() const {
		return reinterpret_cast<const char*>(&data_[0]);
	}

	auto begin() { return data_.begin(); }
	auto end() { return data_.end(); }
	auto begin() const { return data_.begin(); }
	auto end() const { return data_.end(); }

};


void error(const string& s) {
	cout << s;
	exit(EXIT_FAILURE);
}

bool load_pgm(mat<uint16_t>& img, const string& filename) {
	ifstream is(filename, ios::binary);
	if (!is) {
		error("Error in opening the file\n");
	}

	string magic_number;
	getline(is, magic_number);
	if (magic_number != "P5")
		return false;

	string line;
	getline(is, line);

	stringstream ss(line);
	string token;


	ss >> token;
	int w = stoi(token);

	ss >> token;
	int h = stoi(token);

	getline(is, token);

	int max_val = stoi(token);

	if (w < 0 || h < 0 || max_val != 65535)
		return false;



	img.resize(h, w);

	// leggiamo , visto che abbiamo solo elementi con maxal > 256
	for (size_t r = 0; r < img.rows(); ++r) {
		for (size_t c = 0; c < img.cols(); ++c) {
			uint16_t val = 0;
			uint8_t x;

			raw_read(is, x);

			val |= x;
			val <<= 8;
			raw_read(is, x);

			val |= x;

			img(r, c) = val;
		}
	}


	return true;
}


void convert(mat<uint8_t>& img_8, mat<uint16_t>& img_16) {

	for (size_t r = 0; r < img_8.rows(); ++r) {
		for (size_t c = 0; c < img_8.cols(); ++c) {
			uint8_t num = img_16(r, c)/256;
			img_8(r, c) = num;
		}
	}
}

bool save_pgm(mat<uint8_t>& img, const string& filename) {

	ofstream out(filename, ios::binary);
	if (!out)
		error("error in opening the output file\n");

	out << "P5\n";
	out << img.cols() << " ";
	out << img.rows();
	out << "\n";
	out << "255\n";

	// scriviamo i dati:
	for (size_t r = 0; r < img.rows(); ++r) {
		for (size_t c = 0; c < img.cols(); ++c) {
			out.put(img(r, c));
		}
	}

	return true;
}




void ToRGB(mat<vec3b>& dest, mat<uint8_t>& m) {

	bool r_or_b = true;
	for (size_t r = 0; r < dest.rows(); ++r) {
		for (size_t c = 0; c < dest.cols(); ++c) {
			
			// se riga pari --> siamo nel caso red/ green 
			// se riga dispari --> siamo nel caso green/blue
			if (r == 0 || r % 2 == 0) {
				if (r_or_b) {
					dest(r, c)[0] = m(r, c);
					r_or_b = false;
				}
				else
				{
					// è un pixel verde
					dest(r, c)[1] = m(r, c);
					r_or_b = true;
				}
			}
			else
			{
				if (r_or_b) {
					// pixel blu
					dest(r, c)[2] = m(r, c);
					r_or_b = false;
				}
				else
				{
					// è un pixel verde
					dest(r, c)[1] = m(r, c);
					r_or_b = true;
				}
			}
		}
		if (r == 0 || r % 2 == 0)
			r_or_b = false;
		else
			r_or_b = true;
	}
}


bool save_ppm(const string& filename, const mat<vec3b>& img, bool ascii)
{
	std::ofstream os(filename, std::ios::binary);
	if (!os)
		return false;

	os << (ascii ? "P3" : "P6") << "\n";
	os << "# MDP2020\n";
	os << img.cols() << " " << img.rows() << "\n";
	os << "255\n";

	if (ascii) {
		for (int r = 0; r < img.rows(); ++r) {
			for (int c = 0; c < img.cols(); ++c) {
				os << static_cast<int>(img(r, c)[0]) << " "
					<< static_cast<int>(img(r, c)[1]) << " "
					<< static_cast<int>(img(r, c)[2]) << " ";
			}
			os << "\n";
		}
	}
	else {
		os.write(img.raw_data(), img.raw_size());
	}

	return true;
}

int main(int argc, char* argv[]) {
	if (argc != 3)
		error("Error Usage: bayer_decode <input file .PGM> <output prefix>\n");

	mat<uint16_t> img_16;

	
	if (!load_pgm(img_16,argv[1]))
		return EXIT_FAILURE;

	mat<uint8_t> img_8(img_16.rows(),img_16.cols());

	convert(img_8, img_16);

	stringstream ss;
	ss << argv[2];
	ss << "_gray.pgm";

	if (!save_pgm(img_8, ss.str()))
		return EXIT_FAILURE;

	
	
	mat<vec3b> img_rgb(img_8.rows(),img_8.cols());

	ToRGB(img_rgb, img_8);

	stringstream uu;
	uu << argv[2];
	uu << "_bayer.ppm";

	if (!save_ppm(uu.str(), img_rgb,false))
		return EXIT_FAILURE;


	return EXIT_SUCCESS;
}