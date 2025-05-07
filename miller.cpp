#include <iostream>
#include <vector>
#include <string>
#include <sstream> // Для форматирования вывода вектора чисел
#include <iomanip>  // setw
#include <numeric>
#include <random>   // Генерация случайных чисел
#include <stdexcept>
#include <chrono>   // Инициализация генератора
#include <cmath>    // sqrt для факторизации
#include <set>      // Для хранения уникальных простых множителей

// Типы данных
using ull = unsigned long long;
using u128 = __uint128_t;
using namespace std;

//  Начало Вспомогательных Функций 

// Модульное умножение (без изменений)
ull modulnoe_umnozhenie(ull a, ull b, ull m) {
    if (m == 0) throw runtime_error("Ошибка: Модуль m не может быть равен 0");
    a %= m;
    b %= m;
    u128 res = (u128)a * b;
    return (ull)(res % m);
}

// Модульное возведение в степень (без изменений)
ull modulnaya_stepen(ull base, ull exp, ull mod) {
    ull res = 1;
    if (mod == 0) throw runtime_error("Ошибка: Модуль m не может быть равен 0");
    if (mod == 1) return 0;
    base %= mod;
    if (base == 0) return (exp == 0) ? 1 : 0;

    while (exp > 0) {
        if (exp % 2 == 1) res = modulnoe_umnozhenie(res, base, mod);
        base = modulnoe_umnozhenie(base, base, mod);
        exp /= 2;
    }
    return res;
}

// Факторизация (без изменений)
vector<ull> prime_factors(ull num) {
     if (num <= 1) return {};
    set<ull> factors;
    while (num % 2 == 0) { factors.insert(2); num /= 2; }
    ull limit = static_cast<ull>(sqrt(static_cast<long double>(num)));
    for (ull i = 3; i <= limit; i += 2) {
        while (num % i == 0) {
            factors.insert(i);
            num /= i;
            limit = static_cast<ull>(sqrt(static_cast<long double>(num)));
        }
         if (limit < i) break;
    }
    if (num > 1) { factors.insert(num); }
    return vector<ull>(factors.begin(), factors.end());
}

//  Конец Вспомогательных Функций 

//  Начало Теста Миллера-Рабина (Необязательный Фильтр) 
// Функция без изменений, используется только как фильтр
bool filter_miller_rabin(ull n, int kolichestvo_proverok) {
     if (n < 2) return false; if (n == 2 || n == 3) return true; if (n % 2 == 0) return false;
    ull d = n - 1; int s = 0; while (d % 2 == 0) { d /= 2; s++; }
    static mt19937_64 rng_filter(chrono::steady_clock::now().time_since_epoch().count() + 1);
    uniform_int_distribution<ull> distrib_filter(2, n - 2 < 2 ? 2 : n - 2);
    for (int i = 0; i < kolichestvo_proverok; ++i) {
        ull a = (n <= 4) ? 2 : distrib_filter(rng_filter); ull x = modulnaya_stepen(a, d, n);
        if (x == 1 || x == n - 1) continue; bool found_minus_one = false;
        for (int r = 1; r < s; ++r) { x = modulnoe_umnozhenie(x, x, n); if (x == 1) return false; if (x == n - 1) { found_minus_one = true; break; } }
        if (!found_minus_one) return false;
    } return true;
}
//  Конец Фильтра Миллера-Рабина 


//  Начало Основного Теста (с факторами n-1, КОРРЕКТНАЯ ЛОГИКА) 
/**
 * @brief Основной тест простоты, использующий простые множители n-1 (Корректная логика).
 * Ищет хотя бы одно основание 'a' (среди k_bases попыток), удовлетворяющее:
 * 1. a^(n-1) === 1 (mod n)
 * 2. a^((n-1)/q) =!= 1 (mod n) для ВСЕХ простых множителей q числа n-1.
 * Наличие такого 'a' доказывает простоту n (Теорема Люка/Поклингтона).
 *
 * @param n Число для проверки (предполагается, что n > 3 и нечетное).
 * @param k_bases Количество случайных оснований 'a' для поиска свидетеля (4-6).
 * @param factors Вектор уникальных простых множителей числа n-1.
 * @param used_bases Вектор, в который будут записаны все проверенные основания 'a'.
 * @return true, если найден свидетель простоты 'a', false иначе (не найден за k_bases попыток).
 */
