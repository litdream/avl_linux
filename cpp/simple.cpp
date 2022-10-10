#include <iostream>
#include <string>
#include "avl.hpp"

using std::string, std::ostream;

class Person {
public:
    Person() : name("Ray"), age(49) {}
    Person(string n, int a) : name(n), age(a) {}
    Person(const char *n, int a): name(n), age(a) {}
    const string getName() const { return name; }
    int getAge() const { return age; }
    friend ostream& operator<<(ostream &os, const Person &me) {
        os << me.getName() << "(" << me.getAge() << ")";
        return os;
    }
    
private:
    string name;
    int age;
    
};

int main(int argc, char **argv)
{
    AVLTree<Person> avl;

    Person a;
    avl.insertNode( a.getName().c_str(), &a);

    Person b("Jake", 17);
    avl.insertNode( b.getName().c_str(), &b);
    
    return 0;
}
