using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager
{
    public class DocumentInfo
    {
        public DocumentInfo(string name, Action<IDocumentId> onCloseArena, IContentProvider contentProvider)
        {
            Name = name;
            ContentProvider = contentProvider;
            OnCloseArena = onCloseArena;
        }

        public string Name { get; set; }
        public IContentProvider ContentProvider { get; }
        public Action<IDocumentId> OnCloseArena { get; }
    }
}
