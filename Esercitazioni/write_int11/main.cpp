#include<iostream>
#include<fstream>
#include<cstdint>
#include<vector>
#include<iterator>

template<typename T>

std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
}

class bitwriter {
	uint8_t buffer_;
	int n_ = 0; // numero di elementi del buffer
	std::ostream& os_;

	std::ostream& write_bit(uint8_t bit) {
		buffer_ = buffer_ * 2 + bit; // aggiungiamo il bit al buffer spostandolo a sinistra 
		n_++;
		if (n_ == 8) {
			raw_write(os_, buffer_);
			n_ = 0; // resettiamo il buffer
		}
		return os_;
	}

	public:
		bitwriter(std::ostream& os): os_(os){}

		~bitwriter() {
			flush();
		}

		std::ostream& flush(uint32_t bit = 0) {
			while (n_ > 0)
			{
				write_bit(bit);
			}
			return os_;
		}

		std::ostream& write(uint32_t u, uint8_t n) {
			for (int i = n-1; i >= 0; --i)
			{
				uint32_t cure_bit = (u >> i) & 1; // selezione del bit i-esimo
				write_bit(cure_bit);
			}
			return os_;
		}

		std::ostream& operator()(uint32_t u, uint8_t n) {
			return write(u, n);
		}
		
};


int main(int argc, char* argv[]) {

	if (argc != 3) {
		std::cout << "Usage: write_int11 <filein.txt> <fileout.bin>\n";
		return 1;
	}

	std::cout << argv[1];
	std::ifstream is(argv[1]);
	if (!is) {
		return 1;
	}

	std::vector<int32_t> v{
		std::istream_iterator<int32_t>(is),
		std::istream_iterator<int32_t>()
	};

	std::ofstream out(argv[2], std::ios::binary);
	if (!out)
	{
		return 1;
	}


	bitwriter bw(out);

	for (const auto& x : v) {
		bw(x, 11);
	}
	return 0;
}