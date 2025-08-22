#include <bits/stdc++.h>
#include "json.hpp" // nlohmann/json.hpp

using json = nlohmann::json;
using namespace std;

using i128 = __int128_t; // big 128-bit integer

// ---------- helpers for __int128 ----------
string toString(i128 x)
{
    if (x == 0)
        return "0";
    bool neg = x < 0;
    if (neg)
        x = -x;
    string s;
    while (x > 0)
    {
        int d = x % 10;
        s.push_back('0' + d);
        x /= 10;
    }
    if (neg)
        s.push_back('-');
    reverse(s.begin(), s.end());
    return s;
}

// decode "value" string in given base to __int128
i128 decodeValue(const string &val, int base)
{
    i128 result = 0;
    for (char c : val)
    {
        int digit;
        if (isdigit(c))
            digit = c - '0';
        else if (isalpha(c))
            digit = tolower(c) - 'a' + 10;
        else
            throw runtime_error("Invalid character");
        if (digit >= base)
            throw runtime_error("Invalid digit for base");
        result = result * base + digit;
    }
    return result;
}

// Lagrange interpolation at x=0, exact with rationals
struct Rational
{
    i128 num, den;
    Rational(i128 n = 0, i128 d = 1) : num(n), den(d)
    {
        if (den < 0)
        {
            num = -num;
            den = -den;
        }
        i128 g = std::gcd((long long)num, (long long)den); // small gcd okay
        if (g)
        {
            num /= g;
            den /= g;
        }
    }
    Rational operator+(const Rational &o) const
    {
        return Rational(num * o.den + o.num * den, den * o.den);
    }
    Rational operator*(const Rational &o) const
    {
        return Rational(num * o.num, den * o.den);
    }
};

int main()
{
    ifstream inFile("testcase2.json"); // change filename if needed
    if (!inFile)
    {
        cerr << "Error: Cannot open input file\n";
        return 1;
    }

    json j;
    inFile >> j;

    int n = j["keys"]["n"];
    int k = j["keys"]["k"];

    vector<pair<i128, i128>> points;
    for (auto &[key, val] : j.items())
    {
        if (key == "keys")
            continue;
        int x = stoi(key);
        int base = stoi(val["base"].get<string>());
        string value = val["value"];
        i128 y = decodeValue(value, base);
        points.push_back({(i128)x, y});
    }

    sort(points.begin(), points.end());

    // Lagrange interpolation at 0
    Rational secret(0, 1);
    for (int i = 0; i < k; i++)
    {
        i128 xi = points[i].first;
        i128 yi = points[i].second;
        Rational term(yi, 1);
        for (int j = 0; j < k; j++)
        {
            if (i == j)
                continue;
            i128 xj = points[j].first;
            term = term * Rational(-xj, xi - xj);
        }
        secret = secret + term;
    }

    cout << "Secret (c) = ";
    if (secret.den == 1)
        cout << toString(secret.num) << "\n";
    else
        cout << toString(secret.num) << "/" << toString(secret.den) << "\n";
}
