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

void shellSort(vector<int>& arr, SortStats& stats) {
    int n = arr.size();

    for (int salto = n / 2; salto > 0; salto /= 2) {
        for (int i = salto; i < n; i++) {
            int chave = arr[i];
            int j = i;

            while (j >= salto) {
                stats.comparisons++;

                if (arr[j - salto] > chave) {
                    arr[j] = arr[j - salto];
                    stats.swaps++;
                    j -= salto;
                } else {
                    break;
                }
            }

            arr[j] = chave;
        }
    }
}

int partition(vector<int>& arr, int low, int high, SortStats& stats) {
    int pivot = arr[high]; // pivô (último elemento)
    int i = low - 1;

    for (int j = low; j < high; j++) {
        stats.comparisons++;

        if (arr[j] < pivot) {
            i++;

            // troca arr[i] e arr[j]
            swap(arr[i], arr[j]);
            stats.swaps++;
        }
    }

    // coloca o pivô na posição correta
    swap(arr[i + 1], arr[high]);
    stats.swaps++;

    return i + 1;
}

void quickSortRec(vector<int>& arr, int low, int high, SortStats& stats) {
    if (low < high) {
        int pi = partition(arr, low, high, stats);

        quickSortRec(arr, low, pi - 1, stats);
        quickSortRec(arr, pi + 1, high, stats);
    }
}

void quickSort(vector<int>& arr, SortStats& stats) {
    quickSortRec(arr, 0, arr.size() - 1, stats);
}

void heapify(vector<int>& arr, int n, int i, SortStats& stats) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    // filho esquerdo
    if (left < n) {
        stats.comparisons++;
        if (arr[left] > arr[largest]) {
            largest = left;
        }
    }

    // filho direito
    if (right < n) {
        stats.comparisons++;
        if (arr[right] > arr[largest]) {
            largest = right;
        }
    }

    // se o maior não for a raiz
    if (largest != i) {
        swap(arr[i], arr[largest]);
        stats.swaps++;

        heapify(arr, n, largest, stats);
    }
}

void heapSort(vector<int>& arr, SortStats& stats) {
    int n = arr.size();

    // construir heap (max-heap)
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i, stats);
    }

    // extrair elementos do heap
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        stats.swaps++;

        heapify(arr, i, 0, stats);
    }
}

void merge(vector<int>& arr, int left, int mid, int right, SortStats& stats) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
        stats.swaps++;
    }

    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
        stats.swaps++;
    }

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        stats.comparisons++;

        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
        stats.swaps++;
    }

    while (i < n1) {
        arr[k++] = L[i++];
        stats.swaps++;
    }

    while (j < n2) {
        arr[k++] = R[j++];
        stats.swaps++;
    }
}

void mergeSortRec(vector<int>& arr, int left, int right, SortStats& stats) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSortRec(arr, left, mid, stats);
        mergeSortRec(arr, mid + 1, right, stats);

        merge(arr, left, mid, right, stats);
    }
}

void mergeSort(vector<int>& arr, SortStats& stats) {
    mergeSortRec(arr, 0, arr.size() - 1, stats);
}

int getMax(const vector<int>& arr) {
    int mx = arr[0];
    for (int i = 1; i < arr.size(); i++)
        if (arr[i] > mx)
            mx = arr[i];
    return mx;
}

void countingSort(vector<int>& arr, int exp, SortStats& stats) {
    int n = arr.size();
    vector<int> output(n);
    int count[10] = {0};

    for (int i = 0; i < n; i++) {
        count[(arr[i] / exp) % 10]++;
        stats.swaps++;
    }

    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; i--) {
        int idx = (arr[i] / exp) % 10;
        output[count[idx] - 1] = arr[i];
        count[idx]--;
        stats.swaps++;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
        stats.swaps++;
    }
}

void radixSort(vector<int>& arr, SortStats& stats) {
    int m = getMax(arr);

    for (int exp = 1; m / exp > 0; exp *= 10) {
        countingSort(arr, exp, stats);
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
        {"Insertion Sort", insertionSort},
        {"Shell Sort", shellSort},
        {"Quick Sort", quickSort},
        {"Merge Sort", mergeSort},
        {"Radix Sort", radixSort},
        {"Heap Sort", heapSort}
    };
    const int algorithmCount = 8;

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