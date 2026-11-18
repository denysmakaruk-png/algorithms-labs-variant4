#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <cassert>

void msdRadixSortRecursive(std::vector<std::string>& arr, std::vector<std::string>& aux,
                           int lo, int hi, size_t d) {
    if (hi <= lo + 1) return;

    const int R = 257;
    int count[R + 1] = {0};

    for (int i = lo; i < hi; ++i) {
        int bucket = (d < arr[i].size()) ? (static_cast<unsigned char>(arr[i][d]) + 1) : 0;
        count[bucket + 1]++;
    }

    for (int r = 0; r < R; ++r) {
        count[r + 1] += count[r];
    }

    for (int i = lo; i < hi; ++i) {
        int bucket = (d < arr[i].size()) ? (static_cast<unsigned char>(arr[i][d]) + 1) : 0;
        aux[lo + count[bucket]++] = std::move(arr[i]);
    }

    for (int i = lo; i < hi; ++i) {
        arr[i] = std::move(aux[i]);
    }

    for (int r = 0; r < 256; ++r) {
        int subLo = lo + count[r];
        int subHi = lo + count[r + 1];
        if (subHi > subLo + 1) {
            msdRadixSortRecursive(arr, aux, subLo, subHi, d + 1);
        }
    }
}

void msdRadixSort(std::vector<std::string>& arr) {
    if (arr.size() <= 1) return;
    std::vector<std::string> aux(arr.size());
    msdRadixSortRecursive(arr, aux, 0, static_cast<int>(arr.size()), 0);
}

std::vector<std::string> generateWordDataset(size_t count, size_t minLen, size_t maxLen, uint32_t seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<size_t> lenDist(minLen, maxLen);
    std::uniform_int_distribution<int> charDist('a', 'z');

    std::vector<std::string> words;
    words.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        size_t len = lenDist(gen);
        std::string w(len, ' ');
        for (size_t j = 0; j < len; ++j) {
            w[j] = static_cast<char>(charDist(gen));
        }
        words.push_back(std::move(w));
    }
    return words;
}

void runUnitTests() {
    std::vector<std::string> test1 = {
        "apple", "app", "", "application", "banana", "band", "ban", "a", "ab", "a"
    };
    std::vector<std::string> expected1 = test1;
    std::sort(expected1.begin(), expected1.end());
    msdRadixSort(test1);
    assert(test1 == expected1);
    std::cout << "[PASS] Unit Test 1: Prefix & empty string ('\\0') edge cases passed.\n";

    auto test2 = generateWordDataset(5000, 1, 25, 777);
    auto expected2 = test2;
    std::sort(expected2.begin(), expected2.end());
    msdRadixSort(test2);
    assert(test2 == expected2);
    std::cout << "[PASS] Unit Test 2: 5,000 random variable-length words match std::sort 100%.\n";
}

void benchmarkLab2() {
    std::cout << "\n Benchmark Lab 2: MSD Radix Sort vs std::sort \n";
    size_t counts[] = {50000, 100000, 250000, 500000};

    for (size_t n : counts) {
        auto data1 = generateWordDataset(n, 3, 20, 100 + static_cast<uint32_t>(n));
        auto data2 = data1;

        auto t1 = std::chrono::high_resolution_clock::now();
        msdRadixSort(data1);
        auto t2 = std::chrono::high_resolution_clock::now();
        double msRadix = std::chrono::duration<double, std::milli>(t2 - t1).count();

        auto t3 = std::chrono::high_resolution_clock::now();
        std::sort(data2.begin(), data2.end());
        auto t4 = std::chrono::high_resolution_clock::now();
        double msStd = std::chrono::duration<double, std::milli>(t4 - t3).count();

        assert(data1 == data2);
        std::cout << "Words = " << std::setw(7) << n
                  << " | MSD Radix Sort: " << std::fixed << std::setprecision(2) << std::setw(7) << msRadix << " ms"
                  << " | std::sort: " << std::setw(7) << msStd << " ms"
                  << " | Speedup: " << std::setprecision(2) << (msStd / msRadix) << "x\n";
    }
}

int main() {
    std::cout << " RUNNING LABORATORY WORK 2 (Variant 4) \n";
    runUnitTests();
    benchmarkLab2();
    return 0;
}
