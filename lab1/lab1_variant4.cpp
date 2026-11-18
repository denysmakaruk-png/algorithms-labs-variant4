#include <iostream>
#include <chrono>
#include <cmath>
#include <random>
#include <iomanip>
#include <cassert>

template <typename T>
class CustomVector {
private:
    T* data_;
    size_t size_;
    size_t capacity_;

    void reallocate(size_t new_cap) {
        T* new_data = new T[new_cap]();
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[i];
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_cap;
    }

public:
    CustomVector() : data_(nullptr), size_(0), capacity_(0) {}

    explicit CustomVector(size_t count, const T& value = T())
        : data_(new T[count > 0 ? count : 1]), size_(count), capacity_(count > 0 ? count : 1) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = value;
        }
    }

    CustomVector(const CustomVector& other)
        : data_(new T[other.capacity_ > 0 ? other.capacity_ : 1]),
          size_(other.size_),
          capacity_(other.capacity_ > 0 ? other.capacity_ : 1) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    CustomVector& operator=(const CustomVector& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            capacity_ = other.capacity_ > 0 ? other.capacity_ : 1;
            data_ = new T[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }

    CustomVector(CustomVector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    CustomVector& operator=(CustomVector&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    ~CustomVector() {
        delete[] data_;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            reallocate(capacity_ == 0 ? 2 : capacity_ * 2);
        }
        data_[size_++] = value;
    }

    void resize(size_t new_size, const T& value = T()) {
        if (new_size > capacity_) {
            reallocate(new_size);
        }
        for (size_t i = size_; i < new_size; ++i) {
            data_[i] = value;
        }
        size_ = new_size;
    }

    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
};

template <typename T>
using Matrix2D = CustomVector<CustomVector<T>>;

template <typename T>
Matrix2D<T> createMatrix(size_t rows, size_t cols, T initVal = T()) {
    return Matrix2D<T>(rows, CustomVector<T>(cols, initVal));
}

template <typename T>
Matrix2D<T> addMatrix(const Matrix2D<T>& A, const Matrix2D<T>& B) {
    size_t n = A.size();
    size_t m = A[0].size();
    Matrix2D<T> C = createMatrix<T>(n, m, T());
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < m; ++j)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

template <typename T>
Matrix2D<T> subMatrix(const Matrix2D<T>& A, const Matrix2D<T>& B) {
    size_t n = A.size();
    size_t m = A[0].size();
    Matrix2D<T> C = createMatrix<T>(n, m, T());
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < m; ++j)
            C[i][j] = A[i][j] - B[i][j];
    return C;
}

template <typename T>
Matrix2D<T> standardMultiply(const Matrix2D<T>& A, const Matrix2D<T>& B) {
    size_t r1 = A.size();
    size_t c1 = A[0].size();
    size_t c2 = B[0].size();
    Matrix2D<T> C = createMatrix<T>(r1, c2, T());
    for (size_t i = 0; i < r1; ++i) {
        for (size_t k = 0; k < c1; ++k) {
            T a_ik = A[i][k];
            for (size_t j = 0; j < c2; ++j) {
                C[i][j] += a_ik * B[k][j];
            }
        }
    }
    return C;
}

