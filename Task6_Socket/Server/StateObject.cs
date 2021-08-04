using System.Net.Sockets;
using System.Text;

namespace Server
{
    internal class StateObject
    {
        internal const int BufferSize = 4096;

        internal byte[] buffer = new byte[BufferSize];

        internal StringBuilder stringBuilder = new StringBuilder();

        internal Socket workSocket = null;
    }
}
