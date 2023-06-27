#include<vector>
#include<iterator>
#include<fstream>
#include<iostream>
#include<deque>


using namespace std;

template< typename T>
istream& raw_read(istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

template<typename T>
ostream& raw_write(ostream& os, const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
}

class bitreader {
	uint8_t buffer_;
	uint8_t n_ = 0;
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

class bitwriter {
	uint8_t buffer_;
	uint8_t n_ = 0;
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
	bitwriter(ostream& os) : os_(os) {}

	~bitwriter() {
		flush();
	}

	ostream& flush(uint8_t bit = 0) {
		while (n_ > 0) {
			write_bit(bit);
		}
		return os_;
	}

	ostream& write(uint32_t num, uint8_t n_bit) {
		for (int i = n_bit - 1; i >= 0; --i) {
			uint8_t cure_bit = (num >> i) & 1;
			write_bit(cure_bit);
		}
		return os_;
	}

	ostream& operator()(uint32_t num, uint8_t n_bit) {
		return write(num, n_bit);
	}
};

void syntax(){
	cout << "Usage: packbits [c|d] <input file> <output file>" << endl;
	exit(EXIT_FAILURE);
}


template<typename container>
uint8_t get_run_index(const uint8_t& c, const container& control) {
	container v = control;

	uint8_t index = 0;
	bool flag = false;

	for (auto& e : v) {
		if (e == c && index < 128) {
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

void rle_run(const uint8_t& c, uint8_t& n, bitwriter& bw) {
	uint16_t v = 257;
	v -= n;

	bw(v, 8);
	bw(c, 8);
}

bool encode(const string& filename_in, const string& filename_out) {

	ifstream is(filename_in, ios::binary);
	ofstream out(filename_out, ios::binary);

	bitwriter bw(out);
	bitreader br(is);

	vector<uint8_t> control(0);

	char e = 0, curr = 0;
	char past = 0;


	/*
	while (is.get(e))
	{
		lung++;
	}
	is.clear();
	is.seekg(0);

	*/
	// varainte 
	is.seekg(0, ios_base::end);
	size_t lung = static_cast<size_t>(is.tellg());

	is.clear();
	is.seekg(0,ios_base::beg);

	bool flag = false;

	deque<uint8_t> copy_buffer;
	
	deque<uint8_t> index_q; 

	uint8_t index = 0;

	for (size_t k = 0; k < lung; k += index) {
		is.get(past);
		if (is.fail() || is.eof())
			break;

		if (!flag) {
			is.seekg(0);
		}
		flag = true;
		index = 0;

		streamoff pos = is.tellg();

		char c = 0;
		uint8_t first_c = '\0';
		for (size_t i = 0; i < 128; i++) {
			is.get(c);

			if (is.fail() || is.eof())
				break;
			if (i == 0) {
				first_c = c;
				copy_buffer.push_back(first_c);
			}
			control.push_back(c);
		}

		index = get_run_index(first_c, control);

		if (index) {
			if (index == 2) {
				// non dobbiamo fare la run
				index_q.push_back(1);
				copy_buffer.push_back(first_c);
				index_q.push_back(1);
			}
			else
			{
				index_q.push_back(index);
			}
		}

		if (control.size() > 0) {
			is.clear();
		}

		control.clear();

		is.seekg(pos + (index - 1));
	}

	while (index_q.size() > 0) {
		if (index_q.front() > 1) {
			rle_run(copy_buffer.front(), index_q.front(), bw);

			index_q.pop_front();
			copy_buffer.pop_front();

			continue;

		}

		if (index_q.front() == 1) {
			uint8_t cont = get_run_index(1, index_q);

			bw(cont - 1, 8);

			for (uint8_t r = 0; r < cont;  r++) {
				bw(copy_buffer.front(), 8);
				copy_buffer.pop_front();
			}

			index_q.erase(index_q.begin(), index_q.begin()+ cont);
		}
	}

	bw(128, 8);

	return true;
}

bool decode(const string& filename_in, const string& filename_out) {
	ifstream is(filename_in, ios::binary);
	ofstream out(filename_out, ios::binary);

	if (!is || !out) {
		return false;
	}

	bitreader br(is);

	while (br) {
		uint32_t L = br.read(8);

		if (L == 128) {
			break;
		}
		else if (L > 128 && L <= 255) {
			uint32_t cont =  257 - L;

			uint8_t c = br.read(8);

			while (cont > 0) {
				out << c;
				cont--;
			}
		}
		else if (L < 128 && L >= 0) {
			L++;
			while (L > 0) {
				uint8_t c = br.read(8);
				out << c;
				L--;
			}
		}
		else {
			cout << " non si dovrebbe arrivare qua" << endl;
			return false;
		}
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		syntax();
	}

	char mod = *argv[1];

	bool ris = mod == 'c' ? encode(argv[2], argv[3]) : mod == 'd' ? decode(argv[2], argv[3]) : false;
}