size_t nextPowerOfTwo(size_t n) {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

template <typename T>
Matrix2D<T> strassenRecursive(const Matrix2D<T>& A, const Matrix2D<T>& B, size_t threshold) {
    size_t n = A.size();
    if (n <= threshold) {
        return standardMultiply(A, B);
    }

    size_t half = n / 2;
    Matrix2D<T> A11 = createMatrix<T>(half, half);
    Matrix2D<T> A12 = createMatrix<T>(half, half);
    Matrix2D<T> A21 = createMatrix<T>(half, half);
    Matrix2D<T> A22 = createMatrix<T>(half, half);

    Matrix2D<T> B11 = createMatrix<T>(half, half);
    Matrix2D<T> B12 = createMatrix<T>(half, half);
    Matrix2D<T> B21 = createMatrix<T>(half, half);
    Matrix2D<T> B22 = createMatrix<T>(half, half);

    for (size_t i = 0; i < half; ++i) {
        for (size_t j = 0; j < half; ++j) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + half];
            A21[i][j] = A[i + half][j];
            A22[i][j] = A[i + half][j + half];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + half];
            B21[i][j] = B[i + half][j];
            B22[i][j] = B[i + half][j + half];
        }
    }

    Matrix2D<T> M1 = strassenRecursive(addMatrix(A11, A22), addMatrix(B11, B22), threshold);
    Matrix2D<T> M2 = strassenRecursive(addMatrix(A21, A22), B11, threshold);
    Matrix2D<T> M3 = strassenRecursive(A11, subMatrix(B12, B22), threshold);
    Matrix2D<T> M4 = strassenRecursive(A22, subMatrix(B21, B11), threshold);
    Matrix2D<T> M5 = strassenRecursive(addMatrix(A11, A12), B22, threshold);
    Matrix2D<T> M6 = strassenRecursive(subMatrix(A21, A11), addMatrix(B11, B12), threshold);
    Matrix2D<T> M7 = strassenRecursive(subMatrix(A12, A22), addMatrix(B21, B22), threshold);

    Matrix2D<T> C11 = addMatrix(subMatrix(addMatrix(M1, M4), M5), M7);
    Matrix2D<T> C12 = addMatrix(M3, M5);
    Matrix2D<T> C21 = addMatrix(M2, M4);
    Matrix2D<T> C22 = addMatrix(addMatrix(subMatrix(M1, M2), M3), M6);

    Matrix2D<T> C = createMatrix<T>(n, n);
    for (size_t i = 0; i < half; ++i) {
        for (size_t j = 0; j < half; ++j) {
            C[i][j] = C11[i][j];
            C[i][j + half] = C12[i][j];
            C[i + half][j] = C21[i][j];
            C[i + half][j + half] = C22[i][j];
        }
    }
    return C;
}

template <typename T>
Matrix2D<T> strassenMultiply(const Matrix2D<T>& A, const Matrix2D<T>& B, size_t threshold = 32) {
    size_t r1 = A.size();
    size_t c1 = A[0].size();
    size_t r2 = B.size();
    size_t c2 = B[0].size();
    assert(c1 == r2 && "Incompatible matrix dimensions for multiplication");

    size_t maxDim = r1;
    if (c1 > maxDim) maxDim = c1;
    if (c2 > maxDim) maxDim = c2;

    if (maxDim <= threshold) {
        return standardMultiply(A, B);
    }

    size_t padSize = nextPowerOfTwo(maxDim);
    Matrix2D<T> Apad = createMatrix<T>(padSize, padSize, T(0));
    Matrix2D<T> Bpad = createMatrix<T>(padSize, padSize, T(0));

    for (size_t i = 0; i < r1; ++i)
        for (size_t j = 0; j < c1; ++j)
            Apad[i][j] = A[i][j];

    for (size_t i = 0; i < r2; ++i)
        for (size_t j = 0; j < c2; ++j)
            Bpad[i][j] = B[i][j];

    Matrix2D<T> Cpad = strassenRecursive(Apad, Bpad, threshold);
    Matrix2D<T> C = createMatrix<T>(r1, c2, T(0));
    for (size_t i = 0; i < r1; ++i)
        for (size_t j = 0; j < c2; ++j)
            C[i][j] = Cpad[i][j];

    return C;
}

template <typename T>
Matrix2D<T> inverseGaussJordan(const Matrix2D<T>& A) {
    size_t n = A.size();
    assert(n == A[0].size() && "Matrix must be square");

    Matrix2D<T> aug = createMatrix<T>(n, 2 * n, T(0));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) aug[i][j] = A[i][j];
        aug[i][n + i] = T(1);
    }

    for (size_t i = 0; i < n; ++i) {
        size_t pivot = i;
        for (size_t r = i + 1; r < n; ++r) {
            if (std::fabs(aug[r][i]) > std::fabs(aug[pivot][i])) pivot = r;
        }
        assert(std::fabs(aug[pivot][i]) > 1e-12 && "Matrix is singular");

        if (pivot != i) {
            CustomVector<T> tmp = aug[i];
            aug[i] = aug[pivot];
            aug[pivot] = tmp;
        }

        T div = aug[i][i];
        for (size_t j = 0; j < 2 * n; ++j) aug[i][j] /= div;

        for (size_t r = 0; r < n; ++r) {
            if (r != i) {
                T factor = aug[r][i];
                for (size_t j = 0; j < 2 * n; ++j) {
                    aug[r][j] -= factor * aug[i][j];
                }
            }
        }
    }

    Matrix2D<T> inv = createMatrix<T>(n, n, T(0));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            inv[i][j] = aug[i][n + j];
    return inv;
}

