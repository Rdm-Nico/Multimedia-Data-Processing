#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

// Third version of the @main.cpp item but with out malloc, calloc and realloc and use of Templates 

// Sup di dover leggere dei double 

int cmpfunc(const void* a, const void* b)
{
    int32_t x = *(int32_t*)a;
    int32_t y = *(int32_t*)b;
    if (x < y) {
        return -1;
    }
    else if (x > y) {
        return 1;
    }
    else {
        return 0;
    }
}
// creare  variabili generiche
// Inizializzare prima del nome della classe !!!
template<typename T>

// e poi  si stutiesci T con double 

struct vector {
    T* nums_;
    int n_;
    int capacity_; 

    vector() { 
        printf("defalut costructor\n");
        nums_ = NULL;
        n_ = 0;
        capacity_ = 0;
    }
    vector(int initial_size) { 
        printf("defalut costructor with int\n");
        nums_ = (double*calloc(initial_size, sizeof(double));
        n_ = initial_size;
        capacity_ = initial_size;
    }

    vector(const vector& other) {
        printf("copy costructor\n");
        n_ = other.n_;
        capacity_ = other.capacity_;
        nums_ = (double*)malloc(capacity_* sizeof(double));
        
        for (size_t i = 0; i < n_; i++)
        {
            nums_[i] = other.nums_[i];
            
        }
    }
    vector(vector &&other) { // creare un copy costructor per variabili che saprai che poi verranno distrutte
        printf("copy costructor\n");
        n_ = other.n_;
        capacity_ = other.capacity_;
        nums_ = (double*)malloc(capacity_ * sizeof(double));

        for (size_t i = 0; i < n_; i++)
        {
            nums_[i] = other.nums_[i];

        }
    }

    vector&  operator=(const vector &other) { 
        printf("copy assignment\n");
        if (this != &other) 
        {
            if (capacity_ < other.n_) 
            {
                
                free(nums_);
            
                capacity_ = other.capacity_;
                nums_ = (double*)malloc(capacity_ * sizeof(double));
            }
            
            n_ = other.n_;
            
            for (size_t i = 0; i < n_; i++)
            {
                nums_[i] = other.nums_[i];
            }
        }
        return *this; 
    }


    // other è un r-value reference 
    vector& operator=(vector &&other) { 
        printf(" move assignment\n"); // quando il compilatore verrà subito distrutto, lo tratta come un tipo particolare ( che sta per morire ) --> indicao indicato con '&&' --> non c'è bisogno del 'const' 
        if (this != &other)
        {
            if (capacity_ < other.n_)
            {

                free(nums_);

                capacity_ = other.capacity_;
                nums_ = (double*)malloc(capacity_ * sizeof(double));
            }

            n_ = other.n_;

            for (size_t i = 0; i < n_; i++)
            {
                nums_[i] = other.nums_[i];
            }
        }
        return *this;
    }


    ~vector() { 
        printf("destructor\n");
        delete[] nums_;
    }
    void push_back(const T& num) { // utilizzo cost & -> per evitare una possibile copia del dato che gli sto passando 
        if (n_ == capacity_) {
            capacity_ = (capacity_ == 0 ? 1 : capacity_ * 2);
            auto* tmp = new double[capacity_]; // auto non è un tipo generico: se definite delle variabili e li inizzializzate --> utilizza il tipo dell' inizzializzato. Questo è utile per evitare di scrivere tipi molto lunghi

            // questo è comodo con i template. E' una cosa che succedere a tempo di compilazione 
            if (tmp == NULL) {
                printf("Error: failed to allocate memory.\n");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < n_; ++i) // guardare la dif tra ++i e i++
            {
                tmp[i] = nums_[i];
            }
            

            delete[] nums_;
            nums_ = tmp;
        }
        nums_[n_] = num;
        n_++;
    }
    
    int size() const {
        return n_;
    }
    T at(int i) const { 
        assert(i >= 0 && i < n_);
        return nums_[i];
    }
    T&  operator[](int i) { 
        return nums_[i];
    }
    const T& operator[](int i)  const{ 
        return nums_[i];
    }
};




void scrivi(FILE *F, const vector& v) {
    for (int i = 0; i < v.size(); i++) {
        fprintf(fout, "%f\n", v[i])); 
    }
}

struct numero
{
    int val_;
  
    numero() {
        val_ = 7;
    }
};


struct esempio
{
    int val_;
    esempio(int val = 0) : val_(val) {} // usare il costruttore di default 
};
vector<esempio> crea_elementi() {
    vector<esempio> out;
    out.push_back(6);
    out.push_back(-9);
    out.push_back(23);
    return out;
}// quando avete un oggetto che sapete che poi verrà distrutto --> bisogna RUBARE i dati , e non una deep copy ( SI CHIAMA  MOVE)

int main(int argc, char* argv[])
{
     
    vector<esempio> mio; // RVO = return vector optimization
    
    mio = crea_elementi();
    // ricorda: se ritornate un oggetto, questo deve stare da qualche parte, tutte le volte  che passate parametri ad una funzione, ricordate che questi vengono copiati
    // ricorda questo e appena è possibile utilizza reference 

    



    if (argc != 3) {
        printf("Usage: sort_int <filein.txt> <fileout.txt>\n");
        return 1;
    }

    FILE* fin = fopen(argv[1], "r");
    if (fin == NULL) {
        printf("Error opening input file.\n");
        return 1;
    }

    FILE* fout = fopen(argv[2], "w");
    if (fout == NULL) {
        printf("Error opening output file.\n");
        fclose(fin);
        return 1;
    }

    vector <double> v; // vector di double 
    vector <int> v; // vector di int 
    while (1) {
        double num;
        if (fscanf(fin, "%lf", &num) == 1) {
            v.push_back(num);
        }
        else if (feof(fin)) {
            break;
        }
        else {
            printf("Warning: incorrect data in input file.\n");
            break;
        }
    }
    qsort(&v[0], , sizeof(int32_t), cmpfunc);
    

   
    vector y;
    y = v;
     


    vector z;
    z = y = v;

    v.sort();


    scrivi(fout, v);

    fclose(fin);
    fclose(fout);

    return 0;
}