bool proverka_osnovnaya_s_faktorami(ull n, int k_bases, const vector<ull>& factors, vector<ull>& used_bases) {
     used_bases.clear();
     used_bases.reserve(k_bases);

     // Проверка наличия факторов (критично для теста)
     if (factors.empty()) {
        if (n > 3) { // Для n=2, 3 факторы не нужны, но мы их не должны сюда передавать
             cerr << "\nПредупреждение: Пустой список факторов для n=" << n << ", тест невозможен." << endl;
        }
        // Если тест невозможен, считаем результат отрицательным
        return false;
     }

    // Настройка генератора случайных чисел для оснований 'a'
    static mt19937_64 rng_a(chrono::steady_clock::now().time_since_epoch().count() + 2);
    // Основания обычно выбирают в диапазоне [2, n-1]
    uniform_int_distribution<ull> distrib_a(2, n - 1);

    // Цикл поиска свидетеля 'a' (делаем k_bases попыток)
    for (int i = 0; i < k_bases; ++i) {
        ull a = distrib_a(rng_a);
        used_bases.push_back(a); // Запоминаем проверенное основание

        // Проверяем Условие 1: a^(n-1) === 1 (mod n)
        // Это необходимое условие для простого n (Малая теорема Ферма)
        // и для того, чтобы 'a' могло быть свидетелем в тесте Люка.
        if (modulnaya_stepen(a, n - 1, n) != 1) {
            
            continue; // Пробуем следующее случайное 'a'
        }

        // a^((n-1)/q) =!= 1 (mod n) должно выполняться для ВСЕХ простых факторов q.
        bool condition2_holds_for_all_q = true; // Изначально предполагаем, что условие 2 выполняется
        for (ull q : factors) {
            // Проверка корректности фактора (на всякий случай)
            if ((n - 1) % q != 0) {
                 cerr << "\nЛогическая ошибка: фактор q=" << q << " не делит n-1=" << (n-1) << endl;
                 condition2_holds_for_all_q = false; // Считаем это нарушением
                 break; // Прерываем проверку факторов для этого 'a'
            }
            ull exponent = (n - 1) / q;
            if (modulnaya_stepen(a, exponent, n) == 1) {
                // Нарушение! Для данного 'q' условие 2 не выполнено.
                condition2_holds_for_all_q = false;
                // Нет смысла проверять другие 'q' для этого 'a', т.к. оно уже не свидетель.
                break;
            }
        }

        // Анализируем результат проверки Условия 2 для текущего 'a':
        // Если condition2_holds_for_all_q осталось true, значит, условие 2 выполнилось
        // для ВСЕХ простых факторов q для ЭТОГО основания 'a'.
        // А так как мы уже проверили, что и Условие 1 выполнено (иначе бы сделали continue),
        // то мы нашли 'a', которое является свидетелем простоты n!
        if (condition2_holds_for_all_q) {
            // Найден свидетель! Число n точно простое.
            return true;
        }

        // Если condition2_holds_for_all_q стало false, то текущее 'a' не является
        // свидетелем простоты. Переходим к следующей итерации цикла,
        // чтобы попробовать другое случайное 'a'.
    }

    // Если цикл завершился, а мы ни разу не вернули 'true',
    // это означает, что за k_bases попыток мы не смогли найти
    // ни одного основания 'a', удовлетворяющего обоим условиям.
    // Теоретически, это не доказывает, что n составное, но в рамках
    // вероятностного подхода (или если k_bases достаточно велико),
    // мы считаем, что тест не пройден.
    return false;
}
//  Конец Основного Теста 


