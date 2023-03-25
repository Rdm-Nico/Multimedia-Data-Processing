export module bit;


import <fstream>;


 
template<typename T>
std::istream& raw_read(std::istream& in, T& val, size_t size = sizeof(T)) {
    return in.read(reinterpret_cast<char*>(&val), size);
}

export class bitreader {
    std::istream& is_;
    uint8_t buffer_ = 0; // buffer 
    int n_ = 0; // numeri di bit segnati che ci sono nel byte attuale

public:
    bitreader(std::istream& is) : is_(is) {}

    uint32_t read_bit() {
        if (n_ == 0) {
            raw_read(is_, buffer_);
            n_ = 8;
        }
        n_--; // decrementiamo 
        return(buffer_ >> --n_) & 1; //
    }


    uint32_t operator()(uint32_t n) { // fa la lettura di n bit
        uint32_t u = 0; // variabile temporanea

        while (n-- > 0)
        {
            u = u * 2 + read_bit();
        }

         for(int i = 0; i < n; i++){
             u = u* 2 + read_bit();
         }
        return u;
    }

    bool fail() const { // ritorna true se c'è un errore deve essere const perchè non modifica lo stato dell'oggetto
        return is_.fail();
    }
    explicit operator bool() const { // ritorna true se non c'è un errore
        return !is_.fail();
    }
};

template<typename T>
std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
}


export class bitwriter {
	uint8_t buffer_;
	int n_ = 0; //  numero di elementi nel buffer 
	std::ostream& os_; // memorizziamo lo stream con una reference  e lui si occupa di gestire la memoria 
	// � un riferimento perch� non vogliamo copiare lo stream, vogliamo solo accedere a quello che � gi� stato creato
	// quando verr� distrutto il bitwriter lo stream verr� distrutto anche lui

	std::ostream& write_bit(uint8_t bit) {
		buffer_ = buffer_ * 2 + bit;
		n_++;
		if (n_ == 8) { // se n � 8 scriviamo il buffer
			raw_write(os_, buffer_);
			n_ = 0; // resetto il buffer
		}
		return os_;
	}
	// rendiamo write_bit privata perch� non vogliamo che venga chiamata dall'esterno
public:

	bitwriter(std::ostream& os) : os_(os) {}// non esiste un costruttore di defalut per un output stream

	~bitwriter() {
		flush(); // quando distruggiamo il bitwriter facciamo il flush perch� potrebbero esserci dei bit rimasti nel buffer
		// prima verr� distrutto il bitwriter e poi l'output stream
	}

	std::ostream& flush(uint32_t bit = 0) {
		while (n_ > 0) {
			write_bit(bit);
		}
		return os_;
	}
	std::ostream& write(uint32_t u, uint8_t n) {
		// while (n_ --> 0) // prendo n lo decremento e lo confronto con 0
		// {
		//     write_bit(u >> n_);
		// }

		for (int i = n - 1; i >= 0; --i) {
			uint32_t cure_bit = (u >> i) & 1; // prendiamo il bit i-esimo
			write_bit(cure_bit);
		}
		return os_;
	}

	std::ostream& operator()(uint32_t u, uint8_t n) { // alternativa a chiamare la  write
		return write(u, n);
	}

};
