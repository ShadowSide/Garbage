using System;
using System.IO;
using Global.BusinessCommon.Helpers.Constraints;
using Global.BusinessCommon.Helpers.ExceptionHelpers;
using Global.BusinessCommon.Helpers.SmartLowMemory.IO;
using GlobalInterfaces;
using FileAccess = Delimon.Win32.IO.FileAccess;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager
{
    public enum FileUsingMode
    {
        AutoDetect,//TODO: добавить проверку места на диске приемнике?
        UseSourceFile,
        CopySourceFile//TODO: добавить проверку места на диске приемнике?
    }

    public interface IContentManager
    {
        IContentManagerArena BeginArena();

        IDocumentId MakeLazyDocument(IContentManagerArenaId arena, string name, IContentProvider contentProvider);
        IDocumentId MakeDocument(IContentManagerArenaId arena, string filePath, FileUsingMode mode = FileUsingMode.AutoDetect, bool deleteSourceFile = false, string fileName = null);
        IDocumentId MakeDocument(IContentManagerArenaId arena, INamedStream document);
        INamedStream MakeWritableEmptyDocument(IContentManagerArenaId arena, string name, out IDocumentId documentId);
        bool DocumentIsEmpty(IDocumentId documentId);
        long DocumentSize(IDocumentId documentId);
        string DocumentName(IDocumentId documentId);
        void RenameDocument(IDocumentId documentId, string name);
        INamedStream SharedReadDocument(IDocumentId documentId);
        INamedStream ExclusiveReadWriteDocument(IDocumentId documentId);
    }

    public interface IContentManagerArenaId : IEquatable<IContentManagerArenaId>
    {
        string ArenaId { get; }
    }

    public interface IContentManagerArena : IContentManagerArenaId, IDisposable
    {}

    public interface IDocumentId: IEquatable<IDocumentId>
    {
        string DocumentId { get; }
        IContentManagerArenaId Arena { get; }
    }

    public interface IContentProvider
    {
        long? OptimizedContentLengthProvider { get; }
        void ContentPreparation(string contentDestinationFilePath);
        bool Initialized { get; }
    }
}
