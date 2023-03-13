#include<iostream> 
#include<fstream>
#include<cstdint>
#include<vector>
#include<iterator>


template<typename T>
std::ostream& raw_write(std::ostream &os, const T& val, size_t size = sizeof(T)) {
    return os.write(reinterpret_cast<const char*>(&val),size);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: sort_int <filein.txt> <fileout.txt>\n"; // operatore insert su uno stream, questo produce la printf( in pratica)
        return 1;
    }
    // open the file 
    std::ifstream is(argv[1]);
    std::ofstream out(argv[2], std::ios::binary);

    if (!is || !out)
    {
        std::cout << "Error opening input file.\n";
        return 1;
    }


    // The most vexing parse -->  secondo il parse del c++ non è un costruttore ma si tratta di un prototipo di una funzione 

    std::istream_iterator<int32_t> is_start(is);
    std::istream_iterator<int32_t> is_stop;

    // init the vector with all the value 
    std::vector<int32_t> v(is_start, is_stop);

    
    // type cast in modo non standard 
    int a = 7;
    double b = static_cast<double>(a) / 10; // invoca la conversione da un tipo ad un altro --> Cast statico
    
    const char s[] = "Prova";

    const char *p = s;
    //char *q = static_cast<char *>(s); // lo static cast non può cavare il cost 
    // ci vuole un altro cast 
    char* q = const_cast<char*>(s);
    
    //int32_t  x = 266;
    //int32_t* px = &x;
    //char* qx = reinterpret_cast<char*>(px); // un puntatore a int32_t non può essere dato ad un puntatore a char 


    // scrivere in bin
    int32_t  x = 266;
    raw_write(out, x); // Template deduction --> fa una deduzione del tipo d'ingresso


    for (const auto& x : v) {
        raw_write(out, x);
    }



    return 0;
}