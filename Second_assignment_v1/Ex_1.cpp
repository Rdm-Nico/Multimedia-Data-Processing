#include <iostream> 
#include <fstream> 



class vector {
    private:
        int occ;
        uint8_t byte;
    public:
        vector() {
            occ = 0;
            byte = 0;
        }
};


int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: sort_int <filein.txt> <fileout.txt>\n"; // operatore insert su uno stream, questo produce la printf( in pratica)
        return 1;
    }

    // open input file bin 
    fstream in(argv[1], ios::in | ios::binary);

    if (!is)
    {
        std::cout << "Error opening input file.\n";
        return 1;
    }

    std::ofstream  out(argv[2]);
    if (!out) {
        std::cout << "Error opening output file.\n";
        // non serve più chiudere i file , basta la return 
        return 1;
    }

    vector data;
    while (1)
    {
        in.read((char *) )
    }




}