template <typename T>
Matrix2D<T> inverseLU(const Matrix2D<T>& A) {
    size_t n = A.size();
    assert(n == A[0].size() && "Matrix must be square");

    Matrix2D<T> LU = A;
    CustomVector<size_t> P(n);
    for (size_t i = 0; i < n; ++i) P[i] = i;

    for (size_t i = 0; i < n; ++i) {
        size_t maxRow = i;
        T maxVal = std::fabs(LU[i][i]);
        for (size_t k = i + 1; k < n; ++k) {
            if (std::fabs(LU[k][i]) > maxVal) {
                maxVal = std::fabs(LU[k][i]);
                maxRow = k;
            }
        }
        assert(maxVal > 1e-12 && "Matrix is singular");

        if (maxRow != i) {
            CustomVector<T> tmpRow = LU[i];
            LU[i] = LU[maxRow];
            LU[maxRow] = tmpRow;

            size_t tmpIdx = P[i];
            P[i] = P[maxRow];
            P[maxRow] = tmpIdx;
        }

        for (size_t j = i + 1; j < n; ++j) {
            LU[j][i] /= LU[i][i];
            for (size_t k = i + 1; k < n; ++k) {
                LU[j][k] -= LU[j][i] * LU[i][k];
            }
        }
    }

    Matrix2D<T> inv = createMatrix<T>(n, n, T(0));
    for (size_t col = 0; col < n; ++col) {
        CustomVector<T> y(n, T(0));
        for (size_t i = 0; i < n; ++i) {
            T sum = (P[i] == col) ? T(1) : T(0);
            for (size_t k = 0; k < i; ++k) sum -= LU[i][k] * y[k];
            y[i] = sum;
        }
        for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
            T sum = y[i];
            for (size_t k = i + 1; k < n; ++k) sum -= LU[i][k] * inv[k][col];
            inv[i][col] = sum / LU[i][i];
        }
    }
    return inv;
}

template <typename T>
CustomVector<T> eigenvaluesQR(Matrix2D<T> A, size_t maxIter = 500, double tol = 1e-9, size_t threshold = 32) {
    size_t n = A.size();
    assert(n == A[0].size() && "Matrix must be square");

    for (size_t iter = 0; iter < maxIter; ++iter) {
        Matrix2D<T> Q = createMatrix<T>(n, n, T(0));
        Matrix2D<T> R = createMatrix<T>(n, n, T(0));

        for (size_t j = 0; j < n; ++j) {
            CustomVector<T> v(n, T(0));
            for (size_t row = 0; row < n; ++row) v[row] = A[row][j];

            for (size_t i = 0; i < j; ++i) {
                T dot = T(0);
                for (size_t row = 0; row < n; ++row) dot += Q[row][i] * A[row][j];
                R[i][j] = dot;
                for (size_t row = 0; row < n; ++row) v[row] -= dot * Q[row][i];
            }

            T norm = T(0);
            for (size_t row = 0; row < n; ++row) norm += v[row] * v[row];
            norm = std::sqrt(norm);
            R[j][j] = norm;

            if (norm > 1e-14) {
                for (size_t row = 0; row < n; ++row) Q[row][j] = v[row] / norm;
            }
        }

        A = strassenMultiply(R, Q, threshold);

        double offDiag = 0.0;
        for (size_t i = 1; i < n; ++i)
            for (size_t j = 0; j < i; ++j)
                offDiag += std::fabs(A[i][j]);
        if (offDiag < tol) break;
    }

    CustomVector<T> evals(n);
    for (size_t i = 0; i < n; ++i) evals[i] = A[i][i];
    return evals;
}

