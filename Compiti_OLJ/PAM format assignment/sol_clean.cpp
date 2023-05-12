#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cassert>
#include<vector>

// this is a better and cleaned soluction for the ex_1, using the classes  that the prof has create
/*the first assignment is:
* 
*	write a program that generates a gray level image of 256x256 pixels, where the first row is made of 256 zeros, second is 256 ones, ...
* Save the img in PAM format. The img in XnView should appear as gradient black to white from top to bottom
*  */


// thing the img like a vector 
template <typename T>
struct mat
{
	int rows_;
	int cols_;
	std::vector<T> data_;

	mat(int rows = 0, int cols = 0) : rows_(rows), cols_(cols), data_(rows* cols) {}

	// is necessary to create a method to resize the dimension 
	void resize(int rows, int cols) {
		*this = mat(rows, cols); 
	}
	// create the overload operator: 
	const T& operator()(int r, int c) const {
		// make an assert 
		assert(r >= 0 && r <= rows_ && c >= 0 && c <= cols_);
		return data_[r * cols_ + c];
	}
	// also for change the value of a arbitrary number:
	T& operator()(int r, int c) {
		assert(r >= 0 && r <= rows_ && c >= 0 && c <= cols_);
		return data_[r * cols_ + c];
	}

	// the other method for the selection:
	int rows() const { return rows_; }
	int cols() const { return cols_; }
	int size() const { return cols_ * rows_; }

	// dimension in byte of the data:
	size_t raw_size() const {
		return rows_ * cols_ * sizeof(T);
	}
	 // for saw the data like an array of char , we do  a casting 
	const char* raw_data() const {
		return reinterpret_cast<const char*>(&data_[0]);
	}
};


// create a function that  save a matrix in a file  in PAM format 
bool save_pam(const mat<uint8_t>& img, const std::string& filename) {
	// use ofstream method for don't translet the '\n' 
	std::ofstream out(filename, std::ios::binary);
	if (!out)
		return false;

	// insert the header of the pam:
	out << "P7\n";
	out << "WIDTH " << img.cols() << "\n";
	out << "HEIGHT " << img.rows() << "\n";
	out << "DEPTH 1\n";
	out << "MAXVAL 255\n";
	out << "TUPLTYPE GRAYSCALE\n";
	out << "ENDHDR\n";

	// now we can add all the data
	for (size_t r = 0; r < img.rows(); ++r)
	{
		for (size_t c = 0; c < img.cols(); ++c)
		{
			out.put(img(r, c)); // put write one byte 
		}
	}
	// second method:
	//out.write(img.raw_data(), img.raw_size());

	return true;
}
// we do the load pam of an image in grayscale
bool load_pam(mat<uint8_t>& img, const std::string& filename)
{
	std::ifstream is(filename, std::ios::binary);
	if (!is)
	{
		return false;
	}
	// now we do the parsing:
	std::string magic_number;
	std::getline(is, magic_number);
	if (magic_number != "P7") {
		return false;
	}
	int w, h;

	// it's necessary to do a parse for all the rows 
	while (1) {
		std::string line;
		std::getline(is, line);
		if (line == "ENDHDR") {
			break;
		}
		std::stringstream ss(line);
		std::string token;
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
			if (depth != 1) {
				return false;
			}
		}
		else if (token == "MAXVAL") {
			int maxval;
			ss >> maxval;
			if (maxval != 255) {
				return false;
			}
		}
		else if (token == "TUPLETYPE") {
			std::string tuple;
			ss >> tuple;
			if (tuple != "GRAYSCALE")
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	// resize the img 
	img.resize(h, w);

	// and fill the img with the rest of the data in the file:
	for (size_t r = 0; r < img.rows(); ++r)
	{
		for (size_t c = 0; c < img.cols(); ++c)
		{
			img(r, c) = is.get();
		}
	}
	return true;
}

// add a function that flip an img:
template<typename T>
void flip_inplace(mat<T>& img) {
	using std::swap;
	auto rows = img.rows();
	for (int r = 0; r < rows/2; ++r)
	{
		for (int c = 0; c < img.cols(); ++c)
		{
			swap(img(r, c), img(rows - 1 - r, c));
		}
	}
}

int main(void) {

	// create the matrix
	mat<uint8_t> img1(256, 256);
	
	// we do the assignment of the data, following the text
	for (size_t r = 0; r < img1.rows(); ++r)
	{
		for (size_t c = 0; c < img1.cols(); ++c)
		{
			img1(r, c) = r;
		}
	}
	// know that we've all the data, we can save in PAM format
	save_pam(img1, "out_clean.pam");

	// 2 ex:
	// is necessary to create a function that load a PAM file:
	mat<uint8_t> img2;
	load_pam(img2, "frog.pam");
	// after we load the img we can flip it:
	flip_inplace(img2);

	save_pam(img2, "frog_flipped.pam");


	return 1;
}