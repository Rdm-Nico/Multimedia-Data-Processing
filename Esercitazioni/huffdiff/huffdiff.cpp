#include<fstream>
#include<iostream>
#include<cmath>
#include<vector>
#include<algorithm>
#include<functional>
#include<map>
#include<iomanip>
#include<cassert>
#include<string>
#include<sstream>
using namespace std;




template< typename T>
class mat {
	int rows_;
	int cols_;
	vector<T> data_;
public:
	mat(int rows = 0, int cols = 0): rows_(rows), cols_(cols), data_(cols*rows){}

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

	int rows() { return rows_; }
	int cols() { return cols_; }
	int size() { return rows_*cols_; }

	size_t raw_size() {
		return (rows_ * cols_ * sizeof(T));
	}
	char* raw_data() {
		return reinterpret_cast<char*>(&data_[0]);
	}
	const char* raw_data() const {
		return reinterpret_cast< const char*>(&data_[0]);
	}

	auto begin() { return data_.begin(); }
	auto end() { return data_.end(); }
	auto begin() const  { return data_.begin(); }
	auto end() const  { return data_.end(); }
};

template<typename T>
istream& raw_read(istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

class bitreader {
	uint8_t buffer_;
	int n_ = 0;
	istream& is_;

public:
	bitreader(istream& is): is_(is) {}

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


bool load_pam(const string& filename, mat<uint8_t>& img) {

	ifstream is(filename, ios::binary);
	if (!is)
		return false;
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
		else if (token == "HEIGHT") {
			ss >> h;
		}
		else if (token == "DEPTH") {
			int depth;
			ss >> depth;
			if (depth != 1)
				return false;
		}
		else if (token == "MAXVAL") {
			int maxval;
			ss >> maxval;
			if (maxval != 255)
				return false;
		}
		else if (token == "TUPLTYPE") {
			string tupletype;
			ss >> tupletype;
			if (tupletype != "GRAYSCALE")
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
			img(r, c) = is.get();
		}
	}

	return true;
}

bool save_pam(const string& filename,const  mat<uint8_t>& img) {

	ofstream os(filename, ios::binary);
	if (!os) {
		return false;
	}
	os << "P7\n";
	os << "WIDTH " << img.cols() << "\n";
	os << "HEIGHT " << img.rows() << "\n";
	os << "DEPTH 1\n";
	os << "MAXVAL 255\n";
	os << "TUPLTYPE GRAYSCALE\n";
	os << "ENDHDR\n";

	os.write(img.raw_data(), img.raw_size());

}



mat<int> calc_diffm(mat<uint8_t>& m) {
	int prev = 0;

	mat<int> diff;
	diff.resize(m.rows(), m.cols());

	for (int r = 0; r <= m.rows(); ++r) {
		for (int c = 0; c <= m.cols(); ++c) {
			diff(r, c) = m(r, c) - prev;
			prev = m(r, c);
		}
		prev = m(r, 0);
	}
	return diff;
}

mat<uint8_t> calc_viewable_diffm(mat<int>& diffm) {
	mat<uint8_t> v(diffm.rows(), diffm.cols());

	for (size_t r = 0; r < diffm.rows(); r++) {
		for (size_t c = 0; c < diffm.cols(); c++) {
			int rc = diffm(r, c);

			if (rc == 0) {
				v(r, c) = 128;
			}
			if (rc < -127) {
				v(r, c) = 0;
				continue;
			}
			if (rc > 127) {
				v(r, c) = 255;
				continue;
			}

			rc += 128;
			v(r, c) = static_cast<uint8_t>(rc);
		}
	}
	return v;
}

bool compressed(const string& file_in, const string& file_out){

	mat<uint8_t> img;

	if (!load_pam(file_in, img)) {
		cout << "error in load the pam file" << endl;
		return false;
	}
	mat<int> mdiff = calc_diffm(img);

	mat<uint8_t> viewable_mdiff = calc_viewable_diffm(mdiff);
	if (!save_pam("diff.pam", viewable_mdiff)) {
		cout << "error in saving the pam file..." << endl;
		return false;
	}


	/*
	huffdiff <uint16_t,uint16_t>hf(img);

	hf.compute_diff_mat();
	hf.compute_freq();
	hf.create_table();
	hf.compute_canonical_codes();

	*/
	bitreader br(is);


	return true;
}

bool decompressed(const string& file_in, const string& file_out) {

	return true;
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		cout << "error in passing the arguments... Usage:huffdiff [c|d] <input file> <output file>" << endl;
		return 1;
	}

	char mod = *argv[1];

	bool ris = mod == 'c' ? compressed(argv[2],argv[3]) : mod == 'd' ? decompressed(argv[2],argv[3]) : false;
	
	if (!ris) {
		return 1;
	}

	return 0;
}