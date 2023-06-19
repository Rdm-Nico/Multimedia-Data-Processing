#include<iterator>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>

using namespace std;


vector<int32_t> mapped(vector<int32_t>& v) {
	for (auto& x:v) {
		if (x < 0) {
			x = abs(x) * 2;
		}
		else {
			x = (x * 2) + 1;
		}

	}
	return v;
}

vector<int32_t> revert_map(vector<uint32_t>& v) {
	vector<int32_t> ris;
	for (auto& x : v) {
		int32_t e = 0;
		if (x % 2 == 0) {
			e = -((int)x / 2);
		}
		else {
			e = (x - 1) / 2;
		}
		ris.push_back(e);
	}
	return ris;
}

template<typename T>

ostream& raw_write(ostream& os, const T& value, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char *>(&value), size);
}


class bitwriter {
	uint8_t buffer_;
	int n_ = 0;
	ostream& os_;

	ostream& write_bit(uint32_t bit) {
		buffer_ = (buffer_ * 2) + bit;
		++n_;
		if (n_ == 8) {
			raw_write(os_, buffer_);
			n_ = 0;
		}
		return os_;
	}

	public:
		bitwriter(ostream& os): os_(os){}

		~bitwriter() {
			flush();
		}

		ostream& flush(uint32_t bit = 0) {
			while (n_ > 0) {
				write_bit(bit);
			}
			return os_;
		}

		ostream& write(uint32_t num, uint8_t n_times) {
			for (int i = n_times - 1; i >= 0; --i) {
				uint8_t cure_bit = (num >> i) & 1;
				write_bit(cure_bit);
			}
			return os_;
		}

		ostream& operator()(uint32_t num, uint8_t n_times) {
			return write(num, n_times);
		}
};

template<typename T>
istream& raw_read(istream& is, T& value, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&value), size);
}

class bitreader {
	uint8_t buffer_;
	int n_ = 0;
	istream& is_;

	public:
		bitreader(istream& is) : is_(is) {}

		uint32_t read_bit() {
			if (n_ == 0) {
				raw_read(is_, buffer_);
				n_ = 8;
			}
			--n_;
			return(buffer_ >> n_) & 1;
		}

		uint32_t read(uint8_t num) {
			uint32_t u = 0;
			while (n_--> 0) {
				u = (u << 1) | read_bit();
			}
			return u;
		}

		uint32_t operator()(uint8_t num) {
			return read(num);
		}

		bool fail() const {
			return is_.fail();
		}

		explicit operator bool() const {
			return !fail();
		}

};

int count_bits(int32_t num) {
	int count = 0;
	while (num > 0) {
		++count;
		num /= 2; // num >>=1
	}
	return count;
}
void toElias(bitwriter& rw, vector<int32_t>& v) {
	for ( auto& x : v) {
		auto n_bits = count_bits(x);
		rw(x, 2 * n_bits - 1);
	}
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		return 1;
	}
	string symbol{ argv[1] };

	if (symbol.length() != 1)
	{
		cout << "second parameter not singular caracther\n";
		return 1;
	}

	if (symbol == "c") {
		// compressione
		ifstream is(argv[2]);
		if (!is)
			return -1;

		vector<int32_t> v{
			istream_iterator<int32_t>(is),
			istream_iterator<int32_t>()
		};


		mapped(v);

		ofstream out(argv[3],ios::binary);
		if (!out) {
			return -1;
		}

		bitwriter br(out);

		toElias(br, v);
		
	}
	else if(symbol == "d")
	{
		// decompressione
		ifstream is(argv[2],ios::binary);
		ofstream out(argv[3]);
		if (!is || !out)
			return -1;

		bitreader br(is);

		vector<uint32_t> v_to_map;

		while (1) {
			uint32_t zeros_read = 0; // lettura degli zeri
			uint32_t sx_value = 0;
			uint8_t r = 0;

			if (!br)
				break;

			while (r != 1) {
				r = br.read_bit();
				if (r == 0) {
					zeros_read++; // conto gli zeri
				}
			}
			sx_value = pow(2, zeros_read);

			uint32_t dx_value = br.read(zeros_read);
			uint32_t fin_value = sx_value + dx_value;
			
			cout << fin_value;

			v_to_map.push_back(fin_value);
		}

		vector<int32_t> rev_v = revert_map(v_to_map);

		copy(rev_v.begin(), rev_v.end(), ostream_iterator<int32_t>(out, "\n"));
	}
	else
	{
		cout << "not correct char insert. choose c or d\n";
		return 1;
	}

	return 0;
}