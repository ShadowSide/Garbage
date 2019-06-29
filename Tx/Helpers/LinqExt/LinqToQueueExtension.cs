using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.LinqExt
{
    public static class LinqToQueueExtension
    {
        public static Queue<T> ToQueue<T>(
            this IEnumerable<T> source)
        {
            return new Queue<T>(source);
        }

        public static Queue<T> ToQueue<T>(
            this IQueryable<T> source)
        {
            return new Queue<T>(source);
        }
    }
}
