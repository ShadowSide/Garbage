using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Text;

namespace Global.BusinessCommon.Helpers.Constraints
{
    public class NotNull
    {
        public static T Check<T>(Expression<Func<T?>> func) where T : struct
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (!value.HasValue)
                throw new NullReferenceException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(T?)} is null");
            return value.Value;
        }

        public static T Check<T>(Expression<Func<T>> func) where T : class
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value is null)
                throw new NullReferenceException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(T)} is null");
            return value;
        }

        public static T CheckArgument<T>(Expression<Func<T?>> func) where T : struct
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (!value.HasValue)
                throw new ArgumentNullException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(T?)} is null");
            return value.Value;
        }

        public static T CheckArgument<T>(Expression<Func<T>> func) where T : class
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value is null)
                throw new ArgumentNullException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(T)} is null");
            return value;
        }
    }
}
