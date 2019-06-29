using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Text;

namespace Global.BusinessCommon.Helpers.Constraints
{
    public class NotNullOrEmpty
    {
        public static string Check(Expression<Func<string>> func)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value is null)
                throw new NullReferenceException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(string)} is null");
            if (string.IsNullOrEmpty(value))
                throw new Exception($"'{func.Name ?? func.Body.ToString()}' with type {typeof(string)} is empty");
            return value;
        }

        public static string CheckArgument(Expression<Func<string>> func)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value is null)
                throw new ArgumentNullException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(string)} is null");
            if (string.IsNullOrEmpty(value))
                throw new ArgumentException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(string)} is empty");
            return value;
        }

        public static IEnumerable<T> Check<T>(Expression<Func<IEnumerable<T>>> func)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value is null)
                throw new ArgumentNullException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(IEnumerable<T>)} is null");
            if (!value.Any())
                throw new ArgumentException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(IEnumerable<T>)} is empty");
            return value;
        }

        public static IEnumerable<T> CheckArgument<T>(Expression<Func<IEnumerable<T>>> func)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value is null)
                throw new ArgumentNullException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(IEnumerable<T>)} is null");
            if (!value.Any())
                throw new ArgumentException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(IEnumerable<T>)} is empty");
            return value;
        }
    }
}
