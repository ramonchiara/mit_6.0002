#include <algorithm>
#include <gmpxx.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

gmp_randclass rng(gmp_randinit_default);
int randint(int min, int max) {
    // http://c-faq.com/lib/randrange.html
    // return min + rand() / (RAND_MAX / (max - min + 1) + 1);

    // rng.seed(time(NULL)); // called once, in main
    mpz_class n = rng.get_z_range(mpz_class(max - min + 1)) + min;
    return n.get_si();
}

// http://www.cplusplus.com/articles/D9j2Nwbp/
string to_string(int n) {
    stringstream out;
    out << n;
    return out.str();
}

int rollDie() {
    return randint(1, 6);
}

void testRoll(int n = 10) {
    string result = "";

    for (int i = 1; i <= n; i++) {
        result += to_string(rollDie());
    }

    cout << result;
}

void runSim(string goal, int numTrials, string txt) {
    double total = 0;

    for (int i = 1; i <= numTrials; i++) {
        string result = "";
        for (unsigned int j = 1; j <= goal.size(); j++) {
            result += to_string(rollDie());
        }
        if (result == goal) {
            total++;
        }
    }

    double actualProbability = 1 / pow(6, goal.size());
    double estProbability = total / numTrials;

    cout << "Actual probability of " << txt << " = " << fixed << setprecision(8) << actualProbability << endl;
    cout << "Estimated probability of " << txt << " = " << fixed << setprecision(8) << estProbability << endl;
}

vector<int> uniformPossibleDates() {
    vector<int> possibleDates(365, 0);

    for (unsigned int i = 0; i < possibleDates.size(); i++) {
        possibleDates[i] = i;
    }

    return possibleDates;
}

vector<int> notUniformPossibleDates() {
    vector<int> possibleDates;

    for (int n = 1; n <= 4; n++)
        for (int i = 0; i < 57 ; i++) {
            possibleDates.push_back(i);
        }

    possibleDates.push_back(58);

    for (int n = 1; n <= 4; n++)
        for (int i = 59; i < 366 ; i++) {
            possibleDates.push_back(i);
        }

    for (int n = 1; n <= 4; n++)
        for (int i = 180; i < 270 ; i++) {
            possibleDates.push_back(i);
        }

    return possibleDates;
}

bool sameDate(int numPeople, int numSame) {
    vector<int> possibleDates = uniformPossibleDates();
    // vector<int> possibleDates = notUniformPossibleDates();
    vector<int> birthdays(366, 0);

    for(int p = 1; p <= numPeople; p++) {
        int index = randint(0, possibleDates.size() - 1);
        int birthDate = possibleDates[index];
        birthdays[birthDate]++;
    }

    return *max_element(birthdays.begin(), birthdays.end()) >= numSame;
}

double birthdayProb(int numPeople, int numSame, int numTrials) {
    double numHits = 0;

    for (int t = 1; t <= numTrials; t++) {
        if (sameDate(numPeople, numSame)) {
            numHits++;
        }
    }

    return numHits / numTrials;
}

double factorial(int n) {
    double f = 1;

    for (int i = 1; i <= n; i++) {
        f *= i;
    }

    return f;
}

int main() {
    // srand(time(NULL));
    // srand(0);
    // rng.seed(time(NULL));
    rng.seed(0);

    // testRoll(5);
    // runSim("11111", 1000, "11111");
    // runSim("11111", 10000000, "11111");

    int np[] = {10, 20, 40, 100};
    int n = sizeof(np) / sizeof(int);

    for (int i = 0; i < n; i++) {
        int numPeople = np[i];

        double estProb = birthdayProb(numPeople, 2, 10000);
        // double estProb = birthdayProb(numPeople, 3, 10000);

        //double numerator = factorial(366);
        mpf_class numerator = mpz_class::factorial(366);

        // double denom = pow(366, numPeople) * factorial(366 - numPeople);
        mpz_t x;
        mpz_init(x);
        mpz_ui_pow_ui(x, 366, numPeople);
        mpf_class denom = mpz_class(x) * mpz_class::factorial(366 - numPeople);

        // 1 - numerator / denom
        mpf_class actualProb = 1 - numerator / denom;

        cout << "For " << numPeople << " est. prob. of a shared birthday is " << estProb << endl;
        cout << "Actual prob. for N = 100 = " << setprecision(16) << actualProb << endl;

        mpz_clear(x);
    }
}
