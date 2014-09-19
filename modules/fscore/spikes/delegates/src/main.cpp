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
    auto delegateFree = MyDelegate::from<&myFunction>();
    delegateFree.invoke(77);

    MyClass c;
    auto delegateClass = MyDelegate::from<MyClass, &MyClass::myFunction>(&c);
    delegateClass.invoke(78);

    auto delegateConstClass = MyDelegate::from<MyClass, &MyClass::myConstFunction>(&c);
    delegateConstClass.invoke(79);

    auto delegateReturn = Delegate<int(void)>::from<&myReturnFunction>();
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
    // channel.add<&myFunction>();
    // bool hasListener = channel.hasListener<&myFunction>();
    // FS_PRINT("hasListener = " << hasListener);
    // auto delegate = MyChannel::DelegateType::from<&myFunction>();
    // hasListener = channel.hasListener(delegate);
    // FS_PRINT("hasListener = " << hasListener);
    // channel.removeListener<&myFunction>();
    // hasListener = channel.hasListener<&myFunction>();
    // FS_PRINT("hasListener = " << hasListener);
    //
    // channel.add<MyClass, &MyClass::myFunction>(&c);

    MyClass c;
    MyEvent event(memory::getDebugArena());

    auto channel = event.makeChannel(10);
    channel.add<&myFunction>();
    channel.add<&myOtherFunction>();
    channel.add<MyClass, &MyClass::myFunction>(&c);
    channel.add(MyEvent::DelegateType::from<MyClass, &MyClass::myConstFunction>(&c));

    event.add(&channel);
    // event.add<&myFunction>();
    // event.add<MyClass, &MyClass::myConstFunction>(&c);
    // event.add<MyClass, &MyClass::myFunction>(&c);
    // event.add(MyEvent::DelegateType::from<MyClass, &MyClass::myConstFunction>(&c));
    event.signal(111);
}

    using MyArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
                                MultiThread<MutexPrimitive>,
                                SimpleBoundsChecking,
                                FullMemoryTracking,
                                MemoryTagging>;

    using MyDelegate = Delegate<void(int), MyArena>;
    using MyDelegate2 = Delegate<void(void), MyArena>;
void checkLambda()
{

    auto F = [](){ return [](int ii){FS_PRINT("F " << ii); ++ii;};};

    HeapArea area(FS_SIZE_OF_MB * 32);
    MyArena arena(area, "DelegateArena");

    MyDelegate d1(&arena);
    d1 = F(); // Should reset
    d1.invoke(2);
    d1 = F(); // Should delete
    d1.invoke(3);

    MyDelegate d2(F(), &arena);
    d2(19);

    d2 = d1;
    d2(4);

    MyDelegate2 d3;
    {
    auto lambda = [](){FS_PRINT("lamda... "); FS_ASSERT(!"Intentional Assert");};
    d3 = MyDelegate2(lambda, &arena);
    FS_PRINT("<<<<<<<<<<<<<");
    }

    // auto report = arena.generateArenaReport();
    // fs::memory::logArenaReport(report);
}

void checkLambdaEvent()
{
    using MyArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
                                MultiThread<MutexPrimitive>,
                                SimpleBoundsChecking,
                                FullMemoryTracking,
                                MemoryTagging>;

    using MyEvent = Event<MyArena, void(int)>;
    using MyDelegate = MyEvent::DelegateType;
    using MyChannel = MyEvent::Channel;

    HeapArea area(FS_SIZE_OF_MB * 32);
    MyArena arena(area, "DelegateArena");

    MyClass c;
    MyEvent event(&arena);

    auto channel = event.makeChannel(10);
    channel.add<&myFunction>();
    channel.add<&myOtherFunction>();
    channel.add<MyClass, &MyClass::myFunction>(&c);
    channel.add(MyEvent::DelegateType::from<MyClass, &MyClass::myConstFunction>(&c));
    channel.add(MyDelegate([](int i){FS_PRINT("waka waka " << i);}, &arena));
    channel.add([](int i){FS_PRINT("whoop " << i);});

    auto lambda = [](int i){FS_PRINT("lambda " << i); ++i;};
    auto lambda2 = [](int i){FS_PRINT("lambda " << i); ++i;};
    // The above lambdas "look" identical but should be seperate functions

    event.add(lambda);
    if(!event.has(lambda))
        FS_PRINT("ERROR: should have been the same object");
    //
    MyDelegate d1(lambda, &arena);
    MyDelegate d11(lambda, &arena);
    if(d1 != d11)
        FS_PRINT("ERROR: d1 and d11 should have been the same");

    MyDelegate d2(lambda, &arena);
    MyDelegate d22(lambda2, &arena);

    if(d2 == d22)
        FS_PRINT("d2 and d22 should not be the same");

    event.add(&channel);
    event.add([](int i){FS_PRINT("Direct Add " << i); ++i;});
    event.signal(111);

    if(event.has([](int i){++i;}))
        FS_PRINT("ERROR: should be a unique signature");

    // memory::logArenaReport(arena.generateArenaReport());
}

int main( int, char **)
{
    // checkInvoke();
    // checkComparisons();
    // checkCompareConstToNonConst();
    // checkMake();
    // checkEvent();
    checkLambda();
    // checkLambdaEvent();
    return 0;
}

