#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include <bitset>

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

    if (!is || !out)
    {
        std::cout << "Error int the files\n";
        return 1;
    }
    // create the iterator 
    std::istream_iterator<int32_t> is_start(is);
    std::istream_iterator<int32_t> is_stop;

    // init the vector with the all values 
    std::vector<int32_t> v(is_start, is_stop);

    std::vector<std::bitset<11> > b; 

    for (size_t t = 0; t < v.size(); ++t)
    {
            std::bitset<11> b1(v[t]);
            b.push_back(b1);
    }

    for (size_t t = 0; t < v.size(); ++t)
    {
        std::cout << v[t] << '\t' << b[t] << '\n';
    }
    
    uint8_t buf = 0;
    uint8_t bit;
    int num = 0; // numeri di bit segnati che ci sono nel byte attuale  
    for (size_t t = 0; t < b.size(); t++)
    {
        size_t u = b[t].size()-1;
        while (1)
        {
            bit = b[t][u];
            buf = buf * 2 + bit;
            // incrementiamo num
            num++;

            if (num == 8)
            {
                // stampiamo il valore  nel file
                raw_write(out,buf);
                // inseriamo i bit rimasti
                num = 0;
                buf = 0;
                if(u != 0)
                    u--;
                else
                {
                    break;
                }

            }
            else if( u == 0)
                break;
            else
            {
                // decrementiamo u
                u--;
            }
        }

    }

	return 0;
}
