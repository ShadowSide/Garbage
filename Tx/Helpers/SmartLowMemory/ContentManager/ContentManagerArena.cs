using System;

namespace Global.BusinessCommon.Helpers.SmartLowMemory.ContentManager.Detail
{
    public class ContentManagerArena : IContentManagerArena
    {
        private Action<IContentManagerArenaId> _endArena;

        public ContentManagerArena(Action<IContentManagerArenaId> endArena)
        {
            _endArena = endArena ?? throw new ArgumentNullException(nameof(endArena));
        }

        public string ArenaId { get; } = Guid.NewGuid().ToString();

        public void Dispose()
        {
            var endArena = _endArena;
            _endArena = null;
            endArena?.Invoke(this);
        }

        public bool Equals(IContentManagerArenaId other)
        {
            return (!(other is null)) && other.ArenaId.Equals(ArenaId);
        }

        public override int GetHashCode()
        {
            return ArenaId.GetHashCode();
        }

        public override string ToString()
        {
            return ArenaId;
        }
    }
}
