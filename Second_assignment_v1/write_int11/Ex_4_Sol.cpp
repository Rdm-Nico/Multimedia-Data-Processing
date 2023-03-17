#include<iostream> 
#include<fstream>
#include<cstdint>
#include<vector>
#include<iterator>

//  Example 4 of the assignments with solution

// facciamo una classe ci ci gestisce uno stream c++ a bit 
class bitwriter{
    uint8_t buffer_;
    int n_ = 0; //  numero di elementi nel buffer 
    std::ostream& os_; // memorizziamo lo stream con una reference  e lui si occupa di gestire la memoria 
    // è un riferimento perchè non vogliamo copiare lo stream, vogliamo solo accedere a quello che è già stato creato
    // quando verrà distrutto il bitwriter lo stream verrà distrutto anche lui

    std::ostream &write_bit(uint8_t bit){
        buffer_ = buffer_ * 2 + bit; 
        n_++;
        if(n_ == 8){ // se n è 8 scriviamo il buffer
            raw_write(os_, buffer_);
            n_ = 0; // resetto il buffer
        }
        return os_;
    }
    // rendiamo write_bit privata perchè non vogliamo che venga chiamata dall'esterno
    public:

    bitwriter(std::ostream& os): os_(os){}// non esiste un costruttore di defalut per un output stream

    ~bitwriter(){
        flush(); // quando distruggiamo il bitwriter facciamo il flush perchè potrebbero esserci dei bit rimasti nel buffer
        // prima verrà distrutto il bitwriter e poi l'output stream
    }

    std::ostream &flush(uint32_t bit =0 ){
        while(n_ > 0){
            write_bit(bit);
        }
        return os_;
    }
    std::ostream &write(uint32_t u, uint8_t n){
        // while (n_ --> 0) // prendo n lo decremento e lo confronto con 0
        // {
        //     write_bit(u >> n_);
        // }
        
        for(int i= n-1; i >=0; --i){
            uint32_t cure_bit = (u >> i )& 1; // prendiamo il bit i-esimo
            write_bit(cure_bit);
        }
        return os_;
    }

    std::ostream &operator()(uint32_t u, uint8_t n){ // alternativa a chiamare la  write
        return write(u,n);
    }
    
};

template<typename T>
std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T)) {
    return os.write(reinterpret_cast<const char*>(&val), size);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: sort_int <filein.txt> <fileout.txt>\n"; 
        return 1;
    }

    // open the files
    std::ifstream is(argv[1]);
    std::ofstream out(argv[2], std::ios::binary);
    // create the iterator 
    std::istream_iterator<int32_t> is_start(is);
    std::istream_iterator<int32_t> is_stop;

    // init the vector with the all values 
    std::vector<int32_t> v(is_start, is_stop);
    if (!is || !out)
    {
        std::cout << "Error opening input file.\n";
        return 1;
    }


   

    // creimo un buffer
    uint8_t buffer;
    int n = 0;
    /*
    for (const auto &x : v)
    {
        for(int i= 10; i >=0; --i){
            uint32_t cure_bit = (x >> i )& 1; // prendiamo il bit i-esimo
            // aggiungiamo il bit al buffer sposandolo a sinistra
            buffer = buffer * 2 + cure_bit; 
            n++;
            if(n == 8){ // se n è 8 scriviamo il buffer
                raw_write(out, buffer);
                n = 0; // resetto il buffer
            }
        }
    }
    while(n > 0){
            uint8_t cure_bit = 0; // bit corrente
            // aggiungiamo il bit al buffer sposandolo a sinistra
            buffer = buffer * 2 + cure_bit; 
            n++;
            if(n == 8){ // se n è 8 scriviamo il buffer
                raw_write(out, buffer);
                n = 0; // resetto il buffer
            }
        }
     */
    bitwriter bw(out); 
    for (const auto &x : v)
    {
        bw(x, 11);
    }
    
    return 0; // distrugge prima il  bitwriter e poi lo stream
}