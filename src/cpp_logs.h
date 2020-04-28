#include <map>
#include <set>
#include <vector>
#include <iostream>

typedef int DEBUG_LEVEL;
int d_level = 0;

template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const std::pair<const Key, Value>& p)
{
    os << p.first << " => " << p.second;
    return os;
}

template<typename Container>
void log(DEBUG_LEVEL level, const Container& c) {
    if (level >= d_level) {
        for(typename Container::const_iterator it = c.begin();
            it != c.end(); ++it)
        std::cout << *it << '\n';
    }
}

// OPTIONAL Adding your own types

class MyClass
{
    int i;
    std::string s;
public:
    MyClass(int i, std::string s): i(i), s(s) {}

    // declare the operator<<() overload as a friend to grant access
    // to private members
    friend std::ostream& operator<<(std::ostream& os, const MyClass& mc);

};

// define the operator<<() for MyClass as a global function (required)
std::ostream& operator<<(std::ostream& os, const MyClass& mc)
{
    os << "{MyClass: " << mc.s << ": " << mc.i << "}";
    return os;
}
