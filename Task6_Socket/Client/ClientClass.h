#pragma once
#include <winsock.h>
#include <iostream>
#include <string>

using namespace std;

namespace client
{
    class ClientClass
    {
        struct sockaddr_in socketAddress;

    private:
        int clientId;
        int startResult;
        int clientSocket;
        int connectResult;

    public:
        ClientClass(int id);

        int Starting();

        int SocketInitiation();

        void AddressSet();

        string ServiceOneString();

        string ServiceTwoString();

        string RequestService(string userInput);

        string AutoServiceOneString(int arrayLength);

        string AutoServiceTwoString();

        void ShowResponse(string receivedResponse);

        void RunTest(int requestRate, int arrayLength);

        void Run();

    };
}


