using System;
using System.Collections.Generic;
using System.Linq;
using GlobalInterfaces;
using Locator;

namespace Global.BusinessCommon.Helpers.ExceptionHelpers
{
    public class ExceptionHelper
    {
        public static bool IsOrInner<T>(Exception exception) where T : Exception
        {
            if (exception is null)
                return false;
            return exception is T || IsOrInner<T>(exception.InnerException);
        }

        public static string ExceptionMessage(Exception ex, string where = null, IGlobalLogger logger = null)
        {
            try
            {
                var exExtract = (logger is null ? new Func<Exception, string>(CurrentLogger.ExtractExceptionMessage) : logger.ExtractExceptionMessage);
                var message = exExtract(ex);
                return where is null ? message : message + " in " + where;
            }
            catch
            {
                return "ExceptionMessage failed " + ex;
            }
        }

        public static void ExceptionCatcher(Action ignoredExceptionAction, string where = null,
            IGlobalLogger logger = null)
        {
            try
            {
                ignoredExceptionAction();
            }
            catch (Exception ex)
            {
                try
                {
                    var logEx = (logger is null ? new Action<string, Exception>(CurrentLogger.LogEx) : logger.LogEx);
                    logEx(where, ex);
                }
                catch
                {
                    // ignored
                }
            }
        }

        public static TResult ExceptionCatcher<TResult>(Func<TResult> ignoredExceptionFunction,
            TResult defaultValue = default(TResult), string where = null, IGlobalLogger logger = null)
        {
            try
            {
                return ignoredExceptionFunction();
            }
            catch (Exception ex)
            {
                try
                {
                    var logEx = (logger is null ? new Action<string, Exception>(CurrentLogger.LogEx) : logger.LogEx);
                    logEx(where, ex);
                }
                catch
                {
                    // ignored
                }
            }
            return defaultValue;
        }

        public static List<TException> ContainException<TException>(Exception exception) where TException : Exception
        {
            return ContainException(exception, typeof(TException)).Select(_ => _ as TException).ToList();
        }

        public static IEnumerable<Exception> ContainException(Exception exception, Type type)
        {
            if (exception == null)
                return Enumerable.Empty<Exception>();
            if (exception.GetType() == type)
                return new[] { exception };
            var innerIsType = ContainException(exception.InnerException, type);
            var agregatedIsType = exception is AggregateException ? (exception as AggregateException).InnerExceptions.Where(_ => ContainException(_, type) != null) : new List<Exception>();
            return innerIsType.Concat(agregatedIsType);
        }

        public static bool IsThrowAnyException(Action checkedAction, string operation = null)
        {
            try
            {
                checkedAction();
                return false;
            }
            catch (Exception ex)
            {
                CurrentLogger.Trace($"Operation {operation} with error {ex.Message}");
            }
            return true;
        }
    }

    /// <summary>
    /// Класс в конструкторе вызывает функтор initialize.
    /// А в Dispose вызывает функтор dispose.
    /// Если initialize is null, то он не вызывается. 
    /// Если dispose is null - тоже.
    /// </summary>
    public class RAII : IDisposable
    {
        private Action _dispose;
        private IDisposable _disposable;

        public RAII(Action dispose, Action initialize = null)
        {
            _dispose = dispose;
            initialize?.Invoke();
        }

        public RAII(IDisposable disposable)
        {
            _disposable = disposable;
        }

        public void Dispose()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            if (disposing)
                GC.SuppressFinalize(this);
            var dispose = _dispose;
            if (!(dispose is null))
            {
                _dispose = null;
                dispose();
            }
            var disposable = _disposable;
            if (!(disposable is null))
            {
                _disposable = null;
                disposable.Dispose();
            }
        }
    }
}
