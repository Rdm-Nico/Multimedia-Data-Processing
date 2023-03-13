#include<iostream> 
#include<fstream>
#include<cstdint>
#include<vector>
#include<iterator>

template<typename T>
std::istream& raw_read(std::istream& in, T& val, size_t size = sizeof(T)) {
    return in.read(reinterpret_cast<char*>(&val), size);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: sort_int <filein.txt> <fileout.txt>\n"; 
        return 1;
    }
    // open the file 
    std::ifstream is(argv[1], std::ios::binary);
    std::ofstream out(argv[2]);

    if (!is || !out)
    {
        std::cout << "Error opening input file.\n";
        return 1;
    }


   

    // init the vector with all the value 
    std::vector<int32_t> v;

    int32_t num;
    while (raw_read(is,num))
    {
        v.push_back(num);
    }

    for (const auto& x : v) {
        out << x << '\n';
    }
    return 0;
}