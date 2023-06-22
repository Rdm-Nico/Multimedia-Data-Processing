#include<iostream>
#include<fstream>
#include<vector>
#include<cstdint>
#include<sstream>
#include<cassert>
#include<array>


template <typename T>
class mat {
	int rows_;
	int cols_;
	std::vector<T> data_;

public:
	mat(int rows = 0, int cols = 0): rows_(rows), cols_(cols), data_(rows*cols){}

	void resize(int rows, int cols) {
		*this = mat(rows, cols);
	}

	const T& operator()(int r, int c) const {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}
	T& operator()(int r, int c) {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	int rows() const { return rows_; }
	int cols() const { return cols_; }
	int size() const { return rows_*cols_; }

	// dimensione in byte
	size_t raw_size() const {
		return rows_ * cols_ * sizeof(T);
	}

	char* raw_data(){ // ritorna la zona della memoria dove incominciano i dati
		return reinterpret_cast<char*>(&data_[0]);
	}

	const char* raw_data() const { // ritorna la zona della memoria dove incominciano i dati
		return reinterpret_cast< const char*>(&data_[0]);
	}

	auto begin() { return data_.begin(); }
	auto begin() const  { return data_.begin(); }
	auto end() { return data_.end(); }
	auto end() const { return data_.end(); }
};

// per poter gestire le img PAM in formato RGB e' necessario dover creare un array  di 3 elementi specifico
using rgb = std::array<uint8_t, 3>;



bool load_pam(std::ifstream& is, mat<rgb>& img) {
	using namespace std;

	string magic_number;

	getline(is, magic_number);
	if (magic_number != "P7")
		return false;

	int w, h;

	while (true) {
		string line;
		getline(is, line);
		if (line == "ENDHDR")
			break; // siamo arrivati alla fine del header

		stringstream ss(line);
		string token;
		ss >> token;
		if (token == "WIDTH") {
			ss >> w;
		}
		else if (token == "HEIGHT") {
			ss >> h;
		}
		else if (token == "DEPTH") {
			int depth;
			ss >> depth;
			if (depth != 3)
				return false;
		}
		else if (token == "MAXVAL") {
			int maxval;
			ss >> maxval;
			if (maxval != 255)
				return false;
		}
		else if (token == "TUPLTYPE") {
			string tupletype;
			ss >> tupletype;
			if (tupletype != "RGB")
				return false;
		}
		else
			return false; // non ci puo' essere un valore diverso da questi
	}

	// finito il parser
	img.resize(h, w);

	// adesso dobbiamo inserire gli elementi dentro alla matrice 
	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c) {
			img(r, c)[0] = is.get();
			img(r, c)[1] = is.get();
			img(r, c)[2] = is.get();

		}
	}
	return true;
}


void move_data(mat<rgb>& img, mat<uint8_t>& plane, int index) {
	// adesso dobbiamo inserire gli elementi dentro alla matrice 
	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c) {
			 plane(r,c) = img(r, c)[index];
		}
	}
}

std::string strRemoveExt(std::string filename, char del) {
	// elimina gli elemeti dopo del(compreso del)

	std::string ret;

	std::stringstream ss(filename);
	char x;
	while (true) {
		ss >> x;
		if (x == del) {
			break;
		}
		ret.push_back(x);

	}
	return ret;
}

bool save_pam(const std::string& filename, const mat<uint8_t>& img) {
	using namespace std;

	ofstream os(filename, ios::binary);
	if (!os) {
		return false;
	}
	os << "P7\n";
	os << "WIDTH " << img.cols() << "\n";
	os << "HEIGHT " << img.rows() << "\n";
	os << "DEPTH 1\n";
	os << "MAXVAL 255\n";
	os << "TUPLTYPE GRAYSCALE\n";
	os << "ENDHDR\n";

	os.write(img.raw_data(), img.raw_size());

	return true;
}

bool split_img(std::string filename,mat<rgb>& rgb_img,mat<uint8_t>& plane, uint8_t i) {
	using namespace std;

	plane.resize(rgb_img.rows(), rgb_img.cols());


	// per ogni plane ora dobbiamo inserire i dati
	
	move_data(rgb_img, plane, i);

	stringstream plane_filename;

	char x = i == 0 ? 'R' : i == 1 ? 'G' : 'B';

	plane_filename << strRemoveExt(filename, '.') << "_" << x << ".pam";

	save_pam(plane_filename.str(), plane);


	return true;
}



bool split(std::string filename) {
	using namespace std;

	ifstream is(filename, std::ios::binary);
	if (!is) {
		return EXIT_FAILURE;
	}

	mat<rgb> img;

	if (!load_pam(is, img)) {
		return EXIT_FAILURE;
	}
	
	// vettori dei piani R G e B

	vector<mat<uint8_t>> planes(3);

	for (size_t i = 0; i < 3; i++) {
		planes[i] = mat<uint8_t>();
		split_img(filename, img, planes[i], i);
	}

	/*
	if (!split_img(img, planes)) {
		return EXIT_FAILURE;
	}
	// le salviamo in formato pam grayscale
	char letter[] = {'R','G','B'};

	stringstream stringa;
	stringstream nuova;
	stringa << filename;
	char c;
	while (1) {
		stringa >> c;
		if (c == '.')
			break;
		nuova.put(c);
	}
	nuova.put('_');

	string ext = ".pam";
	
	stringstream r; 
	r << nuova.str();
	stringstream g;
	g << nuova.str();
	stringstream b;
	b << nuova.str();

	r.put(letter[0]);
	g.put(letter[1]);
	b.put(letter[2]);

	r << ext;
	g << ext;
	b << ext;

	save_pam(r.str(), planes[0]);
	save_pam(g.str(), planes[1]);
	save_pam(b.str(), planes[2]);

	vector<stringstream> vec_file;
	vec_file.push_back(r);
	vec_file.push_back(g);
	vec_file.push_back(b);

	for (int i = 0; i < 3; i++) {
		vec_file[i].put(letter[i]);
		vec_file[i] << ext;
		if (!save_pam(vec_file[i].str(), planes[i]))
			return false;
		//nuova.seekp(7);
		//nuova;
		//nuova << copia.str();
	}
	*/
	return true;
}


int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "error usage:split <filename.pam> " << std::endl;
		return EXIT_FAILURE;
	}
	std::string filename = argv[1];
	split(filename);

	
	return 0;
}