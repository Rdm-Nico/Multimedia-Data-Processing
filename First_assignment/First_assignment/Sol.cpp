    // librerie per la lettura e scrittura su file 
#include <iostream> // per la scrittura nello stream
#include <fstream> // per i file
#include<algorithm>
#include<iterator>



#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

// 5 version of the main with the correct c++ Sol


// EXCURSUS: explicit  per evitare che delle cose avvengano in modo implicito 


// i file sono inclusi negli stream, un livello superiore alla printf
int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: sort_int <filein.txt> <fileout.txt>\n"; // operatore insert su uno stream, questo produce la printf( in pratica)
        return 1;
    }

    std::ifstream  is(argv[1]); // stesso principio della fopen per i file di lettura 


   
    if (is.fail()) { // si può scrivere meglio: if(!is)
        std::cout << "Error opening input file.\n";
        return 1;
    }
    std::ofstream  out(argv[2]);
    if (!out) {
        std::cout << "Error opening output file.\n";
        // non serve più chiudere i file , basta la return 
        return 1;
    }
    std::vector<double> v;


    while (1) { // è sbagliato SCRIVERE: while(!is.eof()) perchè l'ultimo valore viene duplicato dentro al ciclo 
            
                
        double num;
        is >> num; // equivalente di fscanf ( con la differenze che salta sempre i whitespace, anche quando si stanno leggendo i char 

        
        // usare gli itearoiterator per leggere i file 
        std::istream_iterator<double> start(is);
        std::istream_iterator<double> stop;
        /*for (auto it = start; it != stop; ++it)
        {
            v.pushback(*it);
        }*/

        // versione alternativa con gli adapter
        //std::copy(start, stop, std::back_inserter(v));

        // altra versione 
        std::vector<double> v(start, stop);


        // se fallisce is >> num 
        // per sapere se è andato bene bisogna chiedre allo stream se lo stato va bene 

        if (is.good()) // opp if(is)
        {
            v.pusk_back(num);
        }
        else if (is.eof()) {
            break;
        }
        else {
            std::cout << "Warning: incorrect data in input file.\n";
            break;
        }
    }

    // per la sort utilizziamo 
    //std::sort() prende due  iteratori( li hanno creati loro)
    std::sort(v.begin(), v.end()); // però è brutta --> da c++ 20 c'è il range 
    // la sort avviene tramite un confronto tramite l'operatore '<'


    // come si possono utilizzare gli iteratori
    /* versione classica 
    for (int i = 0; i < v.size(); ++i) {
        os << v[i];
        os << '\n';
        // si può scrivere anche una cosa del genere: os << v[i] << '\n' 
        */

    /* versione con iteratori classica ( che fa molto scifo)*/
    /*std::vector<double>::iterator start = v.begin(); // un eratore è una classe specifica 
    std::vector<double>::iterator stop = v.end();
    std::vector<double>::iterator it;

    for (it = start; it != stop; ++it) {
        os << *it << '\n'
    }*/

    /* versione con iteratori più moderna  */
    /*auto start = v.begin(); // un eratore è una classe specifica 
    auto stop = v.end();
    

    for (auto it = start; it != stop; ++it) {
        const auto& x = *it // con la reference x diviene una copia dei dati che vogliamo leggere ( senza modificare)
        os << x << '\n'
    }*/
    // visto che il contenitore è generico, può essere utilizzato con anche le liste, ecc..

    /* versione con iteratori ancora più moderna( con range-based for loop) */
    /*
    for (auto x :v) { // si può anche scrivere  sempre const auto & 
            os << x << '\n'
    }*/
    /* versione con la copy */

    // esistono acnhe altri algoritmi importanti nella libreria algorhitm
    std::copy(v.begin(), v.end(), std::ostream_iterator<double>(os, "\n"));
    // dato un range di cose -->legge ogni elemento è lo scrive in un output iterator
    // ostream_iterator --> prende in input uno stream e lo rende un iteratore, 


    
    return 0;
}