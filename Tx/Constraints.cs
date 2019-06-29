using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Text;

namespace Global.BusinessCommon.Helpers.Constraints
{
    public class NotNullOrWhiteSpace
    {
        public static string Check(Expression<Func<string>> func)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value is null)
                throw new NullReferenceException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(string)} is null");
            if (string.IsNullOrWhiteSpace(value))
                throw new Exception($"'{func.Name ?? func.Body.ToString()}' with type {typeof(string)} is whitespace");
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
            if (string.IsNullOrWhiteSpace(value))
                throw new ArgumentException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(string)} is whitespace");
            return value;
        }
    }
    
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
    
    public class MustFalse
    {
        public static void CheckArgument(Expression<Func<bool?>> func, string reasonComment = null)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (!value.HasValue)
                throw new ArgumentNullException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool?)} is null");
            if (value.Value)
                throw new ArgumentException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool)} is true, but must be false{(reasonComment is null ? string.Empty : (" for " + reasonComment))}");
        }

        public static void CheckArgument(Expression<Func<bool>> func, string reasonComment = null)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value)
                throw new ArgumentException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool)} is true, but must be false{(reasonComment is null ? string.Empty : (" for " + reasonComment))}");
        }

        public static void Check(Expression<Func<bool?>> func, string reasonComment = null)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (!value.HasValue)
                throw new NullReferenceException($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool?)} is null");
            if (value.Value)
                throw new Exception($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool)} is true, but must be false{(reasonComment is null ? string.Empty : (" for " + reasonComment))}");
        }

        public static void Check(Expression<Func<bool>> func, string reasonComment = null)
        {
            if (func is null)
                throw new ArgumentNullException(nameof(func));
            var funcCompiled = func.Compile();
            var value = funcCompiled.Invoke();
            if (value)
                throw new Exception($"'{func.Name ?? func.Body.ToString()}' with type {typeof(bool)} is true, but must be false{(reasonComment is null ? string.Empty : (" for " + reasonComment))}");
        }
    }
}
