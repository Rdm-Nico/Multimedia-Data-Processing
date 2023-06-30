#include<iostream>
#include<fstream>
#include<cstdint>
#include<vector>
#include<string>
#include<sstream>
#include<algorithm>
#include<deque>
#include<array>
#include<cassert>
#include<format>
#include<map>

using namespace std;

using vec3b = array<uint8_t, 3>;

template<typename T>
struct mat {
	int cols_;
	int rows_;
	vector<T> data_;

public:
	mat(int rows = 0, int cols = 0): rows_(rows), cols_(cols), data_(cols*rows){}

	void resize(int r, int c) {
		assert(r > 0 && c > 0);
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
		return rows_ * cols_ * sizeof(T);
	}
	char* raw_data() {
		return reinterpret_cast<char*>(&data_[0]);
	}
	const char* raw_data() const  {
		return reinterpret_cast<const char*>(&data_[0]);
	}

	auto begin() { return data_.begin(); }
	auto end() { return data_.end(); }
	auto begin() const  { return data_.begin(); }
	auto end() const{ return data_.end(); }
};



bool loadPPM(const string& filename, mat<vec3b>& img) {
	ifstream is(filename, ios::binary);
	if (!is)
		return false;
	
	string magic_number;
	getline(is,magic_number);

	if (magic_number != "P6")
		return false;

	string line;
	getline(is, line);
	
	
	if (line[0] == '#') {
		// comment section
		getline(is, line);

	}

	stringstream ss(line);

	string token;
	ss >> token;
	int W = stoi(token);
	ss >> token;
	int H = stoi(token);

	is >> token;
	int val_max = stoi(token);
	if (val_max != 255)
		return false;

	// leggiamo i valori:
	img.resize(H, W);

	


	is.read(img.raw_data(), img.raw_size());
	
	
	



	return true;
}

class PackBits {
	mat<vec3b>& img_;
	array<vector<uint8_t>,3> encoded_;
	deque<uint8_t> copy_buffer_; // caratteri che devono essere codificati
	deque<uint8_t> indexes_; // coda degli indici 
	array<mat<uint8_t>, 3> img_splits_;
	pair < array<mat<uint8_t>, 3>, array<vector<uint8_t>, 3>> imgs_encoded_;
	vector<map<mat<uint8_t>, vector<uint8_t>>> test;

	bool flag_ = false;

public:
	PackBits(mat<vec3b>& img): img_(img), copy_buffer_(0), indexes_(0){
		for(auto e :test)
			for (auto [k, v] : e) {
				k;
			}
				k.resize(img.rows(), img.cols());
			
	}


	void splitRGB() {
		int i = 0;
		for (auto e: imgs_encoded_.first) {

			for (int r = 0; r < img_.rows(); ++r) {
				for (int c = 0; c < img_.cols(); ++c) {
					e(r, c) = img_(r, c)[i];
				}
			}
			i++;
		}
		
	}


	template<typename container>
	uint8_t get_run_index(uint8_t val_cur, const container& control) {
		uint8_t index = 0;
		container v = control;
		bool flag = false;

		for (auto e : v) {
			if (e == val_cur && index < 128) {
				index++;
				flag = true;
			}
			else {
				if (flag) {
					return index;
				}
			}
		}
		return index;
	}

	void rle_run(const uint8_t& n_times, uint8_t& val) {
		
		uint16_t v = 257;
		v -= n_times;

		encoded_.push_back(static_cast<uint8_t>(v));

		encoded_.push_back(val);
	}

	void Encode() {
		for(auto &[img,encoded]: imgs_encoded_){
		vector<uint8_t> control(0);
		uint8_t index = 0;
		uint8_t val_cur;
		for (int k = 0; k < img.size(); k += index) {
			val_cur = img.data_[k];
			
			
			for (int i = k; i < img.size(); i++) {
				if (i == k) {
					copy_buffer_.push_back(val_cur);
					continue;
				}
				control.push_back(img.data_[i]);
			}

			index = get_run_index(val_cur,control) + 1;

			

			indexes_.push_back(index);

			control.clear();

		}

		while (indexes_.size() > 0) {
			if (indexes_.front() > 1) {

				rle_run(indexes_.front(), copy_buffer_.front());

				indexes_.pop_front();
				copy_buffer_.pop_front();

				continue;
			}
			if (indexes_.front() == 1) {
				uint8_t count;
				count = get_run_index(1, indexes_);

				encoded_.push_back(count);

				for (uint8_t r = 0; r < count; r++) {
					encoded_.push_back(copy_buffer_.front());
					copy_buffer_.pop_front();
				}

				indexes_.erase(indexes_.begin(), indexes_.begin() + count);

			}
		}
		encoded_.push_back(128);
		}
	}

	void print_encoded() {
		for (const auto& e : encoded_) {
			cout << hex;
			
			printf("%02x", e);
			cout << " ";
		}
		cout << endl;
		cout << dec;
	}
};

void splitRGB(const mat<vec3b>& img, mat<uint8_t>& img_r, mat<uint8_t>& img_g, mat<uint8_t>& img_b) {
	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c) {
			img_r(r, c) = img(r, c)[0];
			img_g(r, c) = img(r, c)[1];
			img_b(r, c) = img(r, c)[2];
		}
	}
}

int main(void) {

	mat<vec3b> img;

	if (!loadPPM("test.ppm", img))
		return EXIT_FAILURE;

	// Packbits pb(img)
	// pb.splitRGB()
	// pb.Encode()
	// pb.To_json()

	mat<uint8_t> img_r(img.rows(),img.cols());
	mat<uint8_t> img_g(img.rows(), img.cols());
	mat<uint8_t> img_b(img.rows(), img.cols());

	splitRGB(img, img_r, img_g, img_b);

	PackBits pb_r(img_r);
	PackBits pb_g(img_g);
	PackBits pb_b(img_b);

	pb_r.Encode();
	pb_r.print_encoded();
	pb_g.Encode();
	pb_g.print_encoded();
	pb_b.Encode();
	pb_b.print_encoded();

	return 0;
}