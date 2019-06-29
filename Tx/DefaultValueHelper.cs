using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.GenericHelpers
{
    public static class DefaultValueHelper
    {
        public static T ConstuctDefaultValue<T>(this T value)
        {
            return default(T);
        }
    }
}
