using System.IO;
using GlobalInterfaces;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager
{
    class Document : INamedStream
    {
        public string Name { get; set; }
        public Stream Stream { get; set; }

        public void Dispose()
        {
            Name = null;
            var stream = Stream;
            Stream = null;
            stream?.Dispose();
        }
    }
}
