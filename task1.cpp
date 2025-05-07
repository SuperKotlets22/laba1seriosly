#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace::std;

double calculateY(double x) {
    if (x < -3.0) {
        return x + 3.0;
    } else if (x <= 0.0) {
        double radicand = 9.0 - x * x;
        if (radicand < 0.0) {
             return NAN;
        }
        return sqrt(radicand);
    } else if (x <= 6.0) {
        return -0.5 * x + 3.0;
    } else {
        return x - 6.0;
    }
}

int main() {
    double x_start = -5.0;
    double x_end = 9.0;
    double dx = 1.0;

    cout << "+-------+-----------+" << endl;
    cout << "|   X   |     Y     |" << endl;
    cout << "+-------+-----------+" << endl;

    cout << fixed << setprecision(4);

    for (double x = x_start; x <= x_end + dx/2.0; x += dx) {
        double y = calculateY(x);
        if (isnan(y)) {
            cout << "| " << setw(5) << x << " | " << setw(9) << "NaN" << " |" << endl;
        } else {
            cout << "| " << setw(5) << x << " | " << setw(9) << y << " |" << endl;
        }
    }

    cout << "+-------+-----------+" << endl;

    return 0;
}