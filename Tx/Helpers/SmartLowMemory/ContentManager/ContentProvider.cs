using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Global.BusinessCommon.Helpers.Constraints;
using Global.BusinessCommon.Helpers.SmartLowMemory.IO;
using FileAccess = Delimon.Win32.IO.FileAccess;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager
{
    public abstract class ContentProviderBase : IContentProvider
    {
        public long? OptimizedContentLengthProvider => Initialized ? null : OptimizedContentLengthBeforeInitialization;
        public abstract long? OptimizedContentLengthBeforeInitialization { get; }

        public abstract void CallOnceContentPreparation(string contentDestinationFilePath);

        public void ContentPreparation(string contentDestinationFilePath)
        {
            if (_callOnceFlag)
                return;
            lock (SyncRoot)
            {
                CallOnceContentPreparation(contentDestinationFilePath);
                _callOnceFlag = true;
            }
        }

        public bool Initialized => _callOnceFlag;

        private volatile bool _callOnceFlag;
        protected readonly object SyncRoot = new object();
    }

    public delegate void FileWriterDelegate(string contentDestinationFilePath);
    public delegate void StreamWriterDelegate(Stream contentDestinationStream);
    public delegate long? ContentLengthDelegate();

    public sealed class FileContentProvider : ContentProviderBase
    {
        private readonly FileWriterDelegate _fileWriter;
        private readonly ContentLengthDelegate _optimizedContentLengthBeforeMakeStream;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="fileWriter"></param>
        /// <param name="optimizedContentLengthBeforeMakeStream">Не обязателен, но при его задании чтение Length не будет приводить к созданию стрима с предварительной подготовкой</param>
        public FileContentProvider(FileWriterDelegate fileWriter,
            ContentLengthDelegate optimizedContentLengthBeforeMakeStream = null)
        {
            NotNull.CheckArgument(() => fileWriter);
            _fileWriter = fileWriter;
            _optimizedContentLengthBeforeMakeStream = optimizedContentLengthBeforeMakeStream;
        }

        public override long? OptimizedContentLengthBeforeInitialization => _optimizedContentLengthBeforeMakeStream?.Invoke();

        public override void CallOnceContentPreparation(string contentDestinationFilePath)
        {
            _fileWriter(contentDestinationFilePath);
        }
    }

    public abstract class AbstractContentProviderToStreamWritable : ContentProviderBase
    {
        public abstract void WriteContent(Stream contentDestinationStream);

        public sealed override void CallOnceContentPreparation(string contentDestinationFilePath)
        {
            using (var contentDestination = SmartLowMemoryStream.Open(contentDestinationFilePath, FileAccess.Write))
                WriteContent(contentDestination);
        }
    }

    public sealed class ContentProviderToStreamWritable : AbstractContentProviderToStreamWritable
    {
        private readonly StreamWriterDelegate _streamWriter;
        private readonly ContentLengthDelegate _optimizedContentLengthBeforeMakeStream;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="streamWriter"></param>
        /// <param name="optimizedContentLengthBeforeMakeStream">Не обязателен, но при его задании чтение Length не будет приводить к созданию стрима с предварительной подготовкой</param>
        public ContentProviderToStreamWritable(StreamWriterDelegate streamWriter,
            ContentLengthDelegate optimizedContentLengthBeforeMakeStream = null)
        {
            NotNull.CheckArgument(() => streamWriter);
            _streamWriter = streamWriter;
            _optimizedContentLengthBeforeMakeStream = optimizedContentLengthBeforeMakeStream;
        }

        public override long? OptimizedContentLengthBeforeInitialization => _optimizedContentLengthBeforeMakeStream?.Invoke();

        public override void WriteContent(Stream contentDestinationStream)
        {
            _streamWriter(contentDestinationStream);
        }
    }
}
