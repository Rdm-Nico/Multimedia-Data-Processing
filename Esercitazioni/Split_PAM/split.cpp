#include<iostream>
#include<fstream>
#include<vector>
#include<cstdint>
#include<sstream>
#include<cassert>
#include<array>


template <typename T>
class mat {
	int rows_;
	int cols_;
	std::vector<T> data_;

public:
	mat(int rows = 0, int cols = 0): rows_(rows), cols_(cols), data_(rows*cols){}

	void resize(int rows, int cols) {
		*this = mat(rows, cols);
	}

	const T& operator()(int r, int c) const {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}
	T& operator()(int r, int c) {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	int rows() const { return rows_; }
	int cols() const { return cols_; }
	int size() const { return rows_*cols_; }

	// dimensione in byte
	size_t raw_size() const {
		return rows_ * cols_ * sizeof(T);
	}

	char* raw_data(){ // ritorna la zona della memoria dove incominciano i dati
		return reinterpret_cast<char*>(&data_[0]);
	}

	const char* raw_data() const { // ritorna la zona della memoria dove incominciano i dati
		return reinterpret_cast< const char*>(&data_[0]);
	}

	auto begin() { return data_.begin(); }
	auto begin() const  { return data_.begin(); }
	auto end() { return data_.end(); }
	auto end() const { return data_.end(); }
};

// per poter gestire le img PAM in formato RGB e' necessario dover creare un array  di 3 elementi specifico
using rgb = std::array<uint8_t, 3>;



bool load_pam(std::ifstream& is, mat<rgb>& img) {
	using namespace std;

	string magic_number;

	getline(is, magic_number);
	if (magic_number != "P7")
		return false;

	int w, h;

	while (true) {
		string line;
		getline(is, line);
		if (line == "ENDHDR")
			break; // siamo arrivati alla fine del header

		stringstream ss(line);
		string token;
		ss >> token;
		if (token == "WIDTH") {
			ss >> w;
		}
		else if (token == "EIGHT") {
			ss >> h;
		}
		else if (token == "DEPTH") {
			int depth;
			ss >> depth;
			if (depth != 3)
				return false;
		}
		else if (token == "MAXVAL") {
			int maxval;
			ss >> maxval;
			if (maxval != 255)
				return false;
		}
		else if (token == "TUPLETYPE") {
			string tupletype;
			ss >> tupletype;
			if (tupletype != "RGB")
				return false;
		}
		else
			return false; // non ci puo' essere un valore diverso da questi
	}

	// finito il parser
	img.resize(h, w);

	// adesso dobbiamo inserire gli elementi dentro alla matrice 
	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c) {
			img(r, c)[0] = is.get();
			img(r, c)[1] = is.get();
			img(r, c)[2] = is.get();

		}
	}

	
}



bool split(std::string filename) {
	using namespace std;


	ifstream is(filename, std::ios::binary);
	if (!is) {
		return EXIT_FAILURE;
	}

	mat<rgb> img;

	if (!load_pam(is, img)) {
		return EXIT_FAILURE;
	}
}


int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "error usage:split <filename.pam> " << std::endl;
		return EXIT_FAILURE;
	}
	std::string filename = argv[1];
	split(filename);

	
	return 0;
}