
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <list>

#include "../Client/ClientClass.h"
#include "../Client/ClientClass.cpp"

using namespace std;
using namespace client;

void LoadTest(int requestRatePerSecond)
{
    int arrayLength = 5;
    vector<ClientClass> clientMembers;
    for (int i = 0; i < 5; i++)
    {
        ClientClass client = ClientClass(i);
        clientMembers.push_back(client);
    }

    vector<thread> threads;
    for (int i = 0; i < 5; i++)
    {
        thread clientThread(&ClientClass::RunTest, clientMembers[i], requestRatePerSecond,
            arrayLength);
        threads.push_back(move(clientThread));
    }

    for (auto& th : threads)
    {
        th.join();
    }

    return;
}

void RandomClientNumber()
{
    vector<ClientClass> clientMembers;
    srand((unsigned)time(0));
    int  randomClientNumber = rand() % 50 + 1;
    cout << endl << "Random client number : " << randomClientNumber;

    for (int i = 0; i < randomClientNumber; i++)
    {
        ClientClass client = ClientClass(i);
        clientMembers.push_back(client);
    }

    vector<thread> threads;
    for (int i = 0; i < randomClientNumber; i++)
    {
        thread clientThread(&ClientClass::RunTest, clientMembers[i], 2, 5);
        threads.push_back(move(clientThread));

    }

    for (auto& th : threads)
    {
        th.join();
    }

    return;
}

void RandomArrayLength()
{
    int requestRate = 2;
    srand((unsigned)time(0));
    int  randomArrayLength = rand() % 100 + 1;
    cout << endl << "Random array size : " << randomArrayLength;
    
    ClientClass client = ClientClass(1);
    client.RunTest(requestRate, randomArrayLength);

    return;
}

void RandomRequestRate()
{
    int arrayLength = 10;
    srand((unsigned)time(0));
    int  randomRequestRate = rand() % 100 + 1;
    cout << endl << "Random request rate : " << randomRequestRate;

    ClientClass client = ClientClass(1);
    client.RunTest(randomRequestRate, arrayLength);

    return;
}



int main()
{
    LoadTest(200);

    //RandomArrayLength();

    //RandomRequestRate();

    //RandomClientNumber();
    
    return 0;
}
