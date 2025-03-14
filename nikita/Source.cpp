#include <iostream>
#include <string>
#include <cctype>

using namespace std;

void NumberOfVowels(string str1, int& res) {
	// Функция на количество гласных
	char all_vowels[10] = { 'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U' };

	for (int i = 0; i < str1.length(); i++)
	{
		for (int j = 0; j < 10; j++) {
			if (str1[i] == all_vowels[j])
				res++;
		}
	}
}



int main() {
	int task;

			int result = 0;
			string string1;

			cout << "Enter your string: ";

			cin.get();
			getline(cin, string1);

			NumberOfVowels(string1, result);

			cout << "Vowels: " << result << endl;
		
	return 0;
}