#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>

using namespace std;

struct SortStats {
    long long comparisons;
    long long swaps;

    SortStats() : comparisons(0), swaps(0) {}
};

struct BenchmarkResult {
    string algorithmName;
    string orderType;
    int size;
    double averageCpuTimeMs;
    long long totalComparisons;
    long long totalSwaps;
    long long totalOperations;
};

enum OrderType {
    ORDER_ASCENDING,
    ORDER_DESCENDING,
    ORDER_RANDOM
};

string getOrderTypeName(OrderType orderType) {
    if (orderType == ORDER_ASCENDING) {
        return "Ascending";
    }
    if (orderType == ORDER_DESCENDING) {
        return "Descending";
    }
    return "Random";
}

vector<int> generateAscendingVector(int size) {
    vector<int> values(size);
    for (int i = 0; i < size; i++) {
        values[i] = i + 1;
    }
    return values;
}

vector<int> generateDescendingVector(int size) {
    vector<int> values(size);
    for (int i = 0; i < size; i++) {
        values[i] = size - i;
    }
    return values;
}

vector<int> generateRandomVector(int size, unsigned int seed) {
    vector<int> values(size);
    srand(seed);

    for (int i = 0; i < size; i++) {
        values[i] = rand() % (size * 10 + 1);
    }

    return values;
}

vector<int> generateBaseVector(int size, OrderType orderType, unsigned int seed) {
    if (orderType == ORDER_ASCENDING) {
        return generateAscendingVector(size);
    }

    if (orderType == ORDER_DESCENDING) {
        return generateDescendingVector(size);
    }

    return generateRandomVector(size, seed);
}

bool isSorted(const vector<int>& values) {
    for (size_t i = 1; i < values.size(); i++) {
        if (values[i - 1] > values[i]) {
            return false;
        }
    }
    return true;
}

void swapWithCount(int& a, int& b, SortStats& stats) {
    int temp = a;
    a = b;
    b = temp;
    stats.swaps++;
}

/*
|--------------------------------------------------------------------------
| Sorting algorithms
|--------------------------------------------------------------------------
| Add new algorithms following the same pattern:
| void yourSort(vector<int>& values, SortStats& stats)
|--------------------------------------------------------------------------
*/

void bubbleSort(vector<int>& values, SortStats& stats) {
    int n = static_cast<int>(values.size());

    for (int i = 0; i < n - 1; i++) {
        bool changed = false;

        for (int j = 0; j < n - 1 - i; j++) {
            stats.comparisons++;

            if (values[j] > values[j + 1]) {
                swapWithCount(values[j], values[j + 1], stats);
                changed = true;
            }
        }

        if (!changed) {
            break;
        }
    }
}

void selectionSort(vector<int>& values, SortStats& stats) {
    int n = static_cast<int>(values.size());

    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;

        for (int j = i + 1; j < n; j++) {
            stats.comparisons++;

            if (values[j] < values[minIndex]) {
                minIndex = j;
            }
        }

        if (minIndex != i) {
            swapWithCount(values[i], values[minIndex], stats);
        }
    }
}

void insertionSort(vector<int>& values, SortStats& stats) {
    int n = static_cast<int>(values.size());

    for (int i = 1; i < n; i++) {
        int key = values[i];
        int j = i - 1;

        while (j >= 0) {
            stats.comparisons++;

            if (values[j] > key) {
                values[j + 1] = values[j];
                stats.swaps++;
                j--;
            } else {
                break;
            }
        }

        values[j + 1] = key;
    }
}

typedef void (*SortFunction)(vector<int>&, SortStats&);

struct AlgorithmEntry {
    string name;
    SortFunction function;
};

BenchmarkResult runSingleCase(
    const AlgorithmEntry& algorithm,
    const vector<int>& baseVector,
    const string& orderTypeName,
    int size,
    int repetitions
) {
    double totalCpuTimeMs = 0.0;
    long long totalComparisons = 0;
    long long totalSwaps = 0;

    for (int run = 0; run < repetitions; run++) {
        vector<int> workingVector = baseVector;
        SortStats stats;

        clock_t startClock = clock();
        algorithm.function(workingVector, stats);
        clock_t endClock = clock();

        double cpuTimeMs = 1000.0 * static_cast<double>(endClock - startClock) / CLOCKS_PER_SEC;
        totalCpuTimeMs += cpuTimeMs;
        totalComparisons += stats.comparisons;
        totalSwaps += stats.swaps;

        if (!isSorted(workingVector)) {
            cerr << "Error: " << algorithm.name
                 << " failed for size " << size
                 << " and order " << orderTypeName << ".\n";
        }
    }

    BenchmarkResult result;
    result.algorithmName = algorithm.name;
    result.orderType = orderTypeName;
    result.size = size;
    result.averageCpuTimeMs = totalCpuTimeMs / repetitions;
    result.totalComparisons = totalComparisons;
    result.totalSwaps = totalSwaps;
    result.totalOperations = totalComparisons + totalSwaps;

    return result;
}

void printHeader(ostream& out) {
    out << left
        << setw(18) << "Algorithm"
        << setw(14) << "Order"
        << setw(10) << "Size"
        << setw(18) << "Avg CPU Time (ms)"
        << setw(18) << "Comparisons"
        << setw(15) << "Swaps"
        << setw(18) << "Total Ops"
        << "\n";

    out << string(111, '-') << "\n";
}

void printResultRow(ostream& out, const BenchmarkResult& result) {
    out << left
        << setw(18) << result.algorithmName
        << setw(14) << result.orderType
        << setw(10) << result.size
        << setw(18) << fixed << setprecision(3) << result.averageCpuTimeMs
        << setw(18) << result.totalComparisons
        << setw(15) << result.totalSwaps
        << setw(18) << result.totalOperations
        << "\n";
}

int main() {
    const int sizes[] = {500, 1000, 2000, 5000, 10000, 20000};
    const int sizeCount = 6;
    const int repetitions = 3;
    const unsigned int randomSeed = 12345;

    AlgorithmEntry algorithms[] = {
        {"Bubble Sort", bubbleSort},
        {"Selection Sort", selectionSort},
        {"Insertion Sort", insertionSort}
    };
    const int algorithmCount = 3;

    ofstream outputFile("benchmark_results.txt");
    if (!outputFile.is_open()) {
        cout << "Could not create benchmark_results.txt\n";
        return 1;
    }

    cout << "Automatic benchmarking started...\n";
    cout << "Repetitions per case: " << repetitions << "\n\n";

    outputFile << "Automatic benchmarking results\n";
    outputFile << "Repetitions per case: " << repetitions << "\n\n";

    printHeader(cout);
    printHeader(outputFile);

    for (int a = 0; a < algorithmCount; a++) {
        for (int s = 0; s < sizeCount; s++) {
            int currentSize = sizes[s];

            for (int o = 0; o < 3; o++) {
                OrderType orderType = static_cast<OrderType>(o);
                string orderTypeName = getOrderTypeName(orderType);

                vector<int> baseVector = generateBaseVector(
                    currentSize,
                    orderType,
                    randomSeed + currentSize + o
                );

                BenchmarkResult result = runSingleCase(
                    algorithms[a],
                    baseVector,
                    orderTypeName,
                    currentSize,
                    repetitions
                );

                printResultRow(cout, result);
                printResultRow(outputFile, result);
            }
        }
    }

    outputFile.close();

    cout << "\nFinished. Results saved to benchmark_results.txt\n";
    return 0;
}