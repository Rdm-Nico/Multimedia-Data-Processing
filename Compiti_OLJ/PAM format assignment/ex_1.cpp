#include <iostream>
#include <fstream>
#include<vector>
#include<cassert> // per le assert 
#include<cstdint>
#include<string>
#include<sstream> // strattare come le string le righe 
// create a 256x256 pixels image , where is draw a gradient from black to white from top to down, in PAM format 



// vedere le img come matrici , come matrici di dati.
template <typename T>
struct mat {
	int rows_;
	int cols_;
	std::vector<T> data_; // matrice di int in questo caso 

	// costruttore 
	mat(int rows = 0, int cols = 0): rows_(rows), cols_(cols), data_(rows*cols){}

	void resize(int rows, int cols) {
		*this = mat(rows, cols); // operatore di assegnamento di defalut 
	}

	// prendere un elemento  in una certa riga e certa colonna
	const T& operator()(int r, int c) const{
		assert(r >= 0 && r < rows_ && c>= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	T& operator()(int r, int c){
		assert(r >= 0 && r < rows_ && c>= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	// per cercare di evitare di copiare 
	//T& at(int r, int c) {
		//return const_cast<T&>(static_cast<const mat*>(this)->at(r, c));
	//}

	int rows() const { return rows_;}
	int cols() const {return cols_;}
	int size() const { return rows_ * cols_; }

	// dimensione in byte dei dati
	size_t raw_size()const {
		return rows_ * cols_ * sizeof(T);
	}
	const char *raw_data()const {
		return reinterpret_cast<const char *>(&data_[0]);
	}
};
// sempre una const & perch� vogliamo non vogliamo una  copia 
bool save_pam(const mat<uint8_t>& img, const std::string& filename) { // guarda se vuoi string view C++ 17
	// create the PAM file 
	std::ofstream out(filename,std::ios::binary); // per non tradurre i \n 
	if (!out) {
		return false;
	}
	// insert the header of the pam file:
	out << "P7\n";
	out << "WIDTH " << img.cols() << "\n";
	out << "HEIGHT " << img.rows() << "\n";
	out << "DEPTH 1\n";
	out << "MAXVAL 255\n";
	out << "TUPLTYPE GRAYSCALE\n";
	out << "ENDHDR\n";

	// write the raster --> scriviamo tutti i byte della nostra immagine 
	// 1 metodo:
	for (int r = 0; r < img.rows(); ++r)
	{
		for (int c = 0; c < img.cols(); ++c)
		{
			out.put(img(r, c)); // put scrive un byte 
		}
	}	
	// 2 metodo:
	//out.write(img.raw_data(), img.raw_size());

	return true;
}

bool load_pam(mat<uint8_t> &img, const std::string& filename) {
	std::ifstream is(filename, std::ios::binary);
	if (!is)
		return false;

	// dentro string c'� getline
	std:: string magic_number;
	std::getline(is, magic_number);
	// fino a end token ci sono le varie righe  -> che possono essere inserite in ordine causale 
	// dobbiamo fare un parser -> che legge le righe e le insrisce bene 
	
	int w, h; // larghezza e altgezz a
	while (1)
	{
		std::string line;
		std::getline(is, line);
		if (line == "ENDHDR")
			break;
		// facciamo il parsing della linea
		std::stringstream ss(line);
		std::string token;
		ss >> token; // estrai il primo token dalla stringa --> leggono i caratteri fino al primo whitespace
		if (token == "WIDTH") {
			ss >> w;
		}
		else if(token == "EIGHT"){
			ss >> h;
		}
		else if (token == "DEPTH"){
			int depth;
			ss >> depth;
			if (depth != 1) // perchè abbiamo solo immagini a 1 canale
				return false;
		}
		else if (token == "MAXVAL"){
			int  maxval;
				ss >> maxval; 
				if (maxval > 255)
					return false;
		}
		else if (token == "TUPLTYPE"){
			std::string tupltype;
			ss >> tupltype;
		}
	}

	// facciamo la resize dell'img 
	img.resize(h, w);


	for (int r = 0; r < img.rows(); ++r)
	{
		for (int c = 0; c < img.cols(); ++c)
		{
			img(r, c) = is.get(); // put permette di scrivere un byte
		}
	}

	return true;
}
int main(void) {
	

	// create the matrix
	mat<uint8_t> imgl(256, 256);

	for (int r = 0; r < imgl.rows(); ++r)
	{
		for (int c = 0; c < imgl.cols(); ++c)
		{
			imgl(r, c) = r;
		}
	}
	save_pam(imgl, "output.pam");

	// 2 ex 
	mat<uint8_t> img2;
	// read a file in pam version 
	load_pam(img2, "frog.pam");

	save_pam(img2, "frog_flipped.pam");

	return 1;
}