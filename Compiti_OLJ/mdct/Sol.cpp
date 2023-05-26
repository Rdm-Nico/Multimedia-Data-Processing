// sol del prof spiegata
// file stream in binario 
/*
	usa us.seekg(0,std::ios::end); -> faccio la sick spostan
	auto pos = is.tellg() -> prende la dim 
	is.seekg(0,std::ios::beg)

	e poi creo un vettore con la dim 
	e poi faccio una read con un reinterpreter cast di puntatore a char 

	diff tra v.resize() e v.reserve()
	resize alloca la memoria e mette la v.size() alla dim del vettore interessato 

	la reserve si limita ad allocare la memoria ma la v.size() � ancora a 0 
	la reserve pu� essere utile con i cicli quando si utilizza la v.push_back() del vettore 
	e si vuole aggiungere un elem alla volta 

	se volete la RVO( return value optimization) dovete mettere un unico vettore nella funzione 
	*/

/*
	il prof ha fatto un formato .wav cos� 

	calcolare l'entropia:
	std::ranges::input_range auto &&range -> 

	fai un template con typename itT  -> due iteratori begin(samples) e end(samples)

	fare una mappa che a ogni valore associ in numero che volte che compare 
	
	*first � il primo elem della first e uso decltype ( type_of nel c++) e ooi facciamo una std::remove_reference toglie la reference -> con ::type  posso riceve il tipo di dato
	e uso un static cast della distance(first,last)

	facciamo una mappa  di tipo e il count 

	e poi uso gli iteratori:

	e poi calcolo l'entropia:

	con un for( auto& [x,c] : count) -> estrapolo i due valori della mappa count  ( structured bindings)

	e faccio dei passaggi algebrici per calcolare l'entropia 

*/

/*
	e poi bisogna quantizzare:
	uso std::placeholder per creare una classe _1 
	utilizzo std::trasform() che applica una funzione std::divide<int>{}, _1, Q 
	e prendiamo la std::bind che  leghiamo la divide e facciamo il binding dei dei parametri _1 e Q -> � una funzione che accetta un solito 

	ma non � molto comodo da usare:

	si usa una funzione lambda 
	[& -> piglia tutto come reference , se no si pu� usare direttamente Q](int val -> dove si istanziano i parametri){ 
			return val / Q;
	}
	si pu� anche dividere l'implementazione 


	nel c++ c'� std::ranges nuovi tipi di algo basati su degli intervalli che sono reallizati tramite i "concetti" ( gli devi passare qualcosa che rispetti questi requisiti: avere begin() e end() ) 
	
	usiamo la copy 
	con std::views:trasform permette di far diventare un operazione -> applico la funzione scritta dentro al trasform  ad ogni elemento di samples
	*/

/*
	trasformata coseno discreto:
	creo una tabella di coseni calcolati 


	e poi creo i vettore con N zeri prima e zeri dopo 
	e poi per ogni finestra chiamoa la win_mdct 

	per la IMDCT
	mi tengo la finestra precedente 
	partendo dalla posizione 1, trasformo la finestra corrente e alla fine ottengo gli N campioni 
	e poi prev = move(curr)

	questa versione fa scifo perch� � molto lenta 
*/

#define _USE_MATH_DEFINES
#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <cstdint>

template <typename T>
struct freq {
	std::unordered_map<T, uint32_t> histo;

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
	auto end() const { return histo.end(); }
};

template <typename T>
std::vector<T> read_track(const std::string& fname)
{
	using namespace std;
	ifstream is(fname, ios::binary);
	is.seekg(0, ios::end);
	auto dim = is.tellg();
	is.seekg(0, ios::beg);

	vector<T> samples(size_t(dim) / sizeof(T));
	is.read(reinterpret_cast<char*>(samples.data()), dim);
	return samples;
}

template<typename T>
void write_track(const std::string& fname, const std::vector<T>& samples)
{
	using namespace std;
	ofstream os(fname, ios::binary);
	os.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(T));
}

template<typename T>
void print_freq(const std::vector<T>& samples)
{
	using namespace std;
	freq<T> f;
	f = for_each(begin(samples), end(samples), f);
	cout << "Numero di valori differenti: " << f.size() << "\n";
	cout << "Entropia: " << f.entropy() << "\n";
}

template<typename T>
T quant(const T& val, double factor)
{
	return T(lround(val / factor));
}

template<typename T>
T dequant(const T& val, double factor)
{
	return T(lround(val * factor));
}

std::vector<double> sin_tab;
std::vector<std::vector<double>> cos_tab;

void precalc_tables(size_t N)
{
	sin_tab.resize(2 * N);
	for (size_t i = 0; i < sin_tab.size(); ++i)
		sin_tab[i] = sin((M_PI / (2 * N)) * (i + 0.5));

	cos_tab = std::vector<std::vector<double>>(N, std::vector<double>(2 * N));
	for (size_t k = 0; k < N; ++k) {
		for (size_t n = 0; n < 2 * N; ++n) {
			cos_tab[k][n] = cos(M_PI / N * (n + 0.5 + N / 2) * (k + 0.5));
		}
	}
}

