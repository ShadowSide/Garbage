using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Text;

namespace Global.BusinessCommon.Helpers.Constraints
{
    public class MustTrue
    {
        public static void CheckArgument(Expression<Func<bool?>> func, string reasonComment = null)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (!value.HasValue)
                throw new ArgumentNullException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool?)} is null");
            if (!value.Value)
                throw new ArgumentException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool)} is false, but must be true{(reasonComment is null ? string.Empty : (" for " + reasonComment))}");
        }

        public static void CheckArgument(Expression<Func<bool>> func, string reasonComment = null)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (!value)
                throw new ArgumentException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool)} is false, but must be true{(reasonComment is null ? string.Empty : (" for " + reasonComment))}");
        }

        public static void Check(Expression<Func<bool?>> func, string reasonComment = null)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (!value.HasValue)
                throw new NullReferenceException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool?)} is null");
            if (!value.Value)
                throw new Exception($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool)} is false, but must be true{(reasonComment is null ? string.Empty : (" for " + reasonComment))}");
        }

        public static void Check(Expression<Func<bool>> func, string reasonComment = null)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (!value)
                throw new Exception($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool)} is false, but must be true{(reasonComment is null ? string.Empty : (" for " + reasonComment))}");
        }
    }
}
