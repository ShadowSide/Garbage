using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.LinqExt
{
    public static class CollectionAddRangeExtension
    {
        public static void AddRange<T>(this ICollection<T> collection, IEnumerable<T> source)
        {
            foreach (var item in source)
                collection.Add(item);
        }
    }
}
