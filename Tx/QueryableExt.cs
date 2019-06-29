using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Text;

namespace Global.BusinessCommon.Helpers.Expressions.QueryableExtensions
{
    public static class QueryableExtension
    {
        /// <summary>
        /// Для linq запороса query возвращает запрос:
        /// query.Where(item => propertyForEqualityChecking(item) == whereEqualTo);
        /// </summary>
        public static IQueryable<TSetItem> WhereEqualQuery<TSetItem, Value>(this IQueryable<TSetItem> query, Expression<Func<TSetItem, Value>> propertyForEqualityChecking, Value whereEqualTo) where TSetItem : class
        {
            var whereEqualToExpr = Expression.Constant(whereEqualTo);
            ParameterExpression setItemParameter = Expression.Parameter(typeof(TSetItem));
            var propertyForEqualityCheckingMember = (PropertyInfo)((MemberExpression)propertyForEqualityChecking.Body).Member;
            var getPropertyForEqualityCheckingValueExpr = Expression.Property(setItemParameter, propertyForEqualityCheckingMember);
            var whereExpr = Expression.Lambda<Func<TSetItem, bool>>(Expression.Equal(getPropertyForEqualityCheckingValueExpr, whereEqualToExpr), setItemParameter);
            var resultSet = query
                .Where(whereExpr);
            return resultSet;
        }
    }
}
