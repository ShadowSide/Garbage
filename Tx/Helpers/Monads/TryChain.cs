using Global.BusinessCommon.Helpers.ExceptionHelpers;
using Locator;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.Monads
{
    public static class TryChain
    {
        /// <summary>
        /// Выполянть функции из аргументов, пока не встретиться функция не кидающая исключения или функции не закончатся.
        /// Исключения всех функций записываются в лог с warning левел. Исключение из последней доступной функции летит ниже.
        /// </summary>
        public static void Try(string operation, IEnumerable<Action> funcs)
        {
            var _ = funcs ?? throw new ArgumentNullException(nameof(funcs));
            Exception lastIterationException = null;
            int iteration = 0;
            foreach (var func in funcs)
            {
                try
                {
                    lastIterationException = null;
                    ++iteration;
                    var __ = func ?? throw new NullReferenceException(nameof(func));
                    func();
                    return;
                }
                catch (Exception ex)
                {
                    var opName = $"{nameof(TryChain)} for operation " + operation + " on iteration " + iteration;
                    lastIterationException = ex;
                    CurrentLogger.WriteLog(opName + ": " + CurrentLogger.ExtractExceptionMessage(ex), System.Diagnostics.TraceEventType.Warning);
                }
            }
            if (!(lastIterationException is null))
            {
                var opName = $"{nameof(TryChain)} for operation " + operation + " on iteration " + iteration;
                CurrentLogger.LogEx(opName, lastIterationException);
                throw lastIterationException;
            }
            throw new ArgumentException($"{nameof(funcs)} for {nameof(TryChain)} with empty list");
        }

        /// <summary>
        /// Выполянть функции из аргументов, пока не встретиться функция не кидающая исключения или функции не закончатся.
        /// Исключения всех функций записываются в лог с warning левел. Исключение из последней доступной функции летит ниже.
        /// </summary>
        public static T Try<T>(string operation, IEnumerable<Func<T>> funcs)
        {
            var _ = funcs ?? throw new ArgumentNullException(nameof(funcs));
            Exception lastIterationException = null;
            int iteration = 0;
            foreach (var func in funcs)
            {
                try
                {
                    lastIterationException = null;
                    ++iteration;
                    var __ = func ?? throw new NullReferenceException(nameof(func));
                    return func();
                }
                catch (Exception ex)
                {
                    var opName = $"{nameof(TryChain)} for operation " + operation + " on iteration " + iteration;
                    lastIterationException = ex;
                    CurrentLogger.WriteLog(opName + ": " + CurrentLogger.ExtractExceptionMessage(ex), System.Diagnostics.TraceEventType.Warning);
                }
            }
            if (!(lastIterationException is null))
            {
                var opName = $"{nameof(TryChain)} for operation " + operation + " on iteration " + iteration;
                CurrentLogger.LogEx(opName, lastIterationException);
                throw lastIterationException;
            }
            throw new ArgumentException($"{nameof(funcs)} for {nameof(TryChain)} with empty list");
        }

        /// <summary>
        /// Выполянть функции из аргументов, пока не встретиться функция не кидающая исключения или функции не закончатся.
        /// Исключения всех функций записываются в лог с warning левел. Исключение из последней доступной функции летит ниже.
        /// </summary>
        public static T Try<T>(string operation, params Func<T>[] funcs)
        {
            var _ = funcs ?? throw new ArgumentNullException(nameof(funcs));
            var remainFuncAmount = funcs.Count();
            foreach (var func in funcs)
            {
                try
                {
                    --remainFuncAmount;
                    var __ = func ?? throw new NullReferenceException(nameof(func));
                    return func();
                }
                catch(Exception ex)
                {
                    var opName = $"{nameof(TryChain)} with function number {funcs.Count() - remainFuncAmount} for operation " + operation;
                    if (remainFuncAmount <= 0)
                    {
                        CurrentLogger.LogEx(opName, ex);
                        throw ex;
                    }
                    CurrentLogger.WriteLog(opName + ": " + CurrentLogger.ExtractExceptionMessage(ex), System.Diagnostics.TraceEventType.Warning);
                }
            }
            throw new ArgumentException($"{nameof(funcs)} for {nameof(TryChain)} with empty list");
        }

        /// <summary>
        /// Выполянть функции из аргументов, пока не встретиться функция не кидающая исключения или функции не закончатся.
        /// Не кидает исключений, возвращая при этом defaultValue, если все функции из цепочки зафейлились
        /// </summary>
        public static T TryNoThrow<T>(string operation, T defaultValue, params Func<T>[] funcs)
        {
            return ExceptionHelper.ExceptionCatcher<T>(()=>Try(operation, funcs), defaultValue, operation);
        }

        /// <summary>
        /// Выполянть функции из аргументов, пока не встретиться функция не кидающая исключения, возвращающая подходящщее 
        /// предикату IsSuitValue значение или функции не закончатся.
        /// Если IsSuitValue == null, то предикат подтверждает любые значения не равные default(T) (например null).
        /// Исключения всех функций записываются в лог с warning левел. Исключение из последней доступной функции летит ниже.
        /// Если не нашли ни одну функцию удовлетворяющую предикату, то летит исключение по нехватки функций
        /// </summary>
        public static T TryWhileNotPredicate<T>(string operation, Predicate<T> IsSuitValue, params Func<T>[] funcs)
        {
            var _ = funcs ?? throw new ArgumentNullException(nameof(funcs));
            IsSuitValue = IsSuitValue ?? new Predicate<T> (result => !Object.Equals(result, default(T)));
            var remainFuncAmount = funcs.Count();
            foreach (var func in funcs)
            {
                try
                {
                    --remainFuncAmount;
                    var __ = func ?? throw new NullReferenceException(nameof(func));
                    var result = func();
                    if(IsSuitValue(result))
                        return result;
                }
                catch (Exception ex)
                {
                    var opName = $"{nameof(TryChain)} with function number {funcs.Count() - remainFuncAmount} for operation " + operation;
                    if (remainFuncAmount <= 0)
                    {
                        CurrentLogger.LogEx(opName, ex);
                        throw ex;
                    }
                    CurrentLogger.WriteLog(opName + ": " + CurrentLogger.ExtractExceptionMessage(ex), System.Diagnostics.TraceEventType.Warning);
                }
            }
            throw new Exception($"{nameof(funcs)} for {nameof(TryChain)} list is exceeded");
        }

        /// <summary>
        /// Выполянть функции из аргументов, пока не встретиться функция не кидающая исключения, возвращающая подходящщее 
        /// предикату IsSuitValue значение или функции не закончатся.
        /// Если IsSuitValue == null, то предикат подтверждает любые значения не равные default(T) (например null).
        /// Не кидает исключений, возвращая при этом defaultValue, если все функции из цепочки зафейлились или законились.
        /// </summary>
        public static T TryWhileNotPredicateNoThrow<T>(string operation, T defaultValue, Predicate<T> IsSuitValue, params Func<T>[] funcs)
        {
            return ExceptionHelper.ExceptionCatcher<T>(() => TryWhileNotPredicate(operation, IsSuitValue, funcs), defaultValue, operation);
        }
    }
}
