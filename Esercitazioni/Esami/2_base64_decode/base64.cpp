#include "base64.h"


class base64 {
	unordered_map<uint8_t, uint8_t> Table1_;
	string input_;
	string decoded_;
	bool needsPadding_;
	uint8_t paddingNumber_;
	uint32_t buffer_;
	
	void create_table() {
		unordered_map<uint8_t, uint8_t> m;
		for (uint8_t i = 0; i <= 25; i++)
			m[i] = 65 + i;

		for (uint8_t i = 26; i <= 51; i++)
			m[i] = 71 + i;

		for (uint8_t i = 52; i <= 61; i++)
			m[i] = i - 52 + '0'; // perche' vogliamo il numero ascii non quello attuale '0' = 48

		m[62] = '+';
		m[63] = '/';

		for (auto e : m)
			Table1_[e.second] = e.first;
	}
public:
	base64(string input) : input_(input), buffer_(0), paddingNumber_(0) {
		create_table();
		needsPadding_ = input.length() % 4 != 0 ? true:false;
	}

	void cleanInput() {
		string clean;
		for (auto e : input_) {
			if (Table1_.count(e) == 1 || e == '=')
				clean.push_back(e);
		}
		input_ = clean;

	}

	void decode() {
		size_t triplets = 0;

		for (size_t i = 0; i < input_.size(); i += 4) {

			uint32_t val = 0;

			// 4 caratteri codificati
			array<uint8_t, 4> quad_base64 = { input_[i],input_[i + 1],input_[i + 2],input_[i + 3] };

			// 4 caratteri decodificati
			array<uint8_t, 4> quad_decoded_64;
			uint8_t v = 0;

			for (auto& e : quad_base64) {
				if (e == '=')
					break; // siamo arrivati alla fine
				quad_decoded_64[v] = Table1_.at(e); // avremo di ritorno il valore a 6 bit 

				v++;
			}

			// trasformiamo il valore da base 64 in base 10 e accumuliamo il risultato in val. adesso abbiamo 24 bits di 8*3 in val
			for (int j = 0, k = 3; j < 4; j++, k--) {
				if (quad_base64[j] == '=') {
					paddingNumber_ = 4 - j;
					break; 
				}
				val += quad_decoded_64[j] * pow(64, k);
			}
		

			// leggiamo 8 bit per 3 volte nel nostro buffer di "24" bit
			uint8_t bits = 24;
			for (uint8_t n = 0; n < 3 - paddingNumber_; n++) {
				uint8_t x = 0;
				bits -= 8; // tiriamo via 8 bits

				x |= (val >> bits);
				decoded_.push_back(x);
			}
		}

		triplets++;
	}


	string getDecoded() {
		return decoded_;
	}
};





string base64_decode(const std::string& input) {

	base64 b(input);

	b.cleanInput();
	b.decode();
	return b.getDecoded();
}

