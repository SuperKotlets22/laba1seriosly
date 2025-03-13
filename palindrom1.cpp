#include <iostream>
#include <string>
using namespace std;

// Функция для проверки, является ли строка палиндромом
bool Palindrome(const string& text) {
    int left = 0;                      // Индекс начала строки
    int right = text.length() - 1;     // Индекс конца строки

    while (left < right) {
        // Пропускаем пробелы слева
        while (left < right && text[left] == ' ') {
            left++;
        }
        // Пропускаем пробелы справа
        while (left < right && text[right] == ' ') {
            right--;
        }

        // Сравниваем символы
        if (text[left] != text[right]) {
            return false;  // Если символы не совпадают, это не палиндром
        }

        left++;
        right--;
    }

    return true;  // Если все символы совпали, это палиндром
}

int main() {
    setlocale(LC_ALL, "Russian"); 

    string text;
    cout << "Введите строку: ";
    getline(cin, text);  // Ввод строки с пробелами

    if (Palindrome(text)) {
        cout << "Палиндром" << endl;
    }
    else {
        cout << "Не палиндром" << endl;
    }

    return 0;
}