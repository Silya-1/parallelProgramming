#include <omp.h>
#include "iostream"
#include <sys/time.h>

double timer()
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	return (double)ts.tv_sec + 1e-6 * (double)ts.tv_usec;
}

struct Matrix {
private:
    int size;
    double* mat;
public:
    
    Matrix(int size) {
        this->size = size;
        mat = new double[size * size];
    }
    ~Matrix() {
            delete[] mat;
    }

    void transpose(){
        double* step = new double[size * size];
        for(auto i = 0; i < size; i++){
            for(auto j = 0; j < size; j++){
                step[j * size + i] = mat[i * size + j];
            }
        }
        delete[] mat;
        mat = step;
    }
    static void mul(Matrix& a, Matrix& b, Matrix& result) { 
        int N = result.getSize();
        b.transpose();
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                double sum = 0;
                for (int k = 0; k < N; k++) {
                    sum += a(x, k) * b(y, k);
                }
                result(x, y) = sum;
            }
        }
        b.transpose();
    }
    
    static void mul_omp(Matrix& a, Matrix& b, Matrix& result) { 
        const int N = result.getSize();
        b.transpose();
#pragma omp parallel for default(shared)
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                double sum = 0;
                for (int k = 0; k < N; k++) {
                    sum += a(x, k) * b(y, k);
                }
                result(x, y) = sum;
            }
        }
        b.transpose();
    }

    void operator = (Matrix a){
        for(auto i = 0; i < size; i++){
            for(auto j = 0; j< size; j++)
                mat[i*size + j] = a(i,j);
        }
    }
    double operator () (int x, int y)const{
        return mat[x * size + y];
    }

    double& operator () (int x, int y) {
        return mat[x * size + y];
    }

    friend Matrix operator + (Matrix& a, Matrix& b) {
        int size = b.getSize();
        Matrix res(a.size);
        for (auto i = 0; i < size; i++) {
            for(auto j = 0; j < size; j++)
                res(i,j) = a(i,j) + b(i,j);
        }
        return res;
    }

    friend Matrix operator - (Matrix& a,  Matrix& b) {
        int size = b.getSize();
        Matrix res(a.size);
        for (auto i = 0; i < size; i++) {
            for(auto j = 0; j < size; j++)
                res(i,j) = a(i,j) - b(i,j);
        }
        return res;
    }
    
    int getSize() {
        return size;
    }

    void print(){
        for (auto i = 0; i < size; i++){
            for(auto j = 0; j < size; j++){
                std::cout<<mat[i * size + j] << " ";
            }
            std::cout<<'\n';
        }
    }

    void static strassen(Matrix& a, Matrix& b, Matrix& result) {
        const int size = a.getSize();
        if (size <= 64) {
            Matrix::mul_omp(a, b, result);
            return;
        }
        const int halfSize = size / 2;
        Matrix a11 = Matrix(halfSize);
        Matrix a12 = Matrix(halfSize);
        Matrix a21 = Matrix(halfSize);
        Matrix a22 = Matrix(halfSize);
        Matrix b11 = Matrix(halfSize);
        Matrix b12 = Matrix(halfSize);
        Matrix b21 = Matrix(halfSize);
        Matrix b22 = Matrix(halfSize);
        Matrix r11 = Matrix(halfSize);
        Matrix r12 = Matrix(halfSize);
        Matrix r21 = Matrix(halfSize);
        Matrix r22 = Matrix(halfSize);
        Matrix p1 = Matrix(halfSize);
        Matrix p2 = Matrix(halfSize);
        Matrix p3 = Matrix(halfSize);
        Matrix p4 = Matrix(halfSize);
        Matrix p5 = Matrix(halfSize);
        Matrix p6 = Matrix(halfSize);
        Matrix p7 = Matrix(halfSize);
#pragma omp parallel for default(shared)
        for (int x = 0; x < halfSize; x++) {
            for (int y = 0; y < halfSize; y++) {
                a11(x, y) = a(x, y);
                a12(x, y) = a(x, y + halfSize);
                a21(x, y) = a(x + halfSize, y);
                a22(x, y) = a(x + halfSize, y + halfSize);
                b11(x, y) = b(x, y);
                b12(x, y) = b(x, y + halfSize);
                b21(x, y) = b(x + halfSize, y);
                b22(x, y) = b(x + halfSize, y + halfSize);
            }
        }
#pragma omp parallel default(shared)
#pragma omp single
        {
#pragma omp task 
            {
                Matrix aResult = a11 +  a22; 
                Matrix bResult = b11 + b22;
                strassen(aResult, bResult, p1);
            }
#pragma omp task
            {
                Matrix aResult = a21 + a22; 
                strassen(aResult, b11, p2); 
            }
#pragma omp task
            {
                Matrix bResult = b12 - b22; 
                strassen(a11, bResult, p3); 
            }
#pragma omp task
            {
                Matrix bResult = b21 - b11;
                strassen(a22, bResult, p4);
            }
#pragma omp task
            {
                Matrix aResult = Matrix(halfSize);
                aResult = a11 + a12; 
                strassen(aResult, b22, p5);   
            }
#pragma omp task
            {
                Matrix aResult = a21 - a11;
                Matrix bResult = b11 +  b12;
                strassen(aResult, bResult, p6); 
            }
#pragma omp task
            {
                Matrix aResult  = a12 -  a22;
                Matrix bResult = b21 +  b22;
                strassen(aResult, bResult, p7); 
            }
#pragma omp taskwait
#pragma omp task 
            {
                r12 = p3 + p5;
            }
#pragma omp task 
            {            
                r21 = p2 + p4;
            }         
#pragma omp task 
            {
                Matrix aResult = p1 + p4;
                Matrix bResult = aResult + p7;
                r11 = bResult - p5; 
            }
#pragma omp task 
            {
                Matrix aResult = p1 +  p3;
                Matrix bResult = aResult + p6;
                r22 = bResult - p2; 
            }
            
            #pragma omp taskwait
        }
#pragma omp parallel for default(shared)
        for (int x = 0; x < halfSize; x++) {
            for (int y = 0; y < halfSize; y++) {
                result(x, y) = r11(x, y);
                result(x, y + halfSize) = r12(x, y);
                result(x + halfSize, y) = r21(x, y);
                result(x + halfSize, y + halfSize) = r22(x, y);
            }
        }
    }

};