template<typename CT, typename ST>
void win_mdct(const std::vector<ST>& padded, std::vector<CT>& coeffs, size_t offset, size_t N)
{
	for (size_t k = 0; k < N; ++k) {
		double Xk = 0.;
		for (size_t n = 0; n < 2 * N; ++n) {
			ST xn = padded[offset * N + n];
			double wn = sin_tab[n];
			double cn = cos_tab[k][n];

			Xk += xn * cn * wn;
		}

		coeffs[offset * N + k] = CT(round(Xk));
	}
}


template<typename CT, typename ST>
std::vector<CT> MDCT(const std::vector<ST>& samples, size_t N)
{
	using namespace std;
	auto nwin = size_t(ceil(samples.size() / double(N))) + 2;
	vector<ST> padded(nwin * N, 0);
	copy(begin(samples), end(samples), begin(padded) + N);

	vector<CT> coeffs((nwin - 1) * N);
	for (size_t i = 0; i < nwin - 1; ++i)
		win_mdct(padded, coeffs, i, N);

	return coeffs;
}

template<typename CT>
std::vector<double> win_imdct(const std::vector<CT>& coeffs, size_t offset, size_t N)
{
	using namespace std;
	vector<double> recon(2 * N);

	for (size_t n = 0; n < 2 * N; ++n) {
		double tmpn = 0.;
		for (size_t k = 0; k < N; ++k) {
			CT xk = coeffs[offset * N + k];
			double ck = cos_tab[k][n];
			tmpn += ck * xk;
		}

		recon[n] = 2. / N * sin_tab[n] * tmpn;
	}

	return recon;
}

template<typename ST, typename CT>
std::vector<ST> IMDCT(const std::vector<CT>& coeffs, size_t N)
{
	using namespace std;
	auto nwin = coeffs.size() / N;
	vector<ST> samples((nwin - 1) * N);

	vector<double> prev = win_imdct(coeffs, 0, N);
	for (size_t i = 1; i < nwin; ++i) {
		vector<double> curr = win_imdct(coeffs, i, N);

		for (size_t j = 0; j < N; ++j)
			samples[(i - 1) * N + j] = ST(round(curr[j] + prev[N + j]));

		prev = move(curr);
	}

	return samples;
}

int main()
{
	using namespace std::placeholders;
	using datatype = int16_t;
	using coefftype = int32_t;

	auto original = read_track<datatype>("test.raw");

	std::cout << "Info su campioni:\n";
	print_freq(original);

	// quantize in time
	if (true) {
		double quant_fac = 2600;
		auto quant_f = bind(quant<datatype>, _1, quant_fac); // quantize with factor 2600
		auto dequant_f = bind(dequant<datatype>, _1, quant_fac); // dequantize with factor 2600

		std::vector<datatype> quant_t(original.size()); // apply quantization in all samples
		transform(begin(original), end(original), begin(quant_t), quant_f);

		std::cout << "Info su campioni quantizzati:\n";
		print_freq(quant_t);

		std::vector<datatype> recon(original.size()); // apply dequantization in all samples
		transform(begin(quant_t), end(quant_t), begin(recon), dequant_f);

		write_track("output_qt.raw", recon); // write dequantized samples

		// compute error
        // difference between original and dequantized samples
		std::vector<datatype> error(original.size());
		for (size_t i = 0; i < original.size(); ++i)
			error[i] = original[i] - recon[i];
        
        // potevi farlo anche con la trasform 
        // take all the elements of original and recon and apply the minus operator to them and then store the result in error
        std::transform(begin(original), end(original), begin(recon),begin(error), std::minus<datatype>());

		write_track("error_qt.raw", error);
	}

	// quantize in frequency
	if (true) {
		double quant_fac = 10000;
		auto quant_f = bind(quant<coefftype>, _1, quant_fac);
		auto dequant_f = bind(dequant<coefftype>, _1, quant_fac);
		// N is the number of coefficients
		size_t N = 1024;

		precalc_tables(N);
		auto coeff = MDCT<coefftype>(original, N);

		std::cout << "Info su coefficienti:\n";
		print_freq(coeff);

		// quantize
		std::vector<coefftype> coeff_q(coeff.size());
		transform(begin(coeff), end(coeff), begin(coeff_q), quant_f);

		std::cout << "Info su coefficienti quantizzati:\n";
		print_freq(coeff_q);

		// dequantize
		std::vector<coefftype> coeff_recon(coeff.size());
		transform(begin(coeff_q), end(coeff_q), begin(coeff_recon), dequant_f);

		auto recon = IMDCT<datatype>(coeff_recon, N);

		write_track("output.raw", recon);

		std::vector<datatype> error(recon.size());
		for (size_t i = 0; i < original.size(); ++i)
			error[i] = original[i] - recon[i];

		write_track("error.raw", error);
	}
}