#include <iostream>
#include <locale>
#include <cwctype> // для towlower и towupper
#include <string>

using namespace std;
//hi mir.mir
int main() {
    // Устанавливаем русскую локаль
    locale::global(locale("ru_RU.utf8"));
    wcin.imbue(locale());
    wcout.imbue(locale());

    wstring input;
    wchar_t choice;

    // Ввод строки
    wcout << L"Введите строку: ";
    getline(wcin, input);

    // Ввод выбора регистра
    wcout << L"Выберите регистр (L - нижний, U - верхний): ";
    wcin >> choice;

    // Преобразование строки
    if (choice == L'L' || choice == L'l') {
        for (wchar_t &ch : input) ch = towlower(ch);
        wcout << L"Строка в нижнем регистре: " << input << endl;
    } 
    else if (choice == L'U' || choice == L'u') {
        for (wchar_t &ch : input) ch = towupper(ch);
        wcout << L"Строка в верхнем регистре: " << input << endl;
    } 
    else {
        wcout << L"Некорректный выбор регистра!" << endl;
        return 1;
    }

    return 0;
}
