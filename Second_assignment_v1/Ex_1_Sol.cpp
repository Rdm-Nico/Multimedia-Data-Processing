#include <iostream> 
#include <fstream> 
#include<cstdint>
#include <iomanip> // manipolatori di output
#include <format>
#include <unordered_map> // mappa non ordinata  che tiene traccia dei dati ( sono più veloci se non si tiene conto dell'ordine dei dati) --> sono hash map
#include<map> // invece di mappa non ordinate --> usare gli alberi binari
#include<array> // il vettore più veloce di tutti , però devi impostare la capienza necessaria  
#include<algorithm>

template<typename T, typename CountT = uint32_t> 

struct freq
{
    std::unordered_map<T, CountT> count_;

    void operator()(const T& val) {
        ++count_[val];
    }

    auto begin() {return count_.begin();}
    auto end() {return count_.end();}

    auto begin() {return count_.cbegin();} // cbegin = const begin 
    auto end() {return count_.cend();} 

};


// Sol of the first example
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: sort_int <filein.txt> <fileout.txt>\n"; // operatore insert su uno stream, questo produce la printf( in pratica)
        return 1;
    }

     
    std:: ifstream is(argv[1], ios:: binary);
    std::ofstream out(argv[2]);

    if (!is || !out)
    {
        std::cout << "Error opening input file.\n";
        return 1;
    }
    
    freq<int> count_3;

    map<int, uint32_t> count; // primo parametro è il tipo della chiave, il secondo il valore che vogliamo salvare 

    array<uint32_t, 256> count_1; // NON INIZZIALIZZATI
    fill(count_1.begin(),count_1.end(), 0); // ecco un modo per inizzializzarli con la Lib algorithm , meglio di una for 

    //uint32_t count[256] = {0}; // ma se volessimo contare la sequenza di stringhe di testo ? con una mappa 
    while (1)
    {
        uint8_t num; // posso utilizzarlo grazie alla libreria <cstdint>
        num = is.get();

        if (num == EOF)
        {
            break;
        }
        ++count[num]; // come il dict di py --> se non trova la chiave , la crea 
    }

    
    // per scorrere una mappa usiamo gli iteratori
    for(const auto& x: count){ // può essere usato anche con le tue stesse classi 
        os << format("{:02X}\t{}\n", x.first, x.second); 
    }
    
    for (int i = 0; i < 256; ++i) // quando si usano gli array è più comodo fare la versione classica  ( con  i  < count_1.size(); ++i)
    {
        if (count[i] > 0) {
            out << std::setbase(16) << std::setw(2)<< std::setfill('0') << std::uppercase << i << '/t' << std::setbase(10) << count[i] << '/n';
            
            // Sintassi pi� compatta 
            // os << format("{:02X}\t{}\n", i, count[i]); // c'� da usare il Languge C++ 20
        }

    }

    return 0;
}