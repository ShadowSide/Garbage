using System;
using System.Collections.Generic;
using System.Data.Entity.Core.Objects;
using System.Linq;
using System.Linq.Expressions;
using GlobalInterfaces.DB;
using Global.BusinessCommon.Helpers.Expressions;
using Global.BusinessCommon.Helpers.Expressions.QueryableExtensions;
using GlobalInterfaces.DB.NonCollidableHashForeignerKey;
using Global.BusinessCommon.Helpers.DB.Algo.Impl;
using GlobalInterfaces.Containers.MakingNewOrReusing;

namespace Global.BusinessCommon.Helpers.DB.Algo
{
    /// <summary>
    /// Пример использования:
    /// NonCollidableHashForeignerKeyMaker.Make(
    ///             default(IRefNetDbContainer),
    ///             ctx => ctx.SubTypeKndMigratorLogs,
    ///             ctx => ctx.SubTypeKndMigratorLongInfoes,
    ///             set => set.FirstOrDefault(),
    ///             set => set.FirstOrDefault(),
    ///             referencer => referencer.InfoHash,
    ///             referenced => referenced.Hash,
    ///             referencer => referencer.InfoCollider,
    ///             referenced => referenced.Collider,
    ///             referenced => referenced.Info);
    /// Это просто функция генератор, для класса алгоритма обновления хештаблицы на уровне базы данных
    /// </summary>
    public static class NonCollidableHashForeignerKeyMaker
    {
        /// <summary>
        /// Это просто функция генератор, для класса алгоритма обновления хештаблицы на уровне базы данных.
        /// Таблица referencer связывается с таблицей referenced с использованием пары ключей хеш и индекс колизии хеша.
        /// Смотри пример использования выше.
        /// Таблица referencer хранит ссылки на таблицу referenced. 
        /// Алгоритм принимает строковой тип в качестве Info. Берет от него хеш и кладет в referencer.InfoHash, а в
        /// referencer.InfoCollider кладем индекс, если хеши значений совпали для разных значений.
        /// В referenced.Info (referencedValue) кладем саму строковую информацию и 
        /// если такая строка уже есть, то данные не дублируются и используется ссылка на уже положенные данные.
        /// Если такое referencedValue, ещё не было в referenced, то добавляет новую строку с referencedValue
        /// в referenced.
        /// Поддержки удаления из таблицы referenced, посредством алгоритма пока не реализовано.
        /// referenced.Hash и referenced.Collider - первичный составной ключ в таблице referenced, 
        /// на который ссылается ключ referencer.InfoHash и referencer.InfoCollider
        /// </summary>
        /// <typeparam name="TReferencerSetItem">Тип строки ссылающейся таблицы referencer (record)</typeparam>
        /// <typeparam name="TReferencedSetItem">Тип строки таблицы referenced, на которую ссылаются и в которой хранится referencedValue (record)</typeparam>
        /// <typeparam name="TDbContext">Тип контекста базы данных</typeparam>
        /// <typeparam name="TReferencerSet">Тип ссылающейся таблицы referencer</typeparam>
        /// <typeparam name="TReferencedSet">Тип таблицы referenced, на которую ссылаются и в которой хранится referencedValue</typeparam>
        /// <param name="nullDbContext">Нулевая ссылка на контекста БД, рекомендуем default(IRefNetDbContainer). Только ради вывода типов.</param>
        /// <param name="referencerSet"></param>
        /// <param name="referencedSet"></param>
        /// <param name="referencerFirstItem">Получение первого элемента из ссылающейся таблицы referencer. Только для выовда типов.</param>
        /// <param name="referencedFirstItem">Получение первого элемента из таблицы, на которую ссылаются referenced. Только для выовда типов.</param>
        /// <param name="referencerHash">Колонка ссылающейся таблицы, в которой хранится хеш значения referencedValue в качестве части foreigner ключа. Тип в таблице в БД должен быть Nullable</param>
        /// <param name="referencedHash">Колонка таблицы, на которую ссылаются, в которой хранится хеш значения referencedValue в качестве части primary ключа</param>
        /// <param name="referencerCollider">Колонка ссылающейся таблицы, в которой хранится индекс совпадения хеша значения referencedValue в качестве части foreigner ключа. Тип в таблице в БД должен быть Nullable</param>
        /// <param name="referencedCollider">Колонка таблицы, на которую ссылаются, в которой хранится хранится индекс совпадения хеша значения referencedValue в качестве части primary ключа</param>
        /// <param name="referencedValue">Строковое значение, может быть весьма большим до 8к байт</param>
        /// <param name="hasher">Функция хеширования</param>
        /// <param name="valueConverter">Преобразует или проверяет значение, прежде чем положить его в referencedValue таблицы referenced</param>
        /// <returns>Возвращает объект, работающий с хештаблицей строк в бд</returns>
        public static INonCollidableHashForeignerKey<TDbContext, TReferencerSetItem, TReferencedSetItem> Make<TReferencerSetItem, TReferencedSetItem, TDbContext, TReferencerSet, TReferencedSet>(
                TDbContext nullDbContext,
                Func<TDbContext, TReferencerSet> referencerSet,
                Func<TDbContext, TReferencedSet> referencedSet,
                Func<TReferencerSet, TReferencerSetItem> referencerFirstItem,
                Func<TReferencedSet, TReferencedSetItem> referencedFirstItem,
                Expression<Func<TReferencerSetItem, string>> referencerHash,
                Expression<Func<TReferencedSetItem, string>> referencedHash,
                Expression<Func<TReferencerSetItem, int?>> referencerCollider,
                Expression<Func<TReferencedSetItem, int>> referencedCollider,
                Expression<Func<TReferencedSetItem, string>> referencedValue,
                Func<string, string> hasher = null, /*Hasher.Sha1*/
                IDbStringValueConverter valueConverter = null /*new DbMaxStringValueRestricter()*/
            )
            where TDbContext : class
            where TReferencerSet : class, IObjectSet<TReferencerSetItem>
            where TReferencedSet : class, IObjectSet<TReferencedSetItem>
            where TReferencerSetItem : class
            where TReferencedSetItem : class, new()
        {
            return new NonCollidableHashForeignerKey<TDbContext, TReferencerSet, TReferencedSet, TReferencerSetItem, TReferencedSetItem>(
                referencerSet, referencedSet, referencerHash, referencedHash, 
                referencerCollider, referencedCollider, referencedValue, hasher, valueConverter);
        }
    }

