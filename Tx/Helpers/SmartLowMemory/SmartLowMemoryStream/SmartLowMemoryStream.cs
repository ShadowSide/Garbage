using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Global.BusinessCommon.Helpers.SmartLowMemory.IO.Detail;
using BufferedStream = System.IO.BufferedStream;
using File = Delimon.Win32.IO.File;
using FileAccess = Delimon.Win32.IO.FileAccess;
using FileMode = Delimon.Win32.IO.FileMode;
using FileOptions = Delimon.Win32.IO.FileOptions;
using FileShare = Delimon.Win32.IO.FileShare;
using Path = Delimon.Win32.IO.Path;
using Stream = System.IO.Stream;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.IO
{
    public static class SmartLowMemoryStream
    {
        private const int BufferSize = 64 * 1024;

        public static BufferedStream Open(Stream movedOwnStream)
        {
            return new BufferedStream(movedOwnStream ?? throw new ArgumentNullException(nameof(movedOwnStream)), BufferSize);
        }

        public static Stream Open()
        {
            var innerStream = File.Open(Path.GetTempFileName(), FileMode.OpenOrCreate, FileAccess.ReadWrite, FileOptions.DeleteOnClose);
            try
            {
                var outerStream = new BufferedStream(innerStream, BufferSize);
                try
                {
                    return new NotFlushingBufferOnDisposeStream(outerStream, innerStream);
                }
                catch
                {
                    outerStream.Dispose();
                    throw;
                }
            }
            catch
            {
                innerStream?.Dispose();
                throw;
            }
        }

        public static BufferedStream Open(string path, FileAccess access = FileAccess.Read)
        {
            Stream file;
            switch (access)
            {
                case FileAccess.Read:
                    file = File.Open(path, FileMode.Open, FileAccess.Read, FileShare.Read);
                    break;
                case FileAccess.Write:
                case FileAccess.ReadWrite:
                    file = File.Open(path, FileMode.Open, access, FileShare.None);
                    break;
                default:
                    throw new NotImplementedException($"{nameof(access)} mode is not implemented for {access}");
            }
            try
            {
                return new BufferedStream(file, BufferSize);
            }
            catch
            {
                file?.Dispose();
                throw;
            }
        }
    }
}
