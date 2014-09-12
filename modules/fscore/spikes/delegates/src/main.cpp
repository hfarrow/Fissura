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

int myReturnFunction()
{
    return 10;
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

        int myReturnFunction()
        {
            return 777;
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

    Delegate<int(void)> delegateReturn;
    delegateReturn.bind<MyClass, &MyClass::myReturnFunction>(&c);
    int val = delegateReturn();
    FS_PRINT("delegateReturn returned " << val);
}

void checkMake()
{
    using MyDelegate = Delegate<void(int)>;
    auto delegateFree = MyDelegate::make<&myFunction>();
    delegateFree.invoke(77);

    MyClass c;
    auto delegateClass = MyDelegate::make<MyClass, &MyClass::myFunction>(&c);
    delegateClass.invoke(78);

    auto delegateConstClass = MyDelegate::make<MyClass, &MyClass::myConstFunction>(&c);
    delegateConstClass.invoke(79);

    auto delegateReturn = Delegate<int(void)>::make<&myReturnFunction>();
    int val = delegateReturn.invoke();
    FS_PRINT("delegateReturn returned " << val);
}

void checkEvent()
{
    using MyDelegate = Delegate<void(int)>;
    using MyEvent = Event<DebugArena, void(int)>;
    using MyChannel = MyEvent::Channel;

    // MyClass c;
    //
    // MyChannel channel(*memory::getDebugArena(), 10);
    // channel.addListener<&myFunction>();
    // bool hasListener = channel.hasListener<&myFunction>();
    // FS_PRINT("hasListener = " << hasListener);
    // auto delegate = MyChannel::DelegateType::make<&myFunction>();
    // hasListener = channel.hasListener(delegate);
    // FS_PRINT("hasListener = " << hasListener);
    // channel.removeListener<&myFunction>();
    // hasListener = channel.hasListener<&myFunction>();
    // FS_PRINT("hasListener = " << hasListener);
    //
    // channel.addListener<MyClass, &MyClass::myFunction>(&c);

    MyClass c;
    MyEvent event(memory::getDebugArena());

    auto channel = event.makeChannel(10);
    channel.addListener<&myFunction>();
    channel.addListener<&myOtherFunction>();
    channel.addListener<MyClass, &MyClass::myFunction>(&c);
    channel.addListener(MyEvent::DelegateType::make<MyClass, &MyClass::myConstFunction>(&c));

    event.add(&channel);
    // event.add<&myFunction>();
    // event.add<MyClass, &MyClass::myConstFunction>(&c);
    // event.add<MyClass, &MyClass::myFunction>(&c);
    // event.add(MyEvent::DelegateType::make<MyClass, &MyClass::myConstFunction>(&c));
    event.signal(111);
}

void checkLambda()
{
    using MyDelegate = Delegate<void(int)>;
    // MyDelegate delegate([](int i) {FS_PRINT("lambda " << i); i++;});
    // delegate.invoke(1);

    auto F = [](){ return [](int ii){FS_PRINT("F " << ii); ++ii;};};

    MyDelegate d2 = MyDelegate::make<&myFunction>();
    d2(22);
    d2 = F(); // Should reset
    d2.invoke(2);
    d2 = F(); // Should delete
    d2.invoke(2);
}

int main( int, char **)
{
    checkInvoke();
    checkComparisons();
    checkCompareConstToNonConst();
    checkMake();
    checkEvent();
    checkLambda();
    return 0;
}

