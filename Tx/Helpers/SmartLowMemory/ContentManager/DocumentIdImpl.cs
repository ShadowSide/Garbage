using Delimon.Win32.IO;
using System;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager.Detail
{
    public class DocumentIdImpl : IDocumentId
    {
        public string DocumentId { get; }
        public IContentManagerArenaId Arena { get; }

        public DocumentIdImpl(IContentManagerArenaId arena, string documentId = null)
        {
            DocumentId = documentId ?? Path.GetTempFileName();
            Arena = arena;
        }

        public bool Equals(IDocumentId other)
        {
            return (!(other is null)) && Tuple.Create(other.DocumentId, other.Arena).Equals(Tuple.Create(DocumentId, Arena));
        }

        public override int GetHashCode()
        {
            return Tuple.Create(DocumentId, Arena).GetHashCode();
        }

        public override string ToString()
        {
            return DocumentId;
        }
    }
}
