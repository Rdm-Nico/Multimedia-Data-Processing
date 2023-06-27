#include "pbm.h"

//bool BinaryImage::ReadFromPBMC(const string& filename);



int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "error in pass the params" << endl;
		return false;
	}

	BinaryImage img;
	bool ris = img.ReadFromPBM(argv[1]);

	return 0;
}
