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
#include "huffman.h"
#include<unordered_map>
using namespace std;




template< typename T>
struct mat {
	int rows_;
	int cols_;
	vector<T> data_;

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

	int rows() const { return rows_; }
	int cols() const  { return cols_; }
	int size() const { return rows_*cols_; }

	size_t raw_size() const {
		return rows_ * cols_ * sizeof(T);
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

template<typename T>
ostream& raw_write(ostream& os,const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast< const char*>(&val), size);
}

class bitwriter {
	uint8_t buffer_;
	int n_ = 0;
	ostream& os_;

	ostream& write_bit(uint8_t bit) {
		
		buffer_ = (buffer_ << 1) | bit;
		n_++;
		if (n_ == 8) {
			raw_write(os_, buffer_);
			n_ = 0;
		}
		return os_;
	}
public:
	bitwriter(ostream& os): os_(os){}

	ostream& write(uint32_t num, uint8_t n_bit) {
		for (size_t i = n_bit - 1; i >= 0; --i) {
			uint8_t cure_bit = (num >> i) & 1;
			write_bit(cure_bit);
		}
		return os_;
	}
	ostream& operator()(uint32_t num, uint8_t n_bit) {
		return write(num, n_bit);
	}
	~bitwriter() {
		flush();
	}
	ostream& flush() {
		while (n_ > 0)
		{
			write_bit(0);
		}
		return os_;
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

	return true;
}



mat<int> calc_diffm(mat<uint8_t>& m) {

	mat<int> diff;
	diff.resize(m.rows(), m.cols());

	int prev = 0;

	for (int r = 0; r < m.rows(); ++r) {
		for (int c = 0; c < m.cols(); ++c) {
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
				continue;
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



bool generate_huffdiff(mat<int> mdiff, const string out_filename, huffman<int>& h) {
	ofstream os(out_filename, ios::binary);
	if (!os)
		return false;

	bitwriter bw(os);

	string magi_number = "HUFFDIFF";
	os << magi_number;

	uint32_t c = mdiff.cols();
	uint32_t r = mdiff.rows();

	// cosi siamo  sicuri che stampa di fila tutti i valori senza aggiungere whitespaces
	raw_write(os, c);
	raw_write(os, r);

	// scrivo i numeri di elem della Table entries
	bw.write(h.codes_table_.size(), 9);

	// scriviamo la table entries
	for (const auto& e : h.codes_table_) {
		bw(e.sym_, 9);
		bw(e.len_, 5);
	}

	// codifichiamo seguendo la tabella 
	for (size_t r = 0; r < mdiff.rows(); r++) {
		for (size_t c = 0; c < mdiff.cols(); c++) {
			uint32_t val = 0, len = 0;
			bool flag = false;

			for (const auto& e : h.codes_table_) {
				if (e.sym_ == mdiff(r, c)) {
					val = e.val_;
					len = e.len_;
					flag = true;
					break;
				}
			}

			if (flag) { // se abbiamo trovato un elemento
				bw.write(val, len); // scriviamo il valore del simbolo  con la sua lunghezza
			}
			else {
				cout << "error " << " " << mdiff(r, c) << endl;
			}
		}
	}
	return true;

}


bool compressed(const string& file_in, const string& file_out){

	mat<uint8_t> m;

	load_pam(file_in, m);
	/*
	if (!load_pam(file_in, m)) {
		cout << "error in load the pam file" << endl;
		return false;
	}
	*/
	mat<int> mdiff = calc_diffm(m);

	/*
	mat<uint8_t> viewable_mdiff = calc_viewable_diffm(mdiff);
	if (!save_pam("diff.pam", viewable_mdiff)) {
		cout << "error in saving the pam file..." << endl;
		return false;
	}
	*/

	huffman<int> h;

	unordered_map<int, uint32_t> freq;

	for (size_t r = 0; r < mdiff.rows(); r++) {
		for (size_t c = 0; c < mdiff.cols(); c++) {
			int f = mdiff(r, c);
			freq[f]++;
		}
	}

	h.compute_codes_table(freq);
	h.compute_canonical_codes();

	generate_huffdiff(mdiff, file_out, h);
	

	return true;
}

void error(string) {
	return;
}
bool decompressed(const string& file_in, const string& file_out) {

	ifstream is(file_in, ios::binary);
	if (!is) {
		return false;
	}

	string magic_number(8,' ');
	raw_read(is, magic_number[0], 8);
	if (magic_number != "HUFFDIFF") {
		error("wrong input format\n");
	}
	 
	int width, height;
	raw_read(is, width);
	raw_read(is, height);

	bitreader br(is);
	uint32_t tmp;

	tmp = br.read(9);
	size_t TableEntries = tmp;

	huffman<int> h;

	for (size_t i = 0; i < TableEntries; ++i) {
		huffman<int>::code t; // creiamo un elemento code
		t.sym_ = br.read(9);
		t.len_ = br.read(5);
		h.codes_table_.push_back(t);
	}

	h.compute_canonical_codes();

	mat<int> diff(height, width);

	for (int r = 0; r < diff.rows(); ++r) {
		for (int c = 0; c < diff.cols(); ++c) {
			uint32_t len = 0, code = 0;
			size_t pos = 0;
			do {
				while (h.codes_table_[pos].len_ > len) {
					uint32_t bit;
					bit = br.read_bit();
					code = (code << 1) | bit; 
					++len;
				}
				if (code == h.codes_table_[pos].val_) {
					break;
				}
				++pos;
			} while (pos < h.codes_table_.size());
			if (pos == h.codes_table_.size()) {
				error("questo non dovrebbe accadere\n");
			}
			diff(r, c) = h.codes_table_[pos].sym_;
		}
	}

	mat<uint8_t> img(diff.rows(), diff.cols());

	int prev = 0;
	for (int r = 0; r < diff.rows(); ++r) {
		for (int c = 0; c < diff.cols(); ++c) {
			img(r, c) = diff(r, c) + prev;
			prev = img(r, c);
		}
		prev = img(r, 0);
	}
	save_pam(file_out, img);
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