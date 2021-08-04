#include "ClientClass.h"
#include <winsock.h>
#include <iostream>
#include <string>
#include <list>
#include <random>
#include <future>
#include <thread>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define PORT 11000

namespace client
{
    ClientClass::ClientClass(int id)
    {
        clientId = id;
    }

    int ClientClass::Starting()
    {
        WSADATA ws;
        return WSAStartup(MAKEWORD(2, 2), &ws);
    }

    int ClientClass::SocketInitiation()
    {
        return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }

    void ClientClass::AddressSet()
    {
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_port = htons(PORT);
        socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(&(socketAddress.sin_zero), 0, 8);
    }

    string ClientClass::ServiceOneString()
    {
        int arrayLength;
        cout << endl << "Input length of array : ";
        cin >> arrayLength;

        list <double> userArray;
        cout << endl << "Input data of array : " << endl;
        for (int i = 0; i < arrayLength; i++)
        {
            double input;
            cin >> input;
            userArray.push_back(input);
        }

        json jsonInput;
        jsonInput["ServiceOne"] = userArray;
        return jsonInput.dump();
    }

    string ClientClass::ServiceTwoString()
    {
        double firstNumber, secondNumber;
        list <double> userArray;
        cout << endl << "Input First Number : ";
        cin >> firstNumber;
        userArray.push_back(firstNumber);

        cout << endl << "Input Second Number : ";
        cin >> secondNumber;
        userArray.push_back(secondNumber);

        json jsonInput;
        jsonInput["ServiceTwo"] = userArray;
        return jsonInput.dump();
    }

    string ClientClass::RequestService(string userInput)
    {
        char buffer[4096];
        string response;

        userInput += "<EOF>";
        char const* sendBuffer = userInput.c_str();

        int sendResult = send(ClientClass::clientSocket, sendBuffer, userInput.size() + 1, 0);

        if (sendResult != SOCKET_ERROR)
        {
            ZeroMemory(buffer, 4069);
            int bytesReceived = recv(ClientClass::clientSocket, buffer, 4069, 0);
            if (bytesReceived > 0)
            {
                response = string(buffer, 0, bytesReceived);
            }
        }

        else
        {
            response = "Error occured!";
        }

        return response;
    }

    string ClientClass::AutoServiceOneString(int arrayLength)
    {
        list <double> userArray;

        double lowerBound = 0;
        double upperBound = 10000;
        uniform_real_distribution<double> uniformDistribution(lowerBound, upperBound);

        for (int i = 0; i < arrayLength; i++)
        {
            default_random_engine randomEngine;
            double randomDouble = uniformDistribution(randomEngine);
            userArray.push_back(randomDouble);
        }

        json jsonInput;
        jsonInput["ServiceOne"] = userArray;
        return jsonInput.dump();
    }

    string ClientClass::AutoServiceTwoString()
    {
        list <double> userArray;

        double lowerBound = 0;
        double upperBound = 10000;
        uniform_real_distribution<double> uniformDistribution(lowerBound, upperBound);
        default_random_engine randomEngine;

        double firstNumber = uniformDistribution(randomEngine);
        userArray.push_back(firstNumber);
        double secondNumber = uniformDistribution(randomEngine);
        userArray.push_back(secondNumber);

        json jsonInput;
        jsonInput["ServiceTwo"] = userArray;
        return jsonInput.dump();
    }

    void ClientClass::ShowResponse(string receivedResponse)
    {
        json jsonResult;
        jsonResult = json::parse(receivedResponse);

        if (jsonResult.contains("ServiceOne"))
        {
            double result = jsonResult["ServiceOne"][0];
            cout << endl << "Sum of array : " << result;

            return;
        }
        else
        {
            list<double> result = jsonResult["ServiceTwo"];
            cout << endl << "first + second num : " << result.front();
            result.pop_front();
            cout << endl << "first - second num : " << result.front();
            result.pop_front();
            cout << endl << "second - first num : " << result.front();
            result.pop_front();
            cout << endl << "first * second num : " << result.front();
            result.pop_front();
            cout << endl << "first / second num : " << result.front();
            result.pop_front();
            cout << endl << "second / first num : " << result.front();

            return;
        }
    }

    void ClientClass::RunTest(int requestRate, int arrayLength)
    {
        int maxRequest = 500;
        string serviceOneInput = AutoServiceOneString(arrayLength);
        string serviceTwoInput = AutoServiceTwoString();

        startResult = Starting();
        AddressSet();
        clientSocket = SocketInitiation();
        connectResult = connect(clientSocket, (struct sockaddr*)&socketAddress,
            sizeof(socketAddress));

        for (int i = 0; i < maxRequest; i++)
        {
            future<string> RequestServiceOneAsync = async(launch::async,
                [this](string param) { return this->RequestService(param); },
                serviceOneInput);
            ShowResponse(RequestServiceOneAsync.get());

            this_thread::sleep_for(chrono::milliseconds(1000 / requestRate));

            future<string> RequestServiceTwoAsync = async(launch::async,
                [this](string param) { return this->RequestService(param); },
                serviceTwoInput);

            ShowResponse(RequestServiceTwoAsync.get());

            this_thread::sleep_for(chrono::milliseconds(1000 / requestRate));
        }

        future<string> RequestServiceOneAsync = async(launch::async,
            [this](string param) { return this->RequestService(param); },
            "");

        closesocket(clientSocket);
        WSACleanup();

        return;
    }


    void ClientClass::Run()
    {
        string serviceInput;
        string userInput;
        string receivedResponse;

        startResult = Starting();
        if (startResult < 0)
        {
            cout << endl << "Startinig Failed!" << endl;
            return;
        }

        AddressSet();

        clientSocket = SocketInitiation();
        if (clientSocket < 0)
        {
            cout << endl << "Socket Initiation Failed!" << endl;
            return;
        }

        connectResult = connect(clientSocket, (struct sockaddr*)&socketAddress,
            sizeof(socketAddress));
        if (connectResult < 0)
        {
            cout << endl << "Connection Failed!" << endl;
            closesocket(clientSocket);
            WSACleanup();
            return;
        }

        do
        {
            cout << endl << "Select Service 1 or 2 (close with other input!) : ";
            cin >> serviceInput;

            if (serviceInput == "1")
            {
                userInput = ServiceOneString();
            }
            else if (serviceInput == "2")
            {
                userInput = ServiceTwoString();
            }

            else
            {
                cout << "Invalid input";
                break;
            }

            future<string> RequestAsync = async(launch::async,
                [this](string param) { return this->RequestService(param); },
                userInput);


            receivedResponse = RequestAsync.get();
            ShowResponse(receivedResponse);

        } while (true);

        closesocket(clientSocket);
        WSACleanup();
    }
}
