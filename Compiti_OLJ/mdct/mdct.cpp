#include<iostream>
#include<cstdint>
#include<vector>
#include<iterator>
#include<math.h>
#include<string>
#include<fstream>
#include<istream>

using namespace std;

double entropy(vector<int16_t>& v) {
	// calcoliamo l'entropia
	return 0;
}

int main(int argc, char* argv[]) {

	// estrarre il file 

	ifstream is("test.raw", std::ios::binary);

	if (!is) {
		cout << "error in the opening of the file\n";
	}
	istream_iterator<int16_t> is_start(is);
	istream_iterator<int16_t> stop;

	vector<int16_t> v(is_start, stop);

	cout << "the size of v is: " << v.size();

	double entropia;
	entropia = entropy(v);


	return 0;
}