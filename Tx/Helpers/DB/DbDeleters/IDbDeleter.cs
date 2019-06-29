using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.DB.DbDeleters
{
    public interface IDbDeleter<DbContext_>
        where DbContext_ : DbContext
    {
        void Delete(DbContext_ componentDb);
        void FreeSourcesForDelete();
    }

    public interface IDbDeleterByKey<DbContext_, KeyType> : IDbDeleter<DbContext_>
        where DbContext_ : DbContext
    {
        void AddToDelete(KeyType key);
    }
}
