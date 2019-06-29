using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.GenericHelpers.ClassStructResolution
{
    public class RequireStruct<T> where T : struct { }
    public class RequireClass<T> where T : class { }
}
