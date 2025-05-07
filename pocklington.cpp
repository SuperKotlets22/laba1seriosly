#include <iostream>
#include <vector>
#include <tuple>
#include <set>
#include <cmath>
#include <random>

using namespace std;

// Перегрузка оператора вывода для вектора
template <typename T>
ostream& operator<<(ostream& os, vector<T>& result) {
    for (auto item : result) {
        os << "\t" << item << "\t║";  // Вывод каждого элемента с разделителями
    }
    os << "\n╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    return os;
}

// Функция для возведения числа в степень по модулю
int powMod(int base, int degree, int mod) {
    int result = base % mod;  // Начальное значение
    for (int i = 2; i <= degree; i++) {
        result = (result * base) % mod;  // Умножаем и берем остаток от деления
    }
    return result;
}

// Решето Эратосфена для нахождения простых чисел до N
vector<int> sieveEratos(int N) {
    vector<bool> boolPrime(N + 1, true);  // Массив, в котором помечены все числа как простые
    for (int p = 2; p * p <= N; p++) {
        if (boolPrime[p]) {
            for (int i = p * p; i <= N; i += p) {
                boolPrime[i] = false;  // Отмечаем составные числа
            }
        }
    }

    vector<int> prime;
    for (int p = 2; p <= N; p++) {
        if (boolPrime[p]) {
            prime.push_back(p);  // Добавляем простые числа в результат
        }
    }

    return prime;
}

// Миллер-Рабин тест (проверка вероятности простоты числа)
bool rabin(int num, int k) {
    if (num == 2 || num == 3) {
        return true;  // 2 и 3 - простые
    }
    if (num < 2 || num % 2 == 0) {
        return false;  // Четные числа - составные
    }
    int s = 0;
    int d = num - 1;
    while (d % 2 == 0) {
        d /= 2;
        s++;  // Разлагаем (num - 1) на 2^s * d
    }
    for (int i = 0; i < k; i++) {
        int a = rand() % (num - 4) + 2;  // Случайное число для проверки
        int x = powMod(a, d, num);  // x = a^d mod num
        int y;
        for (int j = 0; j < s; j++) {
            y = powMod(x, 2, num);  // Вычисляем x^2 mod num
            if (y == 1 && x != 1 && x != (num - 1)) {
                return false;  // Если x = 1, то число составное
            }
            x = y;
        }
        if (y != 1) {
            return false;  // Если y != 1, то число составное
        }
    }
    return true;  // Число, скорее всего, простое
}

// Функция для генерации случайных чисел в заданном диапазоне
int randDist(int min, int max) {
    random_device rd;
    mt19937 rand(rd());
    uniform_int_distribution<int> dist(min, max);
    return dist(rand);
}

// Тест Поклингтона для проверки вероятности простоты числа
bool poklington(int n, int t, vector<int> qList) {
    set<int> aSet;
    int aj;

    // Генерируем t различных случайных чисел для теста
    while (aSet.size() != t) {
        aj = randDist(2, n - 1);
        aSet.insert(aj);
    }

    // Проверяем условие для всех a
    for (int aj : aSet) {
        if (powMod(aj, n - 1, n) != 1) {
            return false;
        }
    }

    // Проверяем дополнительные условия для делителей числа
    for (int aj : aSet) {
        bool allNol = true;
        for (int qi : qList) {
            if (powMod(aj, (n - 1) / qi, n) == 1) {
                allNol = false;
                break;
            }
        }
        if (allNol) {
            return true;  // n - вероятно простое
        }
    }

    return false;  // n - вероятно составное
}

// Функция для вычисления числа n и списка qList
tuple<int, vector<int>> calcN(vector<int> prime, int bit) {
    int maxIndex = prime.size() - 1, maxPow = 1;
    int min = bit / 2 + 1, max = bit / 2 + 2;
    int powMax = pow(2, max);
    for (; pow(2, maxPow) < pow(2, (bit / 2) + 1); maxPow++);

    int f = 1;
    vector<int> qList;

    // Генерация числа n и соответствующего списка qList
    while (true) {
        int num = randDist(0, maxIndex);  // Случайный индекс для простого числа
        int power = randDist(1, maxPow);  // Случайная степень для простого числа

        if (pow(prime[num], power)) {
            if (f * pow(prime[num], power) < powMax) {
                f *= pow(prime[num], power);  // Умножаем и обновляем f
                qList.push_back(prime[num]);  // Добавляем простое число в qList
            }
        }

        if (f > pow(2, (bit / 2))) {
            if (f >= pow(2, (bit / 2) + 1)) {
                f = 1;
                qList.clear();
            } else {
                break;
            }
        }
    }

    int R = randDist(min - 1, max - 1);  // Генерация случайного числа для R
    if (R % 2 != 0) {
        R++;
    }
    int n = R * f + 1;  // Вычисление числа n
    return make_tuple(n, qList);
}

int main() {
    int bit = 10, t = 10, n, k = 0;
    vector<int> prime = sieveEratos(500);  // Генерация простых чисел до 500

    vector<int> qList;
    vector<int> result;
    vector<string> testVer;
    vector<int> kCont;

    // Генерация чисел и их проверка
    while (result.size() < 10) {
        tie(n, qList) = calcN(prime, bit);  // Генерация числа n и списка qList
        if (poklington(n, t, qList)) {  // Проверка тестом Поклингтона
            result.push_back(n);

            if (rabin(n, 3)) {  // Проверка Миллера-Рабина
                testVer.push_back("+");
            } else {
                testVer.push_back("-");
            }
            kCont.push_back(k);
            k = 0;
        } else {
            if (rabin(n, 3)) {
                k++;
            }
        }
    }

    // Вывод результатов
    cout << "\n╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║Num\t║";
    for (int i = 1; i < 11; i++)
        cout << "\t" << i << "\t║";
    cout << endl;
    cout << "╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣";
    cout << endl;
    cout << "║P\t║" << result;
    cout << "║Test\t║" << testVer;
    cout << "║k\t║";
    for (auto item : kCont)
        cout << "\t" << item << "\t║";
    cout << endl;
    cout << "╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝";
}
