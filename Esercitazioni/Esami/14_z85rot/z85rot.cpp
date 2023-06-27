#include<iostream>
#include<fstream>
#include<vector>
#include<unordered_map>
#include<deque>
#include<algorithm>
#include<cmath>
#include<cassert>
#include<functional>
#include<iomanip>
#include<array>
#include<cstdint>
#include<string>

using namespace std;



template<typename T>
istream& raw_read(istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

using rgb = array<uint8_t, 3>;

template<typename T>
class mat {
	int rows_ = 0;
	int cols_ = 0;
	vector<T> data_;

public:
	mat() {}
	mat(int rows, int cols) : rows_(rows), cols_(cols), data_(cols* rows) {}

	void resize(int r, int c) {
		rows_ = r;
		cols_ = c;
		data_.resize(r * c);
	}

	T& operator()(int r, int c) {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}
	const T& operator()(int r, int c) const  {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	 int rows() const { return rows_; }
	 int cols() const { return cols_; }
	 int size() const { return cols_ * rows_; }

	 size_t  raw_size() const {
		return rows_* cols_ * sizeof(T);
	}
	char* raw_data() {
		return reinterpret_cast<char*>(&data_[0]);
	}

	const char* raw_data() const {
		return reinterpret_cast<const char*>(&data_[0]);
	}

	auto begin() { return data_.begin(); }
	auto end() { return data_.end(); }
	auto begin() const { return data_.begin();}
	auto end() const { return data_.end(); }

};

class Z85 {
	unordered_map<uint8_t, uint8_t> table_;
	mat<rgb>& input_;
	vector<uint8_t> codes_;
	uint32_t buffer_;
	int N_rotation_ = 0;
	bool needsPadding_;
	uint8_t paddingNumber_;
	vector<uint32_t> raw_bins_;

	void create_table() {
		// create the table 
		unordered_map<uint8_t, uint8_t> m;

		for (uint8_t i = 0; i <= 9; i++) {
			m[i] = i + '0'; // perche' vogliamo il numero ascii
		}

		for (uint8_t i = 10; i <= 35; i++) {
			m[i] = 87 + i;
		}

		for (uint8_t i = 36; i <= 61; i++) {
			m[i] = 29 + i;
		}

		vector<uint8_t> other = { '.','-',':','+','=','^','!','/','*','?','&','<','>','(',')','[',']','{','}','@','%','$','#'};
		
		int count = 0;
		for (uint8_t i = 62; i <= 84; i++) {
			m[i] = other[count];
			count++;
		 }
		
		for (auto e : m) {
			table_[e.first] = e.second;
		}
	}

public:
	Z85(mat<rgb>& input,int rotations) : input_(input), N_rotation_(rotations), codes_(0), buffer_(0) {
		
		create_table();

		needsPadding_ = input_.size() % 4 != 0 ? true : false;
		
	}

	void encode() {
		uint32_t dec = 0;
		deque<uint8_t> v;

		for (size_t r = 0; r < input_.rows(); r++) {
			for (size_t c = 0; c < input_.cols(); c++) {

				auto num = input_(r, c)[0];
				v.push_back(input_(r, c)[0]);
				v.push_back(input_(r, c)[1]);
				v.push_back(input_(r, c)[2]);

				if (v.size() >= 4) {
					uint32_t x = 0;

					// byte piu' significativo
					dec |= v[0];

					dec <<= 24;

					// 2 byte
					x |= v[1];
					x <<= 16;
					dec += x;
					x = 0;

					// 3 byte
					x |= v[2];
					x <<= 8;
					dec += x;

					// 4 byte
					dec += v[3];

					raw_bins_.push_back(dec);
					dec = 0;

					v.clear();

				}
			}
		}
		
	}

};


void error(string msg) {
	cout << msg;
	exit(EXIT_FAILURE);
}


static uint8_t get_byte(istream& is) {
	int val;
	is >> val;
	if (0 <= val && val <= 255)
		return val;
	else
	{
		error("wrong pixel value\n");
	}
}

bool load_ppm(mat<rgb>& img, const string& filename_in) {

	ifstream is(filename_in, ios::binary);
	if (!is) {
		error("Error in opening the file\n");
	}

	string magic_number;

	getline(is, magic_number);

	if (magic_number != "P6")
		error("error in loading\n");

	string width_s;
	char v;
	while (true) {
		is.get(v);
		if (v == ' ')
			break;
		width_s.push_back(v);
	}

	int w = stoi(width_s);

	string height_s;
	while (true) {
		is.get(v);
		if ( v == '\n')
			break;
		height_s.push_back(v);
	}

	int h = stoi(height_s);

	if (w < 0 || h < 0)
		error("error in loading the file\n");

	string maxval_s;
	getline(is, maxval_s);

	int maxval = stoi(maxval_s);


	img.resize(h,w);

	/*
	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c) {
			img(r, c)[0] = get_byte(is);
			img(r, c)[1] = get_byte(is);
			img(r, c)[2] = get_byte(is);
		}
	}
	*/
	is.read(img.raw_data(), img.raw_size());
	
	return true;
}

void encode(const int& N, const string& filename_in, const string& filename_out) {

	// facciamo il load ppm
	mat<rgb> img;

	if (!load_ppm(img, filename_in))
		error("error in loading the file\n");

	Z85 z85_base(img, 1);

	z85_base.encode();



}

void decode(const int& N, const string& filename_in, const string& filename_out) {

}

int main(int argc, char* argv[]) {
	if (argc != 5) {
		cout << "error syntax... Usage:Z85rot {c | d} <N> <input file> <output file>" << endl;
		return EXIT_FAILURE;
	}

	char mod = *argv[1];

	int N = atoi(argv[2]);
	
	if (N < 0)
		error("error in pass the rotate number N\n");

	switch (mod)
	{
	case 'c': {

		encode(N, argv[3], argv[4]);

		break;
	}
	case 'd': {
		decode(N, argv[3], argv[4]);
		break;
	}
	default:
		error("error in passing the mod chararcher: only [c|d] allowed\n");
		break;
	}

	return EXIT_SUCCESS;
}