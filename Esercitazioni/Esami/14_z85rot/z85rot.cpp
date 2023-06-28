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
#include<stdlib.h>

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
	mat<rgb> input_;
	vector<uint8_t> codes_;
	uint32_t buffer_;
	int N_rotation_ = 0;
	bool needsPadding_;
	uint8_t paddingNumber_;
	vector<uint32_t> raw_bins_;
	vector<uint8_t> data_;
	size_t count_, actualRotation_;
	vector<uint8_t> encoded_;
	bool flag_ = false;

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
		
		if(!flag_){
			for (auto e : m) {
				table_[e.first] = e.second;
			}
		}
		else
		{
			for (auto e : m) {
				table_[e.second] = e.first;
			}
		}
		
	}

public:
	Z85(int rotations,bool flag) : N_rotation_(rotations), flag_(flag) {
		create_table();
	};

	Z85(mat<rgb> input, int rotations) : input_(input), N_rotation_(rotations), codes_(0), buffer_(0), actualRotation_(0){

		create_table();

		needsPadding_ = input_.size() % 4 != 0 ? true : false;

	};

	void Tobase85(uint32_t in ) {
		vector<uint8_t> v;

		while (true) {
			if (in == 0 && v.size() == 5)
				break;
			if (v.size() < 5 && in == 0) {
				v.push_back(0);
				continue;
			}
			uint32_t r = in % 85;
			in /= 85;
			v.push_back(r);
		}
		count_ += v.size();
		
		for (auto it = v.rbegin(); it != v.rend(); it++) {
			data_.push_back(*it);
		}
	}

	uint8_t getRotatedIndex(int N, uint8_t e) {
		int index = e - actualRotation_;
		index = index < 0 ? index += table_.size() : index;
		actualRotation_ += N_rotation_;

		return static_cast<uint8_t>(index);
	}

	void encode() {
		uint32_t dec = 0;
		deque<uint8_t> v;



		for (size_t r = 0; r < input_.rows(); r++) {
			for (size_t c = 0; c < input_.cols(); c++) {

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

					for (size_t i = 0; i < 4; i++)
						v.pop_front();

				}
			}
		}

		if (needsPadding_ && v.size() > 0) {
			size_t diff = v.size();
			uint8_t bits = 24;
			uint32_t dec = 0;
			for (auto e : v) {
				dec <<= bits;
				dec |= e;
				bits -= 8;
			}
			while (diff-- > 0) {
				dec <<= bits;
				bits -= 8;
			}

			raw_bins_.push_back(dec);
		}

		// trasformare in base 85
		for (auto e : raw_bins_) {
			Tobase85(e);
		}

		// e poi lo codifichiamo 
		for (auto e : data_) {
			uint8_t index = getRotatedIndex(N_rotation_, e);
			encoded_.push_back(table_.at(index));
		}
		
	}

	uint8_t GetRealIndex(uint8_t e) {
		int index = e + actualRotation_;
		index = index >= 85 ? index -= table_.size() : index;
		actualRotation_ += N_rotation_;

		return static_cast<uint8_t>(index);
	}

	auto encoded_codes() { return encoded_; }
	auto table() { return table_; }

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

	char v;

	if(is.peek() == '#'){
		string comment_section;
	
		while (true) {
			is.get(v);
			if (v == '\n')
				break;
			comment_section.push_back(v);
		}
	}

	string width_s;
	
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
	* if ascii
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

	Z85 z85_base(img, N);

	z85_base.encode();

	// scriviamo il file di output:
	ofstream out(filename_out, ios::binary);

	out << img.cols();
	out << ',';
	out << img.rows();
	out << ',';
	for (auto e : z85_base.encoded_codes()) {
		out << e;
	}

	if (out.fail()) {
		error("error in create the output file\n");
	}
}

void save_ppm(mat<rgb> img, const string& filename) {
	ofstream out(filename, ios::binary);

	if (!out)
		error("error in creating the output file\n");
	out << "P6";
	out << '\x0A';
	out << img.cols();
	out << '\x0A';
	out << img.rows();
	out << '\x0A';
	out << 255;
	out << '\x0A';

	/*
	// 1 alternativa
	for (int r = 0; r < img.rows(); r++) {
		for (int c = 0; c < img.cols(); c++) {
			out.put(img(r, c)[0]);
			out.put(img(r, c)[1]);
			out.put(img(r, c)[2]);
		}
	}
	*/

	// 2 alternativa
	out.write(img.raw_data(), img.raw_size());

}

void decode(const int& N, const string& filename_in, const string& filename_out) {

	ifstream is(filename_in, ios::binary);
	if (!is) {
		error("in opening the file\n");
	}

	string width;
	string height;
	char tmp_c;
	while (true) {
		is.get(tmp_c);
		if (tmp_c == ',')
			break;
		width.push_back(tmp_c);
	}

	int W = stoi(width);

	while (true) {
		is.get(tmp_c);
		if (tmp_c == ',')
			break;
		height.push_back(tmp_c);
	}

	int H = stoi(height);

	mat<rgb> img(H, W);

	Z85 z85_base(N,true);

	deque<uint8_t> bins(0);
	vector<uint32_t> raw_bins(0);

	while (true) {
		if (!is || is.eof())
			break;

		char c;
		is.get(c);
		if (!is)
			break;

		
		uint8_t val = z85_base.table()[c];

		uint8_t real_val = z85_base.GetRealIndex(val);

		bins.push_back(real_val);
	}

	if (bins.size() % 5 != 0) {
		error("this is impossibile\n");
	}

	// avremo i valori in base 85 da tornare a trasformare in base 10, 5 valori alla volta

	uint32_t v = 0;
	while (bins.size() > 0) {
		for (int j = 4; j >= 0; --j) {
			uint8_t num = bins.front();
			v += num * pow(85, j);
			bins.pop_front();
		}
		raw_bins.push_back(v);
		v = 0;
	}

	// da qua poi e' necessario trasformare ogni valori nel suo corrispondendete numero in base 16
	vector<uint8_t> data_img;


	for (auto e : raw_bins) {
		
		uint8_t f = 0, s = 0, t = 0, fou = 0;

		f = (e >> 24);
		s = (e >> 16);
		t = (e >> 8);

		unsigned mask = (1 << 8); 
		/*
			in f = 1111'1111 (255)
			cosi' facendo otteniamo una maschera di 8 bit:  0001'0000'0000 (256)
		*/
		mask -= 1; // maschera a 255 ora

		fou = e & mask;


		data_img.push_back(f);
		data_img.push_back(s);
		data_img.push_back(t);
		data_img.push_back(fou);

		/*
		while (e > 0) {
			uint8_t rest = e % 16;
			e /= 16;

			switch (rest)
			{
			case 10: { rest = 'A'; break; }
			case 11: { rest = 'B'; break; }
			case 12: { rest = 'C'; break; }
			case 13: { rest = 'D'; break; }
			case 14: { rest = 'E'; break; }
			case 15: { rest = 'F'; break; }
			default:
				break;
			}

			data_img.push_back(rest);
		}
		*/
		
	}

	copy(data_img.begin(), data_img.end(), img.raw_data());


	save_ppm(img,filename_out);


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
	/*test per github*/

	return EXIT_SUCCESS;
}