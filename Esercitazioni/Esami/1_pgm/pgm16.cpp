#include "mat.h"
#include "pgm16.h"




int main(void) {

	mat<uint8_t> img;
	uint16_t maxvalue;
	load("frog_bin.pgm", img, maxvalue);

	return 0;
}