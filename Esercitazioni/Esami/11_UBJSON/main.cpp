#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <functional>
#include <exception>

#include "bitstreams.h"
#include "ppm.h"
#include "types.h"
#include "image_operations.h"


/*
			COSE DA MIGLIORARE:
			1. Pulire il codice con la conoscenza acqusita ora del parsing 
			2. Salvare in modo coretto i nomi delle immagini
			3. fare correttamente il dump
			4. e applicare all'immagine quelle estratte


*/

using namespace std;

struct json {

	ifstream& is_;
	uint8_t W_;
	uint8_t H_;

	json(ifstream& is): is_(is){}
	
	


	
	
};

struct elements : public json{
		string type_;
		uint8_t x_;
		uint8_t y_;
		uint8_t width_;
		uint8_t height_;
		vector<image<vec3b>> imgs_;
		size_t count = 0 ;
		

		elements(ifstream& is) : json(is) {

			image<vec3b> v;
			while (true) {
				if (is.fail() || is.eof() || is.peek() != '}') {
					break;
				}
				

			
			is.ignore();
			is.ignore();
			is.ignore();
			char c;
			string elems_string;
			while (true)
			{
				is.get(c);
				if (c == '{')
					break;
				elems_string.push_back(c);
			}
			if (elems_string != "elements") {
				exit(EXIT_FAILURE);
			}
			elems_string.clear();
			
			// noi adesso dovremmo skippare fino a trovare l'oggetto "image"

			while(true){
				is.ignore();
				is.ignore();
				while (true)
				{
					is.get(c);
					if (c == '{')
						break;
					elems_string.push_back(c);
				
				}
				if (elems_string != "image") {
					cout << elems_string << " :";

					bitreader br(is);

					string elem_types;
					uint8_t index = 0;
					while (true)
					{
						index = br(8);
						if (index == 105) {
							uint8_t count = br(8);

							for(size_t i =0; i < count; i++)
								elem_types.push_back(br(8));

							cout << elem_types << ',';

							if (is.peek() == '[') {
								// siamo dentro ad una lista che non vogliamo mostrare a video 
								while (true) {
									index = br(8);
									if (index == 105) {
										uint8_t count = br(8);
										br(count * 8);
										break;
									}
								}
							}
							else
							{
								is.ignore(); // perche' c'e il valore che non e' interessante
								is.ignore();
							}
							
							elem_types.clear();
						}
						if (index == '}') {
							// siamo arrivati in un nuovo elemento
							cout << endl;
							break;
						}
						
						
					}
					//while(is.get() != '}'){}
					elems_string.clear();
					continue;
				}
				break;
			}
			cout << "image :";
			is.ignore();
			is.ignore();
			if (is.get() != 'x')
				exit(EXIT_FAILURE);
			cout << "x,";
			is.ignore();

			x_ = is.get();

			is.ignore();
			is.ignore();

			if (is.get() != 'y')
				exit(EXIT_FAILURE);
			cout << "y,";
			is.ignore();
			y_ = is.get();

			is.ignore();
			is.ignore();

			string width;
			while (true)
			{
				is.get(c);
				width.push_back(c);

				if (width == "width")
					break;

			}
			cout << width << ',';
			is.ignore();

			W_ = is.get();

			is.ignore();
			is.ignore();

			string height;
			while (true)
			{
				is.get(c);
				height.push_back(c);
				if (height == "height")
					break;

			}
			cout << height << ',';
			is.ignore();

			H_ = is.get();

			is.ignore();
			is.ignore();

			string data_s;
			while (true)
			{
				is.get(c);
				data_s.push_back(c);
				if (data_s == "data")
					break;

			}

			cout << data_s << ',' <<endl;

			v.resize(W_, H_);
			is.ignore();
			is.ignore();
			is.ignore();
			is.ignore();
			is.ignore();
			is.ignore();
			is.ignore();

			is.read(v.data(), v.data_size());
			
			imgs_.push_back(v);
			}
		}

		auto GetImage(){ return imgs_; }
};

struct canvas : public json {
	
	vec3b backrgb_;

	canvas(ifstream& is) :  json(is) {
		// proviamo a leggere in campo canvas
		while (true)
		{
			if (!is)
				break;
			is.ignore();
			is.ignore();
			is.ignore();
			string canvas_name;
			char c;
			while (true)
			{
				is.get(c);
				if (c == '{')
					break;
				canvas_name.push_back(c);
			}

			if (canvas_name != "canvas") {
				cout << "error in take the obj file" << endl;
				exit(EXIT_FAILURE);
			}

			is.ignore();
			is.ignore();
			string width;
			while (true)
			{
				is.get(c);
				width.push_back(c);

				if (width == "width")
					break;

			}
			is.ignore();

			W_ = is.get();



			is.ignore();
			is.ignore();

			string height;
			while (true)
			{
				is.get(c);
				height.push_back(c);
				if (height == "height")
					break;

			}
			is.ignore();

			H_ = is.get();

			is.ignore();
			is.ignore();

			string background;
			while (true)
			{
				is.get(c);
				if (c == '[')
					break;
				background.push_back(c);
			}
			if (background != "background") {
				exit(EXIT_FAILURE);
			}

			is.ignore();
			is.ignore();
			is.ignore();
			is.ignore();
			is.ignore();

			// inseriamo i colori
			backrgb_[0] = is.get();
			backrgb_[1] = is.get();
			backrgb_[2] = is.get();

			break;
		}
	}
};



int convert(const string& sInput, const string& sOutput) {

	// Dal file UBJ devo estrarre le informazioni e creare il canvas

	ifstream is(sInput, ios::binary);
	canvas c_par(is);

	elements elem(is);

	if (!writeP6("image1.ppm", elem.GetImage()[0]))
		return EXIT_FAILURE;

	unsigned w = c_par.W_; // TODO : modificare
	unsigned h = c_par.H_; // TODO : modificare

	image<vec3b> img(w, h);

	// per ogni elemento dell'img mettiamo il background:
	for (size_t r = 0; r < img.height(); r++) {
		for (size_t c = 0; c < img.width(); c++) {
			img(r, c)[0] = c_par.backrgb_[0];
			img(r, c)[1] = c_par.backrgb_[1];
			img(r, c)[2] = c_par.backrgb_[2];
		}
	}

	// e poi inseriamo l'img
	paste(img, elem.GetImage()[0], elem.x_, elem.y_);


	

	// Dal file UBJ devo estrarre le informazioni sulle immagini da incollare su img 

	// Output in formato PPM
	if (!writeP6(sOutput, img))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

	// TODO : gestire la linea di comando

	string sInput = "caso01.ubj";
	string sOutput = "caso01.ppm";

	return convert(sInput, sOutput);
}