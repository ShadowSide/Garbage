using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.IO.Detail
{
    public class NotFlushingBufferOnDisposeStream : Stream
    {
        private Stream _outerStream;
        private Stream _innerStream;

        public NotFlushingBufferOnDisposeStream(Stream outerStream, Stream innerStream)
        {
            _outerStream = outerStream;
            _innerStream = innerStream;
        }

        public override void Flush() => _outerStream.Flush();
        public override long Seek(long offset, SeekOrigin origin) => _outerStream.Seek(offset, origin);
        public override void SetLength(long value) => _outerStream.SetLength(value);
        public override int Read(byte[] buffer, int offset, int count) => _outerStream.Read(buffer, offset, count);
        public override void Write(byte[] buffer, int offset, int count) => _outerStream.Write(buffer, offset, count);

        public override bool CanRead => _outerStream.CanRead;
        public override bool CanSeek => _outerStream.CanSeek;
        public override bool CanWrite => _outerStream.CanWrite;
        public override long Length => _outerStream.Length;
        public override long Position
        {
            get => _outerStream.Position;
            set => _outerStream.Position = value;
        }

        public override bool CanTimeout => _outerStream.CanTimeout;
        public override int ReadTimeout
        {
            get => _outerStream.ReadTimeout;
            set => _outerStream.ReadTimeout = value;
        }
        public override int WriteTimeout
        {
            get => _outerStream.WriteTimeout;
            set => _outerStream.WriteTimeout = value;
        }

        protected override void Dispose(bool disposing)
        {
            var innerStream = _innerStream;
            var outerStream = _outerStream;

            _innerStream = null;
            _outerStream = null;
            innerStream?.Dispose();
            /*try
            {
                outerStream.Dispose();
            }
            catch
            {
                //ignore
            }*/
        }
    }
}
