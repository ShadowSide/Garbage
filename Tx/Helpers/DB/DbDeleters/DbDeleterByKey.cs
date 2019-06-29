using Global.BusinessCommon.Helpers.Expressions;
using Global.BusinessCommon.Helpers.Expressions.QueryableExtensions;
using Global.BusinessCommon.Helpers.LinqExt;
using LinqKit;
using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Linq.Expressions;
using System.Text;

namespace Global.BusinessCommon.Helpers.DB.DbDeleters
{
    public class DbDeleterByKey<DbContext_, TableSource, SourceRow, KeyType> : IDbDeleterByKey<DbContext_, KeyType>
        where DbContext_ : DbContext
        where TableSource : DbSet<SourceRow>
        where SourceRow : class
    {
        public DbDeleterByKey(Func<DbContext_, TableSource> tableSource, Expression<Func<SourceRow, KeyType>> getKey, List<KeyType> keysForDelete = null)
        {
            _tableSource = tableSource;
            _getKey = getKey;
            _keysForDelete = keysForDelete ?? new List<KeyType>();
        }

        public readonly Func<DbContext_, TableSource> _tableSource;
        public readonly Expression<Func<SourceRow, KeyType>> _getKey;
        public List<KeyType> _keysForDelete;

        public void Delete(DbContext_ componentDb)
        {
            var table = _tableSource(componentDb);
            foreach (var key in _keysForDelete)
                table.RemoveRange(table.WhereEqualQuery(_getKey, key));
        }

        public void AddToDelete(KeyType key)
        {
            _keysForDelete.Add(key);
        }

        public void FreeSourcesForDelete()
        {
            _keysForDelete.Clear();
        }
    }

    public static class DbDeleterByKey
    {
        public static IDbDeleterByKey<DbContext_, KeyType> Make<DbContext_, TableSource, SourceRow, KeyType>(DbContext_ dbContextNullDummy, Func<DbContext_, TableSource> tableSource, Func<TableSource, SourceRow> rowsSourceNullDummy, Expression<Func<SourceRow, KeyType>> getKey, List<KeyType> keysForDelete = null)
            where TableSource : DbSet<SourceRow>
            where SourceRow : class
            where DbContext_ : DbContext
        {
            return new DbDeleterByKey<DbContext_, TableSource, SourceRow, KeyType>(tableSource, getKey, keysForDelete);
        }
    }
}
