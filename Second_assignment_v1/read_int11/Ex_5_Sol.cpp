#include<iostream> 
#include<fstream>
#include<cstdint>
#include<vector>
#include<iterator>

//  Example 5 of the assignments with solution


template<typename T>
std::istream& raw_read(std::istream& in, T& val, size_t size = sizeof(T)) {
    return in.read(reinterpret_cast<char*>(&val), size);
}

// facciamo una classe ci ci gestisce uno stream c++ a bit 
class bitreader {
    std::istream& is_; 
    uint8_t buffer_ = 0; // buffer 
    int n_ = 0; // numeri di bit segnati che ci sono nel byte attuale

public:
        bitreader(std::istream& is) : is_(is) {}

        uint32_t read_bit(){
            if(n_ == 0){
                raw_read(is_, buffer_);
                n_ = 8;
            }
            n_--; // decrementiamo perchè abbiamo letto un bit e lo abbiamo tolto dal buffer
            return(buffer_ >> --n_) & 1; // ritorniamo il bit che ci interessa
        }


        uint32_t operator()(uint32_t n){ // fa la lettura di n bit
            uint32_t u = 0; // variabile temporanea

            while (n --> 0)
            {
                u = u * 2 + read_bit();
            }
            
            // for(int i = 0; i < n; i++){
            //     u = u* 2 + read_bit();
            // }
            return u;
        }

        bool fail() const { // ritorna true se c'è un errore deve essere const perchè non modifica lo stato dell'oggetto
            return is_.fail();
        }
        explicit operator bool() const { // ritorna true se non c'è un errore
            return !is_.fail();
        }
};



int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: sort_int <filein.txt> <fileout.txt>\n"; 
        return 1;
    }

    // open the files
    std::ifstream is(argv[1], std::ios::binary);
    std::ofstream out(argv[2]);

    if (!is || !out)
    {
        std::cout << "Error opening input or output  file.\n";
        return 1;
    }


    bitreader br(is); // crea il bitreader e lo distrugge alla fine del main)
    

    

    while (1)
    {
        uint32_t num = br(11);
        if(!br)  // verifichiamo se c'è un errore
            break;

        if(num >= 1024){ // serve per fare il complemento a 2 
            num -= 2048;
        }
        out << num << '\n'; // se no si può usare i bit iterator
    }
    
   


    
    return 0; 
}