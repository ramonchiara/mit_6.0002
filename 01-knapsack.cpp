#include <algorithm>
#include <gmpxx.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>

using namespace std;

class Food
{
    string name;
    double value;
    double calories;

public:
    Food(string name, double value, double calories):
        name(name), value(value), calories(calories)
    {
    }

    double getValue() const
    {
        return value;
    }

    double getCost() const
    {
        return calories;
    }

    double density() const
    {
        return getValue() / getCost();
    }

private:
    friend ostream& operator<<(ostream& o, const Food &f);
};

ostream& operator<<(ostream& o, const Food &f)
{
    return o << f.name << ": <" << f.value << ", " << f.calories << ">";
}

void buildMenu(string names[], double values[], double calories[], unsigned int size, vector<Food> *menu)
{
    for(unsigned int i = 0; i < size; i++) {
        menu->push_back(Food(names[i], values[i], calories[i]));
    }
}

double greedy(const vector<Food> items, double maxCost, bool (*keyFunction)(const Food &f1, const Food &f2), vector<Food> *result)
{
    vector<Food> itemsCopy;
    copy(items.begin(), items.end(), back_inserter(itemsCopy));
    sort(itemsCopy.begin(), itemsCopy.end(), keyFunction);
    reverse(itemsCopy.begin(), itemsCopy.end());

    double totalValue = 0.0;
    double totalCost = 0.0;

    for(unsigned int i = 0; i < itemsCopy.size(); i++) {
        Food f = itemsCopy.at(i);

        if(totalCost + f.getCost() <= maxCost) {
            result->push_back(f);
            totalCost += f.getCost();
            totalValue += f.getValue();
        }
    }

    return totalValue;
}

void testGreedy(const vector<Food> &items, double constraint, bool (*keyFunction)(const Food &f1, const Food &f2))
{
    vector<Food> taken;
    double val = greedy(items, constraint, keyFunction, &taken);

    cout << "Total value of items take = " << val << endl;
    for(unsigned int i = 0; i < taken.size(); i++) {
        cout << "    " << taken.at(i) << endl;
    }
}

bool sortByValue(const Food &f1, const Food &f2)
{
    return f1.getValue() <= f2.getValue();
}

bool sortByCost(const Food &f1, const Food &f2)
{
    return 1 / f1.getCost() <= 1 / f2.getCost();
}

bool sortByDensity(const Food &f1, const Food &f2)
{
    return f1.density() <= f2.density();
}

void testGreedys(const vector<Food> &foods, double maxUnits)
{
    cout << "Use greedy by value to allocate " << maxUnits << " calories" << endl;
    testGreedy(foods, maxUnits, sortByValue);

    cout << "Use greedy by cost to allocate " << maxUnits << " calories" << endl;
    testGreedy(foods, maxUnits, sortByCost);

    cout << "Use greedy by density to allocate " << maxUnits << " calories" << endl;
    testGreedy(foods, maxUnits, sortByDensity);
}

double maxVal(const vector<Food> &toConsider, double avail, vector<Food> *toTake)
{
    double totalValue;

    if (toConsider.empty() || avail == 0) {
        totalValue = 0;
        toTake->clear();
    } else if (toConsider.at(0).getCost() > avail) {
        // explore right branch only
        vector<Food> nextToConsider(toConsider.begin() + 1, toConsider.end());
        totalValue = maxVal(nextToConsider, avail, toTake);
    } else {
        Food nextItem = toConsider.at(0);
        vector<Food> nextToConsider(toConsider.begin() + 1, toConsider.end());

        // explore left branch
        vector<Food> withToTake;
        double withVal = maxVal(nextToConsider, avail - nextItem.getCost(), &withToTake);
        withVal += nextItem.getValue();

        // explore right branch
        vector<Food> withoutToTake;
        double withoutVal = maxVal(nextToConsider, avail - nextItem.getCost(), &withoutToTake);

        if (withVal > withoutVal) {
            totalValue = withVal;
            copy(withToTake.begin(), withToTake.end(), back_inserter(*toTake));
            toTake->push_back(nextItem);
        } else {
            totalValue = withoutVal;
            copy(withoutToTake.begin(), withoutToTake.end(), back_inserter(*toTake));
        }
    }

    return totalValue;
}

void testMaxVal(const vector<Food> &foods, double maxUnits, bool printItems = true)
{
    cout << "Use search tree to allocate " << maxUnits << " calories" << endl;

    vector<Food> taken;
    double val = maxVal(foods, maxUnits, &taken);

    cout << "Total value of items take = " << val << endl;
    if (printItems) {
        for(unsigned int i = 0; i < taken.size(); i++) {
            cout << "    " << taken.at(i) << endl;
        }
    }
}

