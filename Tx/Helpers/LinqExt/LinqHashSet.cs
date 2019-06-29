using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Linq.Expressions;

namespace Global.BusinessCommon.Helpers.LinqExt
{
    public static class LinqHashSetExtension
    {
        public static HashSet<TKey> ToHashSet<TSource, TKey>(
            this IEnumerable<TSource> source,
            Func<TSource, TKey> keySelector,
            IEqualityComparer<TKey> comparer = null)
        {
            return new HashSet<TKey>(source.Select(keySelector), comparer);
        }

        public static HashSet<TKey> ToHashSet<TSource, TKey>(
            this IQueryable<TSource> source,
            Expression<Func<TSource, TKey>> keySelector, 
            IEqualityComparer<TKey> comparer = null)
        {
            return new HashSet<TKey>(source.Select(keySelector), comparer);
        }

        public static HashSet<TKey> ToHashSet<TKey>(
            this IEnumerable<TKey> source, 
            IEqualityComparer<TKey> comparer = null)
        {
            return new HashSet<TKey>(source, comparer);
        }
    }
}
