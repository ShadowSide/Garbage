using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.Multithreading
{
    public static class SingledThreadAsParallelExtension
    {
        public static ParallelQuery<TSource> ForceSingleThreadIf<TSource>(this ParallelQuery<TSource> query, bool isForcedSingleThread)
        {
            if (!isForcedSingleThread)
                return query;
            return query.WithDegreeOfParallelism(1);
        }
    }
}
