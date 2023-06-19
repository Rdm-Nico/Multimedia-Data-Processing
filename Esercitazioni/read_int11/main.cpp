#include <vector>
#include <fstream>
#include <iterator>
#include <cstdint>
#include <iostream>

template<typename T>

std::istream& raw_read(std::istream& os,T& value, size_t size = sizeof(T)) {
	return os.read(reinterpret_cast<char*>(&value), size);
}

class bitreader {
	uint8_t buffer_;
	int n_ = 0; 
	std::istream& is_;



	

	public:
		bitreader(std::istream &is): is_(is) {}

		uint32_t read_bit() {
			if (n_ == 0) {
				raw_read(is_, buffer_);
				n_ = 8;
			}
			n_--;
			return(buffer_ >> n_) & 1; // ritorniamo il bit interessato
		}

		uint32_t read(uint8_t n_bit) { // fa la lettura di n bit e ritorna il valore 
			uint32_t u = 0;
			while (n_bit--> 0)
			{
				u = (u << 1) | read_bit();
			}
			return u;
		}

		uint32_t operator()(uint8_t n_bit) {
			return read(n_bit);
		}

		bool fail() const { // deve essere const perche non modifica lo stato dell'oggetto
			return is_.fail();
		}

		explicit operator bool() const {
			return !fail();
		}
};



int main(int argc,char* argv[]) {

	if (argc != 3) {
		std::cout << "Usage: read_int11 <filein.bin> <fileout.txt>\n";
	}

	std::ifstream is(argv[1], std::ios::binary);
	if (!is) {
		return 1;
	}

	bitreader br(is);

	std::vector<int32_t> v;

	while (1) {
		int32_t num = br(11);
		if (!br) {
			break;
		}
		if (num >= 1024) { // necessario per complemento a due 
			num -= 2048;
		}
		v.push_back(num);
	}


	std::ofstream out(argv[2]);
	if (!out) {
		return 1;
	}

	std::copy(v.begin(), v.end(), std::ostream_iterator<int32_t>(out, "\n"));

	return 0;
}