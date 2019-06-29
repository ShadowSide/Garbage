using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GlobalInterfaces;
using GlobalInterfaces.DB.NonCollidableHashForeignerKey;
using Locator;

namespace Global.BusinessCommon.Helpers.DB.Algo.Impl
{
    /// <summary>
    /// Бд не может хранить строки больше, чем MaxEntityStringLength. 
    /// Поэтому этот класс обрезает строку значения на основе данных в бд.
    /// </summary>
    public class DbMaxStringValueRestricter : IDbStringValueConverter
    {
        const int MaxEntityStringLength = 7900;
        /*private Dictionary<DbType, int> _maxStringLength = new Dictionary<DbType, int>
        {
            { DbType.MSSQL, 7900 }, //8000
            { DbType.Firebird, 7900 } //32765
        };*/

        public DbMaxStringValueRestricter()
        {
            /*_dbType = Services.Locator.Resolve<ISettings>().Type */
        }

        //private readonly DbType _dbType;

        public string Convert(string value)
        {
            if (string.IsNullOrEmpty(value))
                return value;
            var maxLength = MaxEntityStringLength/*_maxStringLength[_dbType]*/;
            return value.Substring(0, Math.Min(value.Length, maxLength));
        }
    }
}