// Основная функция программы (main) остается такой же, как в предыдущем вашем запросе
// (с выводом таблицы, описанием столбцов и т.д.)
// (весь код main без изменений)
int main() {
    const int NUM_EXPERIMENTS_TARGET = 10;
    const int OSNOVNOY_TEST_BASES = 5; // K = 5 (в диапазоне 4-6)
    const int FILTER_ITERATIONS = 5;
    const bool USE_FILTER = true;

    vector<int> experiment_numbers;
    vector<ull> numbers_tested;
    vector<char> results;
    vector<int> iterations_used;
    vector<vector<ull>> all_used_bases;

    mt19937_64 rng_p(chrono::steady_clock::now().time_since_epoch().count());
    ull min_p = 2;
    ull max_p = 500;
    uniform_int_distribution<ull> distrib_p(min_p, max_p);

    cout << "Проведение " << NUM_EXPERIMENTS_TARGET << " экспериментов:\n";

    cout << "   (Факторизация может занять время...)\n\n";

    int successful_experiments = 0;
    int attempts = 0;
    const int MAX_ATTEMPTS = NUM_EXPERIMENTS_TARGET * 5000; // Увеличено кол-во попыток

    while (successful_experiments < NUM_EXPERIMENTS_TARGET && attempts < MAX_ATTEMPTS) {
        attempts++;
        ull p;
        do {
            p = distrib_p(rng_p);
        } while (p < 5 || p % 2 == 0); // Генерируем нечетные >= 5

        cout << "Попытка " << attempts << ": P = " << p << " ..." << flush;

        // Шаг 1 (Опциональный): Фильтр Миллера-Рабина
        if (USE_FILTER) {
            if (!filter_miller_rabin(p, FILTER_ITERATIONS)) {
                cout << " [Фильтр М-Р: Составное]" << endl;
                continue;
            }
             cout << " [Фильтр М-Р: Пройдено] ->" << flush;
        }

        // Шаг 2: Факторизация P-1
        cout << " Факторизация P-1..." << flush;
        vector<ull> factors = prime_factors(p - 1);
        // Обработка случая, если p-1=1 (p=2) или p-1=2 (p=3), которые не должны возникать
        // Важнее обработать случай, если факторизация не дала множителей для p-1 > 2
        if (factors.empty() && (p - 1) > 1) {
             cout << " [Факторизация: Ошибка/Пусто. Пропуск P]" << endl;
             continue;
        }
        // Проверка для p=3 (factors={2}) или p=2 (factors={}) - сюда не должны попасть
        // if (p <= 3 && factors.empty() && p != 2 ) { // p=3 factors={2}, p=2 factors={}
        //     cout << " [Факторизация: OK (для p<=3)] ->" << flush;
        // } else if (!factors.empty()) {
        //     cout << " [Факторизация: Завершена] ->" << flush;
        // } else {
        //      cout << " [Факторизация: Неожиданный результат. Пропуск P]" << endl;
        //      continue;
        // }
        cout << " [Факторизация: Завершена] ->" << flush; // Упрощенный вывод


        // Шаг 3: Основная проверка с факторами
        cout << " Основной тест (K=" << OSNOVNOY_TEST_BASES << ")..." << flush;
        vector<ull> current_used_bases; // Вектор для получения баз из функции
        bool main_test_result = proverka_osnovnaya_s_faktorami(p, OSNOVNOY_TEST_BASES, factors, current_used_bases);

        // Сохраняем результаты
        successful_experiments++;
        experiment_numbers.push_back(successful_experiments);
        numbers_tested.push_back(p);
        results.push_back(main_test_result ? '+' : '-');
        iterations_used.push_back(OSNOVNOY_TEST_BASES); // K - число баз 'a'
        all_used_bases.push_back(current_used_bases); // Сохраняем список баз 'a'

        // Вывод промежуточного результата
        cout << (main_test_result ? " [Основной тест: Пройдено (+)]" : " [Основной тест: Не пройдено (-)]");
        cout << " Базы a: {";
        for (size_t j = 0; j < current_used_bases.size(); ++j) {
            cout << current_used_bases[j] << (j == current_used_bases.size() - 1 ? "" : ", ");
        }
        cout << "}" << endl;
    }

     if (successful_experiments < NUM_EXPERIMENTS_TARGET) {
         cerr << "\n\nПредупреждение: Не удалось провести " << NUM_EXPERIMENTS_TARGET
              << " успешных экспериментов за " << MAX_ATTEMPTS << " попыток."
              << " Возможно, диапазон генерации P слишком мал, содержит мало простых чисел,"
              << " или факторизация слишком медленная/неудачна." << endl;
     }


    // Вывод итоговой таблицы
    cout << "\n\nИтоговая таблица результатов (" << successful_experiments << " экспериментов):\n";
    cout << "----------------------------------------------------------------------------------------------------\n";
    cout << "| " << setw(3) << left << "N" << " | "
              << setw(10) << left << "P" << " | "
              << setw(15) << left << "Результат (+/-)" << " | "
              << setw(5) << left << "K" << " | "
              << setw(50) << left << "Использованные базы 'a' (из основного теста)" << " |\n";
    cout << "|-----|------------|-----------------|-------|----------------------------------------------------|\n";

    for (size_t i = 0; i < experiment_numbers.size(); ++i) {
        stringstream ss_bases;
        ss_bases << "{";
        if (!all_used_bases[i].empty()) {
            for (size_t j = 0; j < all_used_bases[i].size(); ++j) {
                 ss_bases << all_used_bases[i][j] << (j == all_used_bases[i].size() - 1 ? "" : ", ");
            }
        }
         ss_bases << "}";

        cout << "| " << setw(3) << left << experiment_numbers[i] << " | "
                  << setw(10) << left << numbers_tested[i] << " | "
                  << setw(15) << left << (results[i] == '+' ? "(+) Вероятно пр." : "(-) Составное") << " | "
                  << setw(5) << left << iterations_used[i] << " | "
                  << setw(50) << left << ss_bases.str() << " |\n";
    }
    cout << "----------------------------------------------------------------------------------------------------\n";
    cout << "* N     - номер успешного эксперимента.\n";
    cout << "* P     - число, проверенное основным тестом.\n";
    cout << "* (+/-) - результат основного теста (с использованием множителей P-1).\n";
    cout << "* K     - количество случайных оснований 'a', использованных в основном тесте (здесь = "
         << OSNOVNOY_TEST_BASES << ").\n";
    cout << "* Базы 'a' - список оснований, проверенных в основном тесте для данного P.\n";


    return 0;
}