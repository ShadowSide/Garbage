using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Global.BusinessCommon.Helpers.Constraints;

namespace Global.BusinessCommon.Helpers.SmartLowMemory
{
    public class WeakReference<T> : WeakReference where T : class
    {
        public WeakReference(T value): base(value)
        { }

        public T Value
        {
            get
            {
                var target = Target;
                var result = target as T;
                return result;
            }
        }

        public bool TryGetValue(out T value)
        {
            var result = Value;
            value = result;
            return !(result is null);
        }
    }
}