    /// <summary>
    /// Алгоритм обновления хештаблицы на уровне базы данных.
    /// Используй NonCollidableHashForeignerKeyMaker. Смотри комментарий к нему
    /// </summary>
    public class NonCollidableHashForeignerKey<TDbContext, TReferencerSet, TReferencedSet, TReferencerSetItem, TReferencedSetItem> : INonCollidableHashForeignerKey<TDbContext, TReferencerSetItem, TReferencedSetItem>
        where TDbContext: class 
        where TReferencerSet : class, IObjectSet<TReferencerSetItem>
        where TReferencedSet : class, IObjectSet<TReferencedSetItem>
        where TReferencerSetItem : class
        where TReferencedSetItem : class, new()
    {
        private readonly Func<TDbContext, TReferencerSet> referencerSet;
        private readonly Func<TDbContext, TReferencedSet> referencedSet;
        private readonly Expression<Func<TReferencerSetItem, string>> referencerHash;
        private readonly Expression<Func<TReferencedSetItem, string>> referencedHash;
        private readonly Expression<Func<TReferencerSetItem, int?>> referencerCollider;
        private readonly Expression<Func<TReferencedSetItem, int>> referencedCollider;
        private readonly Expression<Func<TReferencedSetItem, string>> referencedValue;
        private readonly Func<string, string> hasher;
        private readonly IDbStringValueConverter valueConverter;


        /// <summary>
        /// Не предназначен для прямого использования.  
        /// Используй NonCollidableHashForeignerKeyMaker.
        /// </summary>
        public NonCollidableHashForeignerKey(
            Func<TDbContext, TReferencerSet> referencerSet,
            Func<TDbContext, TReferencedSet> referencedSet,
            Expression<Func<TReferencerSetItem, string>> referencerHash,
            Expression<Func<TReferencedSetItem, string>> referencedHash,
            Expression<Func<TReferencerSetItem, int?>> referencerCollider,
            Expression<Func<TReferencedSetItem, int>> referencedCollider,
            Expression<Func<TReferencedSetItem, string>> referencedValue,
            Func<string, string> hasher = null, /*Hasher.Sha1*/
            IDbStringValueConverter valueConverter = null)
        {
            this.referencerSet = referencerSet;
            this.referencedSet = referencedSet;
            this.referencerHash = referencerHash;
            this.referencedHash = referencedHash;
            this.referencerCollider = referencerCollider;
            this.referencedCollider = referencedCollider;
            this.referencedValue = referencedValue;
            this.hasher = hasher ?? (data => Hasher.Sha1(data));
            this.valueConverter = valueConverter ?? new DbMaxStringValueRestricter();
        }

        public TReferencedSetItem InsertOrUpdateValue(TDbContext dbContext, TReferencerSetItem referencerRow, string value)
        {
            if (string.IsNullOrWhiteSpace(value))
            {
                ExpressionHelpers.SetPropertyValue(referencerRow, referencerHash, null);
                ExpressionHelpers.SetPropertyValue(referencerRow, referencerCollider, null);
                return null;
            }
            value = valueConverter.Convert(value);
            var newValueHash = hasher(value);
            TReferencedSetItem referencedRow;
            var referencedSet_ = referencedSet(dbContext);
            var haveReferencedWithNewValueHash =
                referencedSet_
                .WhereEqualQuery(referencedHash, newValueHash)
                .Any();
            if (haveReferencedWithNewValueHash)
            {
                referencedRow = 
                    referencedSet_
                    .WhereEqualQuery(referencedHash, newValueHash)
                    .WhereEqualQuery(referencedValue, value)
                    .FirstOrDefault();
                if (referencedRow == null)
                {
                    var newValueCollider = GetFirstFreeColliderOfHash(dbContext, referencedSet_, newValueHash);
                    referencedRow = AddNewValue(referencerRow, newValueHash, value, referencedSet_, newValueCollider);
                }
                else
                {
                    var newValueCollider = ExpressionHelpers.GetPropertyValue(referencedRow, referencedCollider);
                    ExpressionHelpers.SetPropertyValue(referencerRow, referencerHash, newValueHash);
                    ExpressionHelpers.SetPropertyValue(referencerRow, referencerCollider, newValueCollider);
                }
            }
            else
            {
                var newValueCollider = 1;
                referencedRow = AddNewValue(referencerRow, newValueHash, value, referencedSet_, newValueCollider);
            }
            return referencedRow;
        }

        private TReferencedSetItem AddNewValue(TReferencerSetItem referencerRow, string newValueHash, string value, TReferencedSet referencedSet_, int newValueCollider)
        {
            var referencedRow = new TReferencedSetItem();
            ExpressionHelpers.SetPropertyValue(referencerRow, referencerHash, newValueHash);
            ExpressionHelpers.SetPropertyValue(referencedRow, referencedHash, newValueHash);
            ExpressionHelpers.SetPropertyValue(referencerRow, referencerCollider, newValueCollider);
            ExpressionHelpers.SetPropertyValue(referencedRow, referencedCollider, newValueCollider);
            ExpressionHelpers.SetPropertyValue(referencedRow, referencedValue, value);
            referencedSet_.AddObject(referencedRow);
            return referencedRow;
        }

        private int GetFirstFreeColliderOfHash(TDbContext dbContext, TReferencedSet referencedSet_, string hash)
        {
            return referencedSet(dbContext)
                .WhereEqualQuery(referencedHash, hash)
                .Select(referencedCollider)
                .DefaultIfEmpty(0)
                .Max()+1;
        }

        public string GetValue(TDbContext dbContext, TReferencerSetItem referencer)
        {
            var hashForFinding = ExpressionHelpers.GetPropertyValue(referencer, referencerHash);
            var colliderForFinding = ExpressionHelpers.GetPropertyValue(referencer, referencerCollider);
            if (hashForFinding is null || !colliderForFinding.HasValue)
                return null;
            var result = GetReferenced(dbContext, hashForFinding, colliderForFinding.Value);
            return result is null ? null : ExpressionHelpers.GetPropertyValue(result, referencedValue);
        }

        private TReferencedSetItem GetReferenced(TDbContext dbContext, string hashForFinding, int colliderForFinding)
        {
            var resultSet = referencedSet(dbContext)
                .WhereEqualQuery(referencedHash, hashForFinding)
                .WhereEqualQuery(referencedCollider, colliderForFinding);
            var result = resultSet.FirstOrDefault();
            return result;
        }
    }
}
