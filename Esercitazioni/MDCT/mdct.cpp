#include<cmath>
#include<fstream>
#include<vector>
#include<algorithm>
#include<iomanip>
#include<iostream>
#include<unordered_map>
#define M_PI 3.14

using namespace std;

template< typename T>
std::istream& raw_read(std::istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

class bitreader {
	

	uint8_t buffer_;
	int n_ = 0;
	istream& is_;

public:
	bitreader(istream& is) : is_(is) {}

	uint8_t read_bit() {
		if (n_ == 0) {
			raw_read(is_, buffer_);

			n_ = 8;
		}
		n_--;
		return (buffer_ >> n_) & 1;
	}

	uint32_t read(uint8_t n_bit) {
		uint32_t num;

		while (n_bit --> 0)
		{
			num = (num << 1) | read_bit();
		}
		return num;
	}

	bool fail() const {
		return is_.fail();
	}

	explicit operator bool () const {
		return !fail();
	}
};

// class MDCT:
/*
*	DATI:
	1. Numero di campioni;
	2. istream del file;

	METODI:
	1. l'operazione di trasformazione dei coefficenti;
	2. l'operazione di trasformazioni inversa dei coefficenti;
	3. Reallizare la quantizzazione dei coefficenti;
	4. Dequantizzare i coeff;
	5. Misurare l'entropia;
	6. calcolo del edge attention windows;



*/


template<typename T>
struct freq {
	unordered_map<T, uint32_t> histo;

	void operator()(const T& val) {
		++histo[val];
	}

	double entropy() const {
		double s = 0, h = 0;
		for (const auto& x : histo) {
			s += x.second;
			h += x.second * log2(x.second);
		}
		return log2(s) - h / s;
	}
	auto size() { return histo.size(); }

	auto begin() { return histo.begin(); }
	auto end() { return histo.end(); }
	auto begin() const { return histo.begin(); }
	auto end()const  { return histo.end(); }

};


class MDCT {
	istream& is_;
	vector<double> samples_;
	vector<vector<double>> coeffs_;
	size_t window_;
	size_t windows_processed_ = 0; // serve per processare la finestra 

public:
	MDCT(istream& is, uint16_t w): is_(is), window_(w) {}

	void Read_samples(){
		// leggiamo dal file 
		int16_t v;


		while (true)
		{
			if (is_.fail() || is_.eof())
				break;

			raw_read<int16_t>(is_, v);
			samples_.push_back(v);
		}

		
		// alternativa 2:
		/*
		is_.seekg(0, ios::end);
		auto dim = is_.tellg();
		is_.seekg(0, ios::beg);
		samples_.resize(size_t(dim) / sizeof(double));
		is_.read(reinterpret_cast<char*>(samples_.data()), dim);
		*/
	}

	vector<double> sin_tab;
	vector<vector<double>> cos_tab;
	void precalc_tables() {
		sin_tab.resize(2 * window_);
		for (size_t i = 0; i < sin_tab.size(); ++i)
			sin_tab[i] = sin((M_PI / 2 * window_) / (i + 0.5));

		cos_tab = vector<vector<double>>(window_, vector <double>(2 * window_));

		for (size_t k = 0; k < window_; ++k) {
			for (size_t n = 0; n < 2 * window_; ++n) {
				cos_tab[k][n] = cos((M_PI / window_) * (n + 0.5 + window_ * 0.5) + (k + 0.5));
			}
		}
	}

	double edge_attention(double x_n) {
		return sin_tab[x_n];
	}

	void process_window(size_t curr_window) {
		vector<double> curr_coeffs;
		double X_curr = 0;

		for (size_t k = 0; k < window_; k++) {

			for (size_t n = 0; n < 2 * window_; n++) {
				size_t index = n + (window_ * curr_window);

				double x_n = samples_.size() > index ? samples_[index] : 0; // questo serve per inserire all'inizio e alla fine una finestra di zeri
				X_curr += x_n * edge_attention(x_n) * cos_tab[k][n];
			}
			curr_coeffs.push_back(X_curr);
		}
		coeffs_.push_back(curr_coeffs);
	}

	void direct() {
		precalc_tables();
		while (windows_processed_ * window_ < samples_.size()) {
			process_window(windows_processed_);
			windows_processed_++;
		}
	}

	void quantize_coeff_(unsigned Q) {
		for (auto& v : coeffs_) {
			for (auto& coeff : v)
				coeff /= Q;
		}
	}

	void info() {

		cout << "num of samples: " << samples_.size() << endl;
		cout << "num of coeffs: " << samples_.size() << endl;
	}

	void compute_entropy() {
		freq<double> f;
		f = for_each(begin(samples_), end(samples_), f);
		cout << "Numero di valori differenti: " << f.size() << endl;
		cout << "Entropia: " << f.entropy() << endl;
	}


	
 
};


int main(int argc, char* argv[]) {

	if (argc != 2) {
		return -1;
	}

	ifstream in("test.raw", ios::binary);
	if (!in) {
		return -1;
	}

	bitreader br(in);

	MDCT m(in, 1024);

	m.Read_samples();
	m.direct();
	m.info();
	m.quantize_coeff_(10000);
	m.compute_entropy();
	


	return 0;
}