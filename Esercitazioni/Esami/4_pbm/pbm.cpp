#include "pbm.h"

//bool BinaryImage::ReadFromPBMC(const string& filename);



int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "error in pass the params" << endl;
		return false;
	}

	BinaryImage img_bin;
	bool ris = img_bin.ReadFromPBM(argv[1]);

	Image img;
	img.BinaryImageToImage(img_bin);


	return 0;
}
