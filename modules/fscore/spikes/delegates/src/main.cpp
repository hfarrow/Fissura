#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "fscore.h"

using namespace fs;

void myFunction(int arg)
{
    FS_PRINT("HELLO!  " << arg);
}

void myOtherFunction(int arg)
{
    FS_PRINT("Hello other " << arg);
}

void myFunction(int arg1, int arg2)
{
    FS_PRINT("HELLO!  " << arg1 << " " << arg2);
}

void myFunction()
{
    FS_PRINT("HELLO --");
}

class MyClass
{
    public:
        void myFunction(int arg)
        {
            FS_PRINT("MyClass " << arg);
        }

        void myConstFunction(int arg) const
        {
            FS_PRINT("MyClass const " << arg);
        }
};

void checkComparisons()
{
    using Delegate = Delegate<void(int)>;
    Delegate d1;
    Delegate d2;
    Delegate d3;
    d1.bind<&myFunction>();
    d2.bind<&myFunction>();
    d3.bind<&myOtherFunction>();

    if(d1 == d2)
        FS_PRINT("d1 == d2");

    if(d1 != d2)
        FS_PRINT("uh oh... d1 != d2");

    if(d1 == d3)
        FS_PRINT("uh oh... d1 == d3");

    if(d1 != d3)
        FS_PRINT("d1 != d3");
}

void checkCompareConstToNonConst()
{
    MyClass c1;
    MyClass c2;
    using ClassDelegate = Delegate<void(int)>;
    ClassDelegate classDelegate;
    classDelegate.bind<MyClass, &MyClass::myFunction>(&c1);
    ClassDelegate constDelegate;
    constDelegate.bind<MyClass, &MyClass::myConstFunction>(&c2);
    ClassDelegate constDelegate2;
    constDelegate2.bind<MyClass, &MyClass::myConstFunction>(&c2);
    ClassDelegate constDelegate3;
    constDelegate3.bind<MyClass, &MyClass::myConstFunction>(&c1);

    if(constDelegate != classDelegate)
        FS_PRINT("constDelegate != classDelegate");

    if(constDelegate == constDelegate2)
        FS_PRINT("constDelegate == constDelegate2");

    if(constDelegate2 != constDelegate3)
        FS_PRINT("constDelegate2 != constDelegate3");
}

void checkInvoke()
{
    using MyDelegate1 = Delegate<void(int)>;
    MyDelegate1 delegate1;
    delegate1.bind<&myFunction>();
    delegate1.invoke(10);

    using MyDelegate2 = Delegate<void(int, int)>;
    MyDelegate2 delegate2;
    delegate2.bind<&myFunction>();
    delegate2.invoke(10, 20);

    using MyDelegate0 = Delegate<void()>;
    MyDelegate0 delegate0;
    delegate0.bind<&myFunction>();
    delegate0.invoke();

    using ClassDelegate = Delegate<void(int)>;
    ClassDelegate classDelegate;
    MyClass c;
    classDelegate.bind<MyClass, &MyClass::myFunction>(&c);
    classDelegate.invoke(22);

    ClassDelegate constDelegate;
    constDelegate.bind<MyClass, &MyClass::myConstFunction>(&c);
    constDelegate(24);
}

void checkMake()
{
    using MyDelegate = Delegate<void(int)>;
    auto delegateFree = MyDelegate::make<&myFunction>();
    delegateFree.invoke(77);

    MyClass c;
    auto delegateClass = MyDelegate::make<MyClass, &MyClass::myFunction>(&c);
    delegateClass.invoke(78);

    auto delegateConstClass = MyDelegate::makeConst<MyClass, &MyClass::myConstFunction>(&c);
    delegateConstClass.invoke(79);
}

int main( int, char **)
{
    checkInvoke();
    checkComparisons();
    checkCompareConstToNonConst();
    checkMake();
    return 0;
}

