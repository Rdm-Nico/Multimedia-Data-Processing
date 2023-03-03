#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

// C++ version of the aassignment By the Prof  & ChatGPT

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

    void constructor() {
        nums_ = NULL;
        n_ = 0;
        capacity_ = 0;
    }
    void destructor() {
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
    int32_t at(int i) const {
        assert(i >= 0 && i < n_);
        return nums_[i];
    }
};


int main(int argc, char* argv[])
{
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
    v.constructor();

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

    v.sort();

    for (int i = 0; i < v.size(); i++) {
        fprintf(fout, "%" PRId32 "\n", v.at(i));
    }

    v.destructor();
    fclose(fin);
    fclose(fout);

    return 0;
}