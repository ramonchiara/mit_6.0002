#include <algorithm>
#include <iostream>
#include <vector>

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
    double val;

    val = greedy(items, constraint, keyFunction, &taken);

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
    vector<Food> taken;
    double val;

    cout << "Use search tree to allocate " << maxUnits << " calories" << endl;

    val = maxVal(foods, maxUnits, &taken);

    cout << "Total value of items take = " << val << endl;
    if (printItems) {
        for(unsigned int i = 0; i < taken.size(); i++) {
            cout << "    " << taken.at(i) << endl;
        }
    }
}

int main()
{
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
}

