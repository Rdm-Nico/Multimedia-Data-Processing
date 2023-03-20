//export module bit;
//
//
//import <fstream>
//
//
// 
//export class bitreader {
//    std::istream& is_;
//    uint8_t buffer_ = 0; // buffer 
//    int n_ = 0; // numeri di bit segnati che ci sono nel byte attuale
//
//public:
//    bitreader(std::istream& is) : is_(is) {}
//
//    uint32_t read_bit() {
//        if (n_ == 0) {
//            raw_read(is_, buffer_);
//            n_ = 8;
//        }
//        n_--; // decrementiamo 
//        return(buffer_ >> --n_) & 1; //
//    }
//
//
//    uint32_t operator()(uint32_t n) { // fa la lettura di n bit
//        uint32_t u = 0; // variabile temporanea
//
//        while (n-- > 0)
//        {
//            u = u * 2 + read_bit();
//        }
//
//         for(int i = 0; i < n; i++){
//             u = u* 2 + read_bit();
//         }
//        return u;
//    }
//
//    bool fail() const { // ritorna true se c'è un errore deve essere const perchè non modifica lo stato dell'oggetto
//        return is_.fail();
//    }
//    explicit operator bool() const { // ritorna true se non c'è un errore
//        return !is_.fail();
//    }
//};