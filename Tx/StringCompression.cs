using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.Conversion
{
    public static class StringCompression
    {
        public static string StringToGzipBase64String(string uncompressedString)
        {
            if (uncompressedString is null)
                return null;

            using (var uncompressedStream = new MemoryStream(Encoding.UTF8.GetBytes(uncompressedString)))
            using (var compressedStream = new MemoryStream())
            {
                using (var compressorStream = new GZipStream(compressedStream, CompressionMode.Compress, true))
                    uncompressedStream.CopyTo(compressorStream);

                return Convert.ToBase64String(compressedStream.ToArray());
            }
        }
    }
}
