#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <set>
#include <iomanip>

using namespace std;

// Быстрое возведение в степень по модулю
int powMod(int base, int exponent, int mod) {
    int result = 1;
    base %= mod;
    while (exponent > 0) {
        if (exponent & 1)
            result = int(1LL * result * base % mod);
        base = int(1LL * base * base % mod);
        exponent >>= 1;
    }
    return result;
}

// Генератор случайного числа [min, max]
int randInt(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    return uniform_int_distribution<>(min, max)(gen);
}

// Генерация случайного вещественного числа ξ ∈ [0, 1)
double randDouble() {
static random_device rd;
static mt19937 gen(rd());
return uniform_real_distribution<>(0.0, 1.0)(gen);
}

// Решето Эратосфена
vector<int> sieveEratos(int N) {
    vector<bool> isPrime(N + 1, true);
    vector<int> primes;
    for (int p = 2; p <= N; ++p) {
        if (isPrime[p]) {
            primes.push_back(p);
            for (int i = p * 2; i <= N; i += p)
                isPrime[i] = false;
        }
    }
    return primes;
}

// Тест Миллера-Рабина (проверка на простоту)
bool millerRabin(int n, int iterations = 5) {
    if (n < 2 || n % 2 == 0) return false;
    int d = n - 1;
    int r = 0;
    while (d % 2 == 0) {
        d /= 2;
        r++;
    }

    for (int i = 0; i < iterations; ++i) {
        int a = randInt(2, n - 2);
        int x = powMod(a, d, n);
        if (x == 1 || x == n - 1)
            continue;

        bool passed = false;
        for (int j = 0; j < r - 1; ++j) {
            x = powMod(x, 2, n);
            if (x == n - 1) {
                passed = true;
                break;
            }
        }
        if (!passed)
            return false;
    }
    return true;
}

// Генерация простого числа p по ГОСТ Р 34.10-94 (вариант ξ ≠ 0)
int generateGOSTPrime(const vector<int>& primes, int bitLen) {

    int qBitLen = (bitLen + 1) / 2;
    //int qMin = 1 << (qBitLen - 1);
    int qMin = 0;
    int qMax = (1 << qBitLen) - 1;

    int q;
    do {
        q = primes[randInt(0, primes.size() - 1)];
    } while (q < qMin || q > qMax);


    while (true) {
        double ξ = randDouble();
        double n = (double(1 << (bitLen - 1)) + (ξ * double(1 << (bitLen - 1)))) / q;
        int nInt = int(n);
        if (nInt % 2 != 0) nInt++;

        for (int k = 0; ; k += 2) {
            int p = (nInt + k) * q + 1;
            if (p >= (1 << bitLen)) break;

            if (powMod(2, p - 1, p) == 1 && powMod(2, nInt + k, p) != 1) {
                return p;
            }
        }
    }
}

int main() {
    const int bitLength = 12; // Битовая длина p
    const int testRounds = 5; // Повторы теста Миллера-Рабина
    set<int> primesSet; // Храним уникальные простые p
    vector<int> primes = sieveEratos(500); // Простые q
    vector<string> testResults; // Результаты т

    // Генерация 10 уникальных p
    while (primesSet.size() < 10) {
        int p = generateGOSTPrime(primes, bitLength);
        if (primesSet.count(p)) continue;

        if (millerRabin(p, testRounds)) {
            primesSet.insert(p);
            testResults.push_back("ПРОЙДЕН");
        
        }
    }

    // Вывод результатов в таблице
    cout << "╔═════╦═══════╦═══════════════════════╗\n";
    cout << "║  №  ║   P   ║ Тест Миллера-Рабина   ║\n";
    cout << "╠═════╬═══════╬═══════════════════════╣\n";

    int i = 1;

    for (int p : primesSet) {
        cout << "║ " << setw(3) << i++ << " ║ " << setw(5) << p << " ║"
        << setw(21) << testResults[i - 2] << "         ║\n";
    }

    cout << "╚═════╩═══════╩═══════════════════════╝\n";

    return 0;
}
