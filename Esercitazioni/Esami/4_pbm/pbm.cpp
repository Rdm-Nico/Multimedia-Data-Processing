#include "pbm.h"

//bool BinaryImage::ReadFromPBMC(const string& filename);




Image BinaryImageToImage(const BinaryImage& bimg) {

	Image img;
	// vettore che dovrebbe essere di 500 per 500 

	img.H = bimg.H;
	img.W = bimg.W;

	


	int zeros_padded = img.W % 8;


	

	for (size_t r = 0; r < img.H; r++) {
		for (size_t c = 0; c < bimg.n_bytes; c++) {	

			
			uint8_t num = bimg.ImageData[r * bimg.n_bytes + c];

			uint8_t n_bit = 8;

			if (c == bimg.n_bytes - 1) {
				// ultimo byte della riga viene paddato con  zeros_padded
				n_bit = zeros_padded;
			}

			while (n_bit-- > 0)
			{
				uint8_t cure_bit = (num >> n_bit) & 1;

				if (cure_bit == 0) {
					img.ImageData.push_back(0);
				}
				else
				{
					img.ImageData.push_back(255);

				}
			}
		}


	}
	return img;
}



int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "error in pass the params" << endl;
		return false;
	}

	BinaryImage img_bin;
	bool ris = img_bin.ReadFromPBM(argv[1]);

	Image img;
	img = BinaryImageToImage(img_bin);


	return 0;
}

