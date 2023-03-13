#include<iostream> 
#include<fstream>
#include<cstdint>
#include<vector>
#include<iterator>


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: sort_int <filein.txt> <fileout.txt>\n"; // operatore insert su uno stream, questo produce la printf( in pratica)
        return 1;
    }
    // open the file 
    std::ifstream is(argv[1]);
    std::ofstream out(argv[2]);

    if (!is || !out)
    {
        std::cout << "Error opening input file.\n";
        return 1;
    }
    

    // The most vexing parse -->  secondo il parse del c++ non è un costruttore ma si tratta di un prototipo di una funzione 

    std::istream_iterator<int32_t> is_start(is);
    std::istream_iterator<int32_t> is_stop;
    
    // init the vector with all the value 
    std::vector<int32_t> v(is_start,is_stop);




    return 0;
}