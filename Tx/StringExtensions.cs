using System;
using System.Text.RegularExpressions;

namespace Global.BusinessCommon.Helpers.Strings
{
    /// <summary>
    /// Методы расширения строк
    /// </summary>
    public static class StringExtensions
    {
        /// <summary>
        /// Обрезает строку value до длины maxLength, если та длинее maxLength символов.
        /// Если maxLength is null, то возвращает строку не измененной.
        /// Если value is null, то результат функции также null.
        /// </summary>
        public static string TruncateByMaxLength(this string value, int? maxLength)
        {
            if (!maxLength.HasValue)
                return value;

            if (maxLength < 0)
                throw new ArgumentException(nameof(maxLength) + ": " + maxLength + " is below zero");

            if (string.IsNullOrEmpty(value))
                return value;

            return value.Substring(0, Math.Min(value.Length, maxLength.Value));
        }

        /// <summary>
        /// Получить строку для сортировки
        /// Удаляет спецсимволы и приводит к верхнему регистру
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static string GetStringForSorting(this string value)
        {
            Regex regex = new Regex(@"\W");
            return regex.Replace(value, "").ToUpper();
        }
    }
}
