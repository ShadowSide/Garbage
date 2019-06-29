using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers
{
    public static class Functional
    {
        public static T Id<T>(T arg) => arg;
        public static T Call<T>(Func<T> arg) => arg();
        public static void CallAction(Action arg) => arg();
    }
}
