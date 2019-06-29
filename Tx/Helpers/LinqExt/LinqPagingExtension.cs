using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.LinqExt
{
    public static class LinqPagingExtension
    {
        public static IEnumerable<IEnumerable<TSource>> Paging<TSource>
            (this IEnumerable<TSource> source, int windowSize)
        {
            return source
                .Select((item, index) => new { item, window = index / windowSize })
                .GroupBy(_ => _.window, _ => _.item);
        }
    }
}