gmp_randclass rng(gmp_randinit_default);
int randint(int min, int max)
{
    // http://c-faq.com/lib/randrange.html
    // return min + rand() / (RAND_MAX / (max - min + 1) + 1);

    // rng.seed(time(NULL)); // called once, in main
    mpz_class n = rng.get_z_range(mpz_class(max - min + 1)) + min;
    return n.get_si();
}

// http://www.cplusplus.com/articles/D9j2Nwbp/
string to_string(int n)
{
    stringstream out;
    out << n;
    return out.str();
}

void buildLargeMenu(int numItems, double maxVal, double maxCost, vector<Food> *items)
{
    for (int i = 1; i <= numItems; i++) {
        items->push_back(Food(to_string(i), randint(1, maxVal), randint(1, maxCost)));
    }
}

struct Key {
    const vector<Food> left;
    double remainingCalories;
};

struct Result {
    double totalValue;
    vector<Food> taken;
};

bool operator<(const Key &k1, const Key &k2)
{
    return k1.remainingCalories < k2.remainingCalories;
}

double fastMaxVal(const vector<Food> &toConsider, double avail, vector<Food> *toTake, map<Key, Result> &memo)
{
    double totalValue;
    Key k = {toConsider, avail};

    if (toConsider.empty() || avail == 0) {
        totalValue = 0;
        toTake->clear();
    } else {
        try {
            Result r = memo.at(k);
            totalValue = r.totalValue;
            *toTake = r.taken;
        } catch(out_of_range&) {
            if (toConsider.at(0).getCost() > avail) {
                // explore right branch only
                vector<Food> nextToConsider(toConsider.begin() + 1, toConsider.end());
                totalValue = fastMaxVal(nextToConsider, avail, toTake, memo);
            } else {
                Food nextItem = toConsider.at(0);
                vector<Food> nextToConsider(toConsider.begin() + 1, toConsider.end());

                // explore left branch
                vector<Food> withToTake;
                double withVal = fastMaxVal(nextToConsider, avail - nextItem.getCost(), &withToTake, memo);
                withVal += nextItem.getValue();

                // explore right branch
                vector<Food> withoutToTake;
                double withoutVal = fastMaxVal(nextToConsider, avail - nextItem.getCost(), &withoutToTake, memo);

                if (withVal > withoutVal) {
                    totalValue = withVal;
                    copy(withToTake.begin(), withToTake.end(), back_inserter(*toTake));
                    toTake->push_back(nextItem);
                } else {
                    totalValue = withoutVal;
                    copy(withoutToTake.begin(), withoutToTake.end(), back_inserter(*toTake));
                }
            }
        }
    }

    Result r = {totalValue, *toTake};
    memo[k] = r;
    return totalValue;
}

double fastMaxVal(const vector<Food> &toConsider, double avail, vector<Food> *toTake)
{
    map<Key, Result> memo;
    return fastMaxVal(toConsider, avail, toTake, memo);
}

void testFastMaxVal(const vector<Food> &foods, double maxUnits, bool printItems = true)
{
    cout << "Use FAST search tree to allocate " << maxUnits << " calories" << endl;

    vector<Food> taken;
    double val = fastMaxVal(foods, maxUnits, &taken);

    cout << "Total value of items take = " << val << endl;
    if (printItems) {
        for(unsigned int i = 0; i < taken.size(); i++) {
            cout << "    " << taken.at(i) << endl;
        }
    }
}

int main()
{
    rng.seed(0);

    string names[] = {"wine", "beer", "pizza", "burger", "fries", "cola", "apple", "donut", "cake"};
    double values[] = {89, 90, 95, 100, 90, 79, 50, 10};
    double calories[] = {123, 154, 258, 354, 365, 150, 95, 195};

    vector<Food> foods;
    buildMenu(names, values, calories, sizeof(values) / sizeof(double), &foods);

    testGreedys(foods, 750);
    testGreedys(foods, 1000);
    cout << endl;
    testMaxVal(foods, 750);
    cout << endl;

    for (int numItems = 5; numItems <= 600; numItems += 5) {
        vector<Food> items;
        cout << "Try a menu with " << numItems << " items" << endl;
        buildLargeMenu(numItems, 90, 250, &items);
        // testMaxVal(items, 750, true);
        testFastMaxVal(items, 750, true);
    }
}


