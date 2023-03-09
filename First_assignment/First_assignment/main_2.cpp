#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

// Second version of the @main.cpp item but with class vector and shallow copy 

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

struct vector {
    int32_t* nums_;
    int n_;
    int capacity_; // capacity of nums array

    vector() { // costructor of the class = with the same name  ( costruttore di defalut)
        nums_ = NULL;
        n_ = 0;
        capacity_ = 0;
    }
    vector(int initial_size) { // another costructor but with a inizialize value of capacity
        nums_ = (int32_t*)calloc(initial_size, sizeof(int32_t));
        n_ = initial_size;
        capacity_ = initial_size;
    }

    vector(const vector& other) {
        // una reference che non constene di modificare other
        // per passare le stringe ad una funzione non usate strig, cost String& nome
        n_ = other.n_;
        capacity_ = other.capacity_;
        nums_ = (int32_t *)malloc(capacity_* sizeof(int32_t));
        // check della allocazione di memoria 
        for (size_t i = 0; i < n_; i++)
        {
            nums_[i] = other.nums_[i];
            // la memcpy andava più forte perchè: usare accelerazione hardware più forti
        }
    }

    vector& operator=(vector other) { // copy-and-swap idiom
        swap(*this, other);
        return *this;
    }
    friend void swap(vector& left, vector& right) {
        using std::swap;
        swap(left.n_, right.n_);
        swap(left.capacity_, right.capacity_);
        swap(left.nums_, right.nums_);
    }
    ~vector() { // distructor 
        free(nums_);
    }
    void push_back(int32_t num) {
        if (n_ == capacity_) {
            capacity_ = (capacity_ == 0 ? 1 : capacity_ * 2);
            nums_ = (int32_t*)realloc(nums_, capacity_ * sizeof(int32_t));
            if (nums_ == NULL) {
                printf("Error: failed to allocate memory.\n");
                exit(EXIT_FAILURE);
            }
        }
        nums_[n_] = num;
        n_++;
    }
    void sort() {
        qsort(nums_, n_, sizeof(int32_t), cmpfunc);
    }
    int size() const {
        return n_;
    }
    int32_t at(int i) const { // ecco il nome speciale, definito dal linguaggio per sostituire at(i)
        assert(i >= 0 && i < n_);
        return nums_[i];
    }
    // bisogna fare l'overloading per la funzione: due versione dello stesso metodo, ma sono due funzioni diverse
    /*
        Una versione in cui non si può modificare il valore ritornato ( la seconda )
        Una versione in cui si può utilizzare il valore ritornato( la prima)
    */
    int32_t&  operator[](int i) { // ecco il nome speciale, definito dal linguaggio per sostituire at(i)
        return nums_[i];
    }
    const int32_t& operator[](int i)  const{ // ecco il nome speciale, definito dal linguaggio per sostituire at(i)
        return nums_[i];
    }
};

void raddoppia(int& val) {
    val *= 2;
}


void scrivi(FILE *F, const vector& v) {
    for (int i = 0; i < v.size(); i++) {
        fprintf(fout, "%" PRId32 "\n", v[i])); 
    }
}

struct numero
{
    int val_;
    //numero() : val_(7){}// quando volete iniziallizare gli attributi di una classe , lo fate prima
    numero() {
        val_ = 7;
    }
};

int main(int argc, char* argv[])
{
    /*numero* pn = (numero*)malloc(sizeof(numero));
    int q = pn->val_; // la malloc ritorna memoria e basta 
    // serve una nuova modalità di allocare memoria 
    Si utilizzerà:*/
    numero* pn = new  numero;
    int q = pn->val_;
    // non c'è la garbage collection --> si deve liberare la memoria 
    delete pn;

    // la sintassi per allocare più di un oggetto insieme:
    numero* pt = new  numero[10];
    
    delete[] pt;

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

    vector v;

    while (1) {
        int32_t num;
        if (fscanf(fin, "%" SCNd32, &num) == 1) {
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

    vector x = v; // FAIL! Shallow copy! 

    /*
        NON si può scrivere:
        vector y;
        x = v;
        PERCHE' ADESSO STIAMO ASSEGNANDO, NON PIU' INIZIALLIZANDO
        farà una shallow copy !!! 
    */
    vector y;
    y = v;
    /*
        A shallow copy of an object is a copy whose properties share the same references (point to the same underlying values)
        as those of the source object from which the copy was made.
        As a result, when you change either the source or the copy, 
        you may also cause the other object to change too — and so, 
        you may end up unintentionally causing changes to the source or copy that you don't expect.  
    */    
    /*NON SI PUO FARE v[0] = 5  , perchè deve essere una variabile --> essere un indirizzo di memoria, si inserisce una reference per farlo diventare un l-value*/


    vector z;
    z = y = v;

    v.sort();


    scrivi(fout, v);

    fclose(fin);
    fclose(fout);

    return 0;
}