using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Global.BusinessCommon.Helpers.GenericHelpers.ClassStructResolution;

namespace Global.BusinessCommon.Helpers.Monads
{
    public static class MayBeExtension
    {
        /// <summary>
        /// Возвращает пустой перечислитель IEnumerable, если входной параметр содержит null 
        /// или перечислитель с одним элементом, если входной параметр не null.
        /// </summary>
        /// <typeparam name="T">Перегрузка для T? где T : не ссылочный тип </typeparam>
        public static IEnumerable<T> NullableToEnumerable<T>(this T? source) where T: struct
        {
            if (source.HasValue)
                yield return source.Value;
        }

        /// <summary>
        /// Возвращает пустой перечислитель IEnumerable, если входной параметр содержит null 
        /// или перечислитель с одним элементом, если входной параметр не null.
        /// </summary>
        /// <typeparam name="T">Перегрузка для T где T : ссылочный тип </typeparam>
        public static IEnumerable<T> NullableToEnumerable<T>(this T source) where T: class
        {
            if (!(source is null))
                yield return source;
        }

        /// <summary>
        /// Возвращает для входного параметра с пустым перечислителем IEnumerable mull
        /// или значение, содержащееся в первом элементе.
        /// </summary>
        /// <param name="values">Обычно значение полученное из NullableToEnumerable и цепочки преобразований</param>
        /// <returns>Единичное значение или null</returns>
        public static T? FirstOrNull<T>(this IEnumerable<T> values, RequireStruct<T> ignoredArg = null) where T : struct
        {
            return values.Select(_=>(T?)_).DefaultIfEmpty(null).FirstOrDefault();
        }

        /// <summary>
        /// Возвращает для входного параметра с пустым перечислителем IEnumerable mull
        /// или значение, содержащееся в первом элементе.
        /// </summary>
        /// <param name="values">Обычно значение полученное из NullableToEnumerable и цепочки преобразований</param>
        /// <returns>Единичное значение или null</returns>
        public static T FirstOrNull<T>(this IEnumerable<T> values, RequireClass<T> ignoredArg = null) where T : class
        {
            return values.DefaultIfEmpty(null).FirstOrDefault();
        }

        /// <summary>
        /// Возвращает если элементов в перечислении values в кол-ве не 1 - mull
        /// или значение, содержащееся в первом и единственном элементе.
        /// </summary>
        /// <param name="values">Обычно значение полученное из NullableToEnumerable и цепочки преобразований</param>
        /// <returns>Единичное значение или null</returns>
        public static T SingleOrNull<T>(this IEnumerable<T> values, RequireClass<T> ignoredArg = null) where T : class
        {
            T result = null;
            foreach (var value in values)
            {
                if (!(result is null))
                    return null;
                result = value;
            }
            return result;
        }

        /// <summary>
        /// Возвращает если элементов в перечислении values в кол-ве не 1 - mull
        /// или значение, содержащееся в первом и единственном элементе.
        /// </summary>
        /// <param name="values">Обычно значение полученное из NullableToEnumerable и цепочки преобразований</param>
        /// <returns>Единичное значение или null</returns>
        public static T? SingleOrNull<T>(this IEnumerable<T> values, RequireStruct<T> ignoredArg = null) where T : struct
        {
            T? result = null;
            foreach(var value in values)
            {
                if (result.HasValue)
                    return null;
                result = value;
            }
            return result;
        }
    }   
}
