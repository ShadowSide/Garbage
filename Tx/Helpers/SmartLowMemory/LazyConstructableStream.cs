using Global.BusinessCommon.Helpers.Constraints;
using System;
using System.IO;
using System.Threading;

namespace Global.BusinessCommon.Helpers.SmartLowMemory
{
    public class LazyConstructableStream: Stream
    {
        private readonly Func<long?> _optimizedSizeGetter;
        private Lazy<Stream> _stream;
        private long _virtualPosition = 0;

        private Stream PermissiveStream => _stream.IsValueCreated ? _stream.Value : null;

        public LazyConstructableStream(Func<Stream> streamFactory, Func<long?> optimizedSizeGetterWithoutStreamCreating = null)
        {
            _optimizedSizeGetter = optimizedSizeGetterWithoutStreamCreating;
            NotNull.CheckArgument(() => streamFactory);
            _stream = new Lazy<Stream>(
                () =>
                {
                    var stream = streamFactory();
                    stream.Position = _virtualPosition;
                    return stream;
                }, LazyThreadSafetyMode.None);
        }

        public override void Flush() => PermissiveStream?.Flush();

        public override long Seek(long offset, SeekOrigin origin) => (PermissiveStream is null) ? VirtualSeek(offset, origin) : _stream.Value.Seek(offset, origin);

        private long VirtualSeek(long offset, SeekOrigin origin)
        {
            switch (origin)
            {
                case SeekOrigin.Begin:
                    _virtualPosition = offset;
                    break;

                case SeekOrigin.Current:
                    _virtualPosition = _virtualPosition + offset;
                    break;

                case SeekOrigin.End:
                    _virtualPosition = Length - offset;
                    break;

                default:
                    throw new ArgumentException($"Invalid SeekOrigin value '{origin}'");
            }

            if (PermissiveStream != null)
            {
                _stream.Value.Position = _virtualPosition;
            }

            return _virtualPosition;
        }


        public override void SetLength(long value) => _stream.Value.SetLength(value);
        public override int Read(byte[] buffer, int offset, int count) => _stream.Value.Read(buffer, offset, count);
        public override void Write(byte[] buffer, int offset, int count) => _stream.Value.Write(buffer, offset, count);

        public override bool CanRead => _stream.Value.CanRead;
        public override bool CanSeek => _stream.Value.CanSeek;
        public override bool CanWrite => _stream.Value.CanWrite;
        public override long Length => PermissiveStream?.Length ?? _optimizedSizeGetter?.Invoke() ?? _stream.Value.Length;

        public override long Position
        {
            get => (PermissiveStream is null) ? _virtualPosition : _stream.Value.Position;
            set
            {
                if (PermissiveStream is null)
                {
                    _virtualPosition = value;
                }
                else
                {
                    _stream.Value.Position = value;
                }
            }
        }

        public override bool CanTimeout => _stream.Value.CanTimeout;
        public override int ReadTimeout
        {
            get => _stream.Value.ReadTimeout;
            set => _stream.Value.ReadTimeout = value;
        }
        public override int WriteTimeout
        {
            get => _stream.Value.WriteTimeout;
            set => _stream.Value.WriteTimeout = value;
        }

        protected override void Dispose(bool disposing)
        {
            var stream = PermissiveStream;
            _stream = null;
            stream?.Dispose();
        }
    }
}
