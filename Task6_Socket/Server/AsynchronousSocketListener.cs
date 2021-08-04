// Link of sourrce : https://docs.microsoft.com/en-us/dotnet/framework/network-programming/asynchronous-server-socket-example

using System;
using System.Net;
using System.Text;
using System.Linq;
using System.Net.Sockets;
using System.Threading;
using Newtonsoft.Json;
using System.Collections.Generic;

namespace Server
{
    internal static class AsynchronousSocketListener
    {
        static ManualResetEvent threadSignal = new ManualResetEvent(false);

        static IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
        static IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, 11000);

        static Socket serverSocket = new Socket(AddressFamily.InterNetwork,
                SocketType.Stream, ProtocolType.Tcp);

        internal static void StartListening()
        {
            try
            {
                serverSocket.Bind(localEndPoint);
                serverSocket.Listen(10);
                Console.WriteLine("Server is ready!");

                while (true)
                {
                    threadSignal.Reset();

                    serverSocket.BeginAccept(new AsyncCallback(AcceptCallback),
                        serverSocket);

                    threadSignal.WaitOne();
                    Console.WriteLine("New client connected...");
                }

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            Console.WriteLine("\nPress ENTER to continue...");
            Console.Read();
        }

        static void AcceptCallback(IAsyncResult ar)
        {
            threadSignal.Set();

            Socket socket = (Socket)ar.AsyncState;
            Socket handler = socket.EndAccept(ar);

            StateObject state = new StateObject();
            state.workSocket = handler;
            handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                new AsyncCallback(ReadCallback), state);
        }

        static void ReadCallback(IAsyncResult ar)
        {            
            StateObject state = (StateObject)ar.AsyncState;
            Socket handler = state.workSocket;
            int bytesRead = handler.EndReceive(ar);
            String content = String.Empty;

            if (bytesRead > 0)
            {         
                state.stringBuilder.Append(Encoding.ASCII.GetString(
                    state.buffer, 0, bytesRead));

                if (state.stringBuilder[state.stringBuilder.Length - 2] == '>')
                {
                    Dictionary<string, double[]> result = new Dictionary<string, double[]>();
                    string sendData;

                    content = state.stringBuilder.ToString(0, state.stringBuilder.Length - 6);
                    var clientData = JsonConvert.DeserializeObject<Dictionary<string, double[]>>(content);

                    if (clientData.ContainsKey("ServiceOne"))
                    {
                        result["ServiceOne"] = ArraySummation(clientData["ServiceOne"]);
                    }
                    else
                    {
                        result["ServiceTwo"] = ArithmaticCalculation(clientData["ServiceTwo"][0],
                            clientData["ServiceTwo"][1]);
                    }

                    sendData = JsonConvert.SerializeObject(result);
                    Send(handler, sendData);

                    state.stringBuilder.Clear();
                    handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                    new AsyncCallback(ReadCallback), state);
                }
                else
                {
                    handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                    new AsyncCallback(ReadCallback), state);
                }
            }
            else
            {
                handler.Shutdown(SocketShutdown.Both);
                handler.Close();
                Console.WriteLine("A client disconnected.");
            }

        }

        static void Send(Socket handler, String data)
        {
            byte[] byteData = Encoding.ASCII.GetBytes(data);

            handler.BeginSend(byteData, 0, byteData.Length, 0,
                new AsyncCallback(SendCallback), handler);
        }

        static void SendCallback(IAsyncResult ar)
        {
            try
            {
                Socket handler = (Socket)ar.AsyncState;

                int bytesSent = handler.EndSend(ar);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        static double[] ArraySummation(double[] array)
        {
            double[] result = new double[1] {array.Sum()};
            return result;
        }

        static double[] ArithmaticCalculation(double firstNumber, double secondNumber)
        {
            double[] result = new double[6];
            result[0] = firstNumber + secondNumber;
            result[1] = firstNumber - secondNumber;
            result[2] = secondNumber - firstNumber;
            result[3] = firstNumber * secondNumber;
            
            if (secondNumber != 0)
            {
                result[4] = firstNumber / secondNumber;
            }
            else
            {
                result[4] = 0;
            }

            if (firstNumber != 0)
            {
                result[5] = secondNumber / firstNumber;
            }
            else
            {
                result[5] = 0;
            }
            
            return result;
        }
    }
}
