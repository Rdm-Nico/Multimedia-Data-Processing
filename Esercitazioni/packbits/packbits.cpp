#include<vector>
#include<iterator>
#include<iostream>
#include<fstream>
#include<deque>

using namespace std;

template< typename T>
ostream& raw_write(ostream& os,const T& value, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&value), size);
}

class bitwriter {
	uint8_t buffer_;
	int n_ = 0;
	ostream& os_;

	ostream& write_bit(uint32_t bit) {
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

		~bitwriter() {
			flush();
		}

		ostream& flush(uint32_t bit = 0) {
			while (n_ > 0) {
				write_bit(bit);
			}
			return os_;
		}
		ostream& write(uint32_t num, uint8_t n_bit) {
			for (int i = n_bit - 1; i >= 0; --i) {
				uint32_t cure_bit = (num >> i) & 1;
				write_bit(cure_bit);
			}
			return os_;
		}
		ostream& operator()(uint32_t num, uint8_t n_bit) {
			return write(num, n_bit);
		}

};

template< typename T>
istream& raw_read(istream& is, T& value, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&value), size);
}

class bitreader {
	uint8_t buffer_;
	int n_ = 0;
	istream& is_;

	public:
		bitreader(istream& is): is_(is){}

		uint32_t read_bit() {
			if (n_ == 0) {
				raw_read(is_, buffer_);
				n_ = 8;
			}
			n_--;
			return(buffer_ >> n_) & 1;
		}
		uint32_t read(uint8_t n_bit) {
			uint32_t u = 0;
			while (n_bit --> 0)
			{
				u = (u << 1) | read_bit();
			}
			return u;
		}
		uint32_t operator()(uint8_t n_bit) {
			return read(n_bit);
		}

		bool fail() const  {
			return is_.fail();
		}
		explicit operator bool() const {
			return !fail();
		}
};

template< typename container>
uint8_t get_run_index(uint8_t c, const container& Container) {
	container v = Container;
	uint8_t index = 0;
	bool flag = false;
		
	for (auto e : v) {
		if (e == c && index < 128) {
			index++;
			flag = true;
		}
		else {
			if (flag) { // flag = true
				return index;
			}
		}
	}
	return index;
}


int rle_run(uint8_t c, uint8_t n, bitwriter& bw) {
	// qua inserimo il carattere c, preceduto dalla suo numero di occorrezze
	uint16_t v = 257;
	v -= n;
	bw(v, 8);
	bw(c, 8);

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		cout << "Usage:packbits [c|d] <input file> <output file>";
		return 1;
	}

	auto symbol = *argv[1];

	switch (symbol)
	{
	case 'c': {
		// compressione 
		ifstream is(argv[2], ios::binary);
		ofstream out(argv[3], ios::binary);

		if (!is || !out) {
			return -1;
		}
		bitreader br(is);
		bitwriter bw(out);

		vector<uint8_t> control(0); // vettore che viene utilizzato per controllare quanti caratteri di seguito ci sono 
		char e = 0, curr = 0;
		char past = 0;
		size_t count = 0;

		while (is.get(e)) { // legge i  caratteri uno alla volta e li salva dentro e
			count++;
		}
		// avremo in count la lunghezza in byte del file 
		is.clear();
		is.seekg(0);

		bool flag = false;

		deque<uint8_t> copy_buffer; // caratteri che devono essere codificati 

		deque<uint8_t> index_q; // coda degli indici -> Es: se abbiamo 4 volte 'a' di seguito, dentro index_q avremo '4'

		uint8_t index = 0;

		for (size_t k = 0; k < count; k += index) {
			is.get(past);// prende e lo salva dentro a past
			if (is.fail() || is.eof())
				break;
			if (!flag) {
				is.seekg(0); // riposizioniamo il cursore all'inizio
			}
			flag = true;
			index = 0;
			streamoff pos = is.tellg(); // alias di long long
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
				index_q.push_back(index);
			}
			if (control.size() > 0) {
				is.clear();
			}
			control.clear(); // rimuove tutti gli elem dal container 

			is.seekg(pos + (index - 1));

		}
		while (index_q.size() > 0) {
			if (index_q.front() > 1) { // primo elem della coda degli indici -> quando un carattere e' presente piu' di una volta 
				rle_run(copy_buffer.front(), index_q.front(), bw);
				index_q.pop_front(); // tiriamo via il primo elem
				copy_buffer.pop_front();

				continue;
			}

			if (index_q.front() == 1) { // solo elem che compaiono una volta 
				uint8_t cont = get_run_index(1, index_q); // ritorna il numero di elemnti che devono essere copiati di seguito
				
				bw(cont - 1, 8);

				for (uint8_t r = 0; r < cont; r++) {
					bw(copy_buffer.front(), 8);
					copy_buffer.pop_front();
				}

				index_q.erase(index_q.begin(), index_q.begin() + cont); // elimina dalla coda degli indici tutti gli elementi singoli 
			}
		}

		bw(128, 8);

		break;
	}
	case 'd': {
		// decompressione 
		ifstream is(argv[2], ios::binary);
		ofstream out(argv[3]);

		if (!is || !out) {
			return -1;
		}
		bitreader br(is);
		

		while (!br.fail()) {
			uint32_t L = br.read(8);

			if (L == 128) {
				// finito la decodifica
				break;
			}
			else if (L > 128 && L <= 255 ) {
				// dobbiamo leggere dei valori di seguito
				uint32_t cont = 257 - L;

				uint8_t c = br.read(8);

				while (cont > 0) {
					out << c;
					cont--;
				}
			}
			else if(L < 128 && L >= 0) {
				// c'e' da leggere il numero  di caratteri indicato da L +1
				L++;
				while (L > 0) {
					uint8_t c = br.read(8);
					out << c;
					L--;
				}
			}
		}
		break;
	}

	default:
		cout << "Usage [c,d]" << endl;
		return -1;
		break;
	}
	return 0;
}