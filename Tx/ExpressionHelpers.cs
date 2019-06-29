using LinqKit;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Text;
namespace Global.BusinessCommon.Helpers.Expressions
{

    /// <summary>
    /// Класс явного кастинга для улучшения вывода типа выражений
    /// </summary>
    public static class Lambda
    {
        /// <summary>
        /// Скастить явно лямбду в экшен, принимающий тип T
        /// </summary>
        public static Action<T> Action<T>(Action<T> action)
        {
            return action;
        }

        /// <summary>
        /// Скастить явно лямбду в выражение екшен, принимающий тип T
        /// </summary>
        /*public static Expression<Action<T>> ExprAction<T>(Expression<Action<T>> expression)
        {
            return expression;
        }*/

        /// <summary>
        /// Скастить явно лямбду в функцию, возвращающую тип T
        /// </summary>
        public static Func<T> Func<T>(Func<T> func)
        {
            return func;
        }

        /// <summary>
        /// Скастить явно лямбду в выражение, возвращающее тип T
        /// </summary>
        public static Expression<Func<T>> Expr<T>(Expression<Func<T>> expression)
        {
            return expression;
        }

        /// <summary>
        /// Скастить явно лямбду, принимающую параметр S в функцию, возвращающую тип T
        /// </summary>
        public static Func<S, T> Func<S, T>(Func<S, T> func)
        {
            return func;
        }

        /// <summary>
        /// Скастить явно лямбду, принимающую параметр S в выражение, возвращающее тип T
        /// </summary>
        public static Expression<Func<S, T>> Expr<S, T>(Expression<Func<S, T>> expression)
        {
            return expression;
        }
    }

    /// <summary>
    /// Класс явного кастинга для улучшения вывода типа выражений
    /// </summary>
    public static class Lambda<S>
    {
        /// <summary>
        /// Скастить явно лямбду, принимающую параметр S в функцию, возвращающую тип T
        /// </summary>
        public static Func<S, T> Func<T>(Func<S, T> func)
        {
            return func;
        }

        /// <summary>
        /// Скастить явно лямбду, принимающую параметр S в выражение, возвращающее тип T
        /// </summary>
        public static Expression<Func<S, T>> Expr<T>(Expression<Func<S, T>> expression)
        {
            return expression;
        }

        /// <summary>
        /// Скастить явно лямбду в экшен, принимающий тип S
        /// </summary>
        public static Action<S> Action(Action<S> action)
        {
            return action;
        }

        /// <summary>
        /// Скастить явно лямбду в выражение екшен, принимающий тип S
        /// </summary>
        /*public static Expression<Action<S>> ExprAction(Expression<Action<S>> expression)
        {
            return expression;
        }*/
    }

    public static class ExpressionExtensions
    {
        /// <summary>
        /// g.Compose(f) вернет x => g(f(x)). Работает для выражений.
        /// </summary>
        /// <param name="f">Функция с одним параметром</param>
        /// <param name="g">Функция с одним параметром с типом, возвращаемым из функции f</param>
        public static Expression<Func<A, C>> Compose<A, B, C>(this Expression<Func<A, B>> f, Expression<Func<B, C>> g)
        {
            return _ => g.Invoke(f.Invoke(_));
            /*var fparam = Expression.Parameter(typeof(A));
            return Expression.Lambda<Func<A, C>>(
                Expression.Invoke(g, Expression.Invoke(f, fparam)), fparam);*/
        }

        /// <summary>
        /// g.Compose(f) вернет x => g(f(x)). Работает для функций.
        /// </summary>
        /// <param name="f">Функция с одним параметром</param>
        /// <param name="g">Функция с одним параметром с типом, возвращаемым из функции f</param>
        public static Func<A, C> Compose<A, B, C>(this Func<A, B> f, Func<B, C> g)
        {
            return a => g(f(a));
        }
    }


    public static class ExpressionHelpers
    {
        /// <summary>
        /// Получает значение проперти по Expression lambda из указанного объекта source
        /// </summary>
        public static TValue GetPropertyValue<TSourceObject, TValue>(TSourceObject source, Expression<Func<TSourceObject, TValue>> member) where TSourceObject : class
        {
            MemberExpression memberExpr = (MemberExpression)member.Body;
            string memberName = memberExpr.Member.Name;
            Func<TSourceObject, TValue> compiledDelegate = member.Compile();
            TValue value = compiledDelegate(source);
            return value;
        }

        /// <summary>
        /// Получает значение проперти по lambda из указанного объекта source
        /// </summary>
        public static TValue GetPropertyValue<TSourceObject, TValue>(TSourceObject source, Func<TSourceObject, TValue> member) where TSourceObject : class
        {
            return member(source);
        }

        /// <summary>
        /// Устанавливает значение проперти, указанному в Expression lambda для указанного объекта target
        /// </summary>
        public static void SetPropertyValue<TDestinationObject, TValue>(TDestinationObject target, Expression<Func<TDestinationObject, TValue>> member, TValue value)
        {
            var memberSelectorExpression = member.Body as MemberExpression;
            if (memberSelectorExpression != null)
            {
                var property = memberSelectorExpression.Member as PropertyInfo;
                if (property != null)
                {
                    property.SetValue(target, value, null);
                }
            }
        }
    }
}
