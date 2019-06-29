using Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager.Detail;
using Global.BusinessCommon.Helpers.SmartLowMemory.IO;
using Global.BusinessCommon.Helpers.ExceptionHelpers;
using System;
using System.Collections.Concurrent;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using GlobalInterfaces;
using ArenaInfo = System.Collections.Concurrent.ConcurrentDictionary<
    Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager.IDocumentId, 
    Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager.DocumentInfo>;
using File = Delimon.Win32.IO.File;
using FileAccess = Delimon.Win32.IO.FileAccess;
using FileMode = Delimon.Win32.IO.FileMode;
using FileShare = Delimon.Win32.IO.FileShare;
using Path = Delimon.Win32.IO.Path;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager
{
    public class ContentManager : IContentManager
    {
        public IContentManagerArena BeginArena()
        {
            var arena = new ContentManagerArena(this.OnCloseArena);
            if (!Arenas.TryAdd(arena, new ArenaInfo()))
                throw new Exception($"Content manager arena id {arena} duplicated. Internal error.");
            return arena;
        }

        public IDocumentId MakeLazyDocument(IContentManagerArenaId arena, string name, IContentProvider contentProvider)
        {
            if (contentProvider is null)
                throw new ArgumentNullException(nameof(contentProvider));
            var documentId = new DocumentIdImpl(arena);
            AddDocumentInfo(documentId, new DocumentInfo(name, OnCloseArenaDeleteDocument, contentProvider));
            return documentId;
        }

        public long DocumentSize(IDocumentId documentId)
        {
            using (var document = SharedReadDocument(documentId))
                return document.Stream.Length;
        }

        public string DocumentName(IDocumentId documentId)
        {
            return GetDocumentInfo(documentId).Name;
        }

        public bool DocumentIsEmpty(IDocumentId documentId)
        {
            if (documentId == null)
                return true;
            return DocumentSize(documentId) == 0;
        }

        public void RenameDocument(IDocumentId documentId, string name)
        {
            var documentInfo = GetDocumentInfo(documentId);
            documentInfo.Name = name;
        }

        public INamedStream ExclusiveReadWriteDocument(IDocumentId documentId)
        {
            return OpenLazyDocument(documentId, FileAccess.ReadWrite, FileShare.None);
        }

        private INamedStream OpenLazyDocument(IDocumentId documentId, FileAccess access, FileShare share)
        {
            var documentInfo = GetDocumentInfo(documentId);
            var result = new Document
            {
                Name = documentInfo.Name,
                Stream = new LazyConstructableStream(
                    streamFactory: () => OpenDocumentStream(documentId, access, share),
                    optimizedSizeGetterWithoutStreamCreating: () => documentInfo.ContentProvider?.OptimizedContentLengthProvider)
            };
            return result;
        }

        public INamedStream SharedReadDocument(IDocumentId documentId)
        {
            return OpenLazyDocument(documentId, FileAccess.Read, FileShare.Read);
        }

        public INamedStream MakeWritableEmptyDocument(IContentManagerArenaId arena, string name, out IDocumentId documentId)
        {
            using (var empty = System.IO.Stream.Null)
                documentId = MakeDocument(arena, new Document() {Name = name, Stream = empty });
            return ExclusiveReadWriteDocument(documentId);
        }

        public IDocumentId MakeDocument(IContentManagerArenaId arena, string filePath,
            FileUsingMode mode = FileUsingMode.AutoDetect, bool deleteSourceFile = false, string fileName = null)
        {
            if (arena is null)
                throw new ArgumentNullException(nameof(arena));
            if (string.IsNullOrWhiteSpace(filePath))
                throw new ArgumentException(nameof(filePath));
            if (!File.Exists(filePath))
                throw new ArgumentException($"{nameof(filePath)} = '{filePath}' does not exists.");
            fileName = fileName ?? Path.GetFileName(filePath);
            if (string.IsNullOrWhiteSpace(fileName))
                throw new ArgumentException(nameof(fileName) + " is empty.");
            if(mode == FileUsingMode.AutoDetect)
                mode = PathIsNetworkPath(filePath) ? FileUsingMode.CopySourceFile : FileUsingMode.UseSourceFile;
            IDocumentId documentId = null;
            IContentProvider contentProvider = null;
            Action<IDocumentId> onCloseArena = OnCloseArenaDeleteDocument;
            switch (mode)
            {
                case FileUsingMode.UseSourceFile:
                {
                    if (!deleteSourceFile)
                        onCloseArena = null;
                    documentId = new DocumentIdImpl(arena, filePath);
                } break;
                case FileUsingMode.CopySourceFile:
                {
                    var arenaFilePath = Path.GetTempFileName();
                    if (deleteSourceFile)
                    {
                        File.Move(filePath, arenaFilePath, true);
                        documentId = new DocumentIdImpl(arena, arenaFilePath);
                    }
                    else
                    {
                        contentProvider = new FileContentProvider(contentDestinationPath => File.Copy(filePath, contentDestinationPath, true), () => File.Length(filePath));
                        documentId = new DocumentIdImpl(arena, arenaFilePath);
                    }
                } break;
                default:
                    throw new NotImplementedException($"{nameof(mode)}={mode}");
            }
            AddDocumentInfo(documentId, new DocumentInfo(fileName, onCloseArena, contentProvider));
            return documentId;
        }

        public IDocumentId MakeDocument(IContentManagerArenaId arena, INamedStream document)
        {
            if (document is null)
                throw new ArgumentNullException(nameof(document));
            if (document.Stream is null)
                throw new ArgumentNullException(nameof(document) + "." + nameof(document.Stream));
            var documentId = new DocumentIdImpl(arena);
            using (var content = File.Open(documentId.DocumentId, FileMode.Open, FileAccess.Write, FileShare.None))
            {
                //document.Stream.Flush();
                document.Stream.CopyTo(content);
                //content.Flush();
            };
            AddDocumentInfo(documentId, new DocumentInfo(document.Name, OnCloseArenaDeleteDocument, null));
            return documentId;
        }

        private Stream OpenDocumentStream(IDocumentId documentId, FileAccess access, FileShare share)
        {
            GetDocumentInfo(documentId).ContentProvider?.ContentPreparation(documentId.DocumentId);
            var file = File.Open(documentId.DocumentId, FileMode.Open, access, share);
            try
            {
                return SmartLowMemoryStream.Open(file);
            }
            catch
            {
                file?.Dispose();
                throw;
            }
        }

        private ArenaInfo GetArenaInfo(IContentManagerArenaId arena, IDocumentId documentId = null/*for logging only*/)
        {
            if (Arenas.TryGetValue(arena, out var currentArenaInfo))
                return currentArenaInfo;
            throw new ArgumentException($"Working with disposed arena {arena} for document id '{documentId?.ToString() ?? ""}'");
        }

        private DocumentInfo GetDocumentInfo(ArenaInfo arenaInfo, IDocumentId documentId)
        {
            if (arenaInfo.TryGetValue(documentId, out var currentDocumentInfo))
                return currentDocumentInfo;
            throw new ArgumentException($"Not found document id '{documentId}' on arena '{documentId.Arena}'");
        }

        private DocumentInfo GetDocumentInfo(IDocumentId documentId)
        {
            var arenaInfo = GetArenaInfo(documentId.Arena, documentId);
            return GetDocumentInfo(arenaInfo, documentId);
        }

        private void AddDocumentInfo(ArenaInfo arenaInfo, IDocumentId documentId, DocumentInfo documentInfo)
        {
            if (!arenaInfo.TryAdd(documentId, documentInfo))
                throw new Exception($"Content manager document id {documentId} duplicated in arena {documentId.Arena}.");
        }

        private void AddDocumentInfo(IDocumentId documentId, DocumentInfo documentInfo)
        {
            var arenaInfo = GetArenaInfo(documentId.Arena, documentId);
            AddDocumentInfo(arenaInfo, documentId, documentInfo);
        }

        private void OnCloseArena(IContentManagerArenaId arena)
        {
            if (!Arenas.TryRemove(arena, out var arenaInfo))
                throw new ArgumentException("Already disposed content manager arena " + arena);
            foreach (var documentId in arenaInfo.ToList())
                documentId.Value.OnCloseArena?.Invoke(documentId.Key);
        }

        private void OnCloseArenaDeleteDocument(IDocumentId document)
        {
            var documentPath = document.DocumentId;
            ExceptionHelper.ExceptionCatcher(() => File.Delete(documentPath),
                where: $"Error while disposing arena {document.Arena}. Probably documentId '{document.DocumentId}' still opened because some stream not disposed.");
        }

        [DllImport("shlwapi.dll")]
        private static extern bool PathIsNetworkPath(string pszPath);

        private readonly ConcurrentDictionary<IContentManagerArenaId, ArenaInfo> Arenas = new ConcurrentDictionary<IContentManagerArenaId, ArenaInfo>();
    }
}