void runUnitTests() {
    std::mt19937 gen(42);
    std::uniform_real_distribution<double> dist(-5.0, 5.0);

    Matrix2D<double> A = createMatrix<double>(37, 53);
    Matrix2D<double> B = createMatrix<double>(53, 41);
    for (size_t i = 0; i < 37; ++i)
        for (size_t j = 0; j < 53; ++j) A[i][j] = dist(gen);
    for (size_t i = 0; i < 53; ++i)
        for (size_t j = 0; j < 41; ++j) B[i][j] = dist(gen);

    Matrix2D<double> C_std = standardMultiply(A, B);
    Matrix2D<double> C_str = strassenMultiply(A, B, 16);

    double maxErr = 0.0;
    for (size_t i = 0; i < 37; ++i)
        for (size_t j = 0; j < 41; ++j)
            maxErr = std::max(maxErr, std::fabs(C_std[i][j] - C_str[i][j]));
    assert(maxErr < 1e-9);
    std::cout << "[PASS] Unit Test 1: Strassen arbitrary size (37x53 * 53x41), max error = " << maxErr << "\n";

    size_t n = 45;
    Matrix2D<double> M = createMatrix<double>(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) M[i][j] = dist(gen);
        M[i][i] += 25.0;
    }

    Matrix2D<double> invGJ = inverseGaussJordan(M);
    Matrix2D<double> invLU = inverseLU(M);

    Matrix2D<double> idGJ = strassenMultiply(M, invGJ, 16);
    Matrix2D<double> idLU = strassenMultiply(M, invLU, 16);

    double errGJ = 0.0, errLU = 0.0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double target = (i == j) ? 1.0 : 0.0;
            errGJ = std::max(errGJ, std::fabs(idGJ[i][j] - target));
            errLU = std::max(errLU, std::fabs(idLU[i][j] - target));
        }
    }
    assert(errGJ < 1e-9 && errLU < 1e-9);
    std::cout << "[PASS] Unit Test 2: Gauss-Jordan Inverse (45x45) verified via Strassen, max error = " << errGJ << "\n";
    std::cout << "[PASS] Unit Test 3: LU Inverse (45x45) verified via Strassen, max error = " << errLU << "\n";

    Matrix2D<double> S = createMatrix<double>(4, 4, 0.0);
    S[0][0] = 4.0; S[0][1] = 1.0; S[0][2] = 2.0; S[0][3] = 0.0;
    S[1][0] = 1.0; S[1][1] = 3.0; S[1][2] = 0.0; S[1][3] = 1.0;
    S[2][0] = 2.0; S[2][1] = 0.0; S[2][2] = 5.0; S[2][3] = -1.0;
    S[3][0] = 0.0; S[3][1] = 1.0; S[3][2] = -1.0; S[3][3] = 2.0;

    CustomVector<double> evals = eigenvaluesQR(S, 500, 1e-10, 2);
    double traceA = S[0][0] + S[1][1] + S[2][2] + S[3][3];
    double sumEvals = 0.0;
    for (size_t i = 0; i < evals.size(); ++i) sumEvals += evals[i];
    assert(std::fabs(traceA - sumEvals) < 1e-7);
    std::cout << "[PASS] Unit Test 4: QR Eigenvalues (4x4), trace diff = " << std::fabs(traceA - sumEvals)
              << ", evals = [" << evals[0] << ", " << evals[1] << ", " << evals[2] << ", " << evals[3] << "]\n";
}

void benchmarkLab1() {
    std::cout << "\n--- Benchmark Lab 1 (Strassen threshold = 64 vs Standard) ---\n";
    size_t sizes[] = {64, 128, 256, 512};
    std::mt19937 gen(123);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    for (size_t n : sizes) {
        Matrix2D<double> A = createMatrix<double>(n, n);
        Matrix2D<double> B = createMatrix<double>(n, n);
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j) {
                A[i][j] = dist(gen);
                B[i][j] = dist(gen);
            }

        auto t1 = std::chrono::high_resolution_clock::now();
        Matrix2D<double> C1 = standardMultiply(A, B);
        auto t2 = std::chrono::high_resolution_clock::now();
        double msStd = std::chrono::duration<double, std::milli>(t2 - t1).count();

        auto t3 = std::chrono::high_resolution_clock::now();
        Matrix2D<double> C2 = strassenMultiply(A, B, 64);
        auto t4 = std::chrono::high_resolution_clock::now();
        double msStr = std::chrono::duration<double, std::milli>(t4 - t3).count();

        std::cout << "N = " << std::setw(4) << n
                  << " | Standard: " << std::fixed << std::setprecision(3) << std::setw(8) << msStd << " ms"
                  << " | Strassen: " << std::setw(8) << msStr << " ms\n";
    }
}

int main() {
    std::cout << " RUNNING LABORATORY WORK 1 (Variant 4)\n";
    runUnitTests();
    benchmarkLab1();
    return 0;
}