int main(int argc, char const *argv[])
{
    char *endptr = nullptr;
    int num = strtol(argv[1], &endptr, 10);
    srand (1);
    Matrix a(num);
    Matrix b(num);
    Matrix c(num);
    Matrix d(num);
    for(auto i = 0;i < num; i++){
        for(auto j = 0; j < num; j++){
            a(i,j) = rand() % 10 + 1;
            b(i, j) = rand() % 10 + 1;
            c(i,j) = 0;
        }
    }


    // std::cout<<d.getSize();
    // a.print();
    // std::cout<<'\n';
    // b.print();
    // std::cout<<'\n';
    // d.print();
    // std::cout<<'\n';
    double t = timer();
    Matrix::mul(a,b, c);
    t = timer() - t;
    std::cout<<"TOTAL TIME  =  "<< t<<std::endl;
    // c.print();

    omp_set_num_threads(strtol(argv[2], &endptr, 10)); 
    t = timer();
    Matrix::mul_omp(a,b, c);
    t = timer() - t;
    std::cout<<"TOTAL TIME  =  "<< t<<std::endl;
    t = timer();
    Matrix::strassen(a,b,d);
    t = timer() - t;
    std::cout<<"TOTAL TIME  =  "<< t<<std::endl;

    // for(auto i = 0; i < a.getSize(); i ++){
    //     for(auto j = 0; j < a.getSize(); j++){
    //         if(c(i,j) != d(i,j))
    //             std::cout<<"ERROR"<<std::endl;
    //     }
    // }
    return 0;
}



