using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Global.BusinessCommon.Helpers.ExceptionHelpers;

namespace Global.BusinessCommon.Helpers.Multithreading
{
    /// <summary>
    /// Для каждого потока при обращении к методу Get возвращает свой экземпляр T.
    /// Экземпляры могут быть IDisposable
    /// </summary>
    public class ThreadLocalDisposableObjectsCache<T> : IDisposable where T : class, IDisposable
    {
        private readonly Func<T> _objectFactory;
        private Dictionary<int, T> _cache;
        private readonly object _sync = new object();

        /// <summary>
        /// Принимает фабрику тредлокальных объектов objectFactory
        /// </summary>
        public ThreadLocalDisposableObjectsCache(Func<T> objectFactory)
        {
            if (objectFactory is null)
                throw new ArgumentNullException(nameof(objectFactory));
            _objectFactory = objectFactory;
            _cache = new Dictionary<int, T>();
        }

        /// <summary>
        /// Получает единственные, предназначенный только для данного потока экземпляр объекта 
        /// или исключение из конструктора объекта
        /// </summary>
        public T Get()
        {
            lock (_sync)
            {
                if (_cache is null)
                    throw new ObjectDisposedException(nameof(ThreadLocalDisposableObjectsCache<T>));
                var threadId = Thread.CurrentThread.ManagedThreadId;
                if (_cache.TryGetValue(threadId, out T objectForCurrentThread))
                    return objectForCurrentThread;
                objectForCurrentThread = _objectFactory();
                if (objectForCurrentThread is null)
                    throw new InvalidOperationException(nameof(_objectFactory) + " returned null");
                try
                {
                    _cache.Add(threadId, objectForCurrentThread);
                }
                catch
                {
                    ExceptionHelper.ExceptionCatcher(objectForCurrentThread.Dispose);
                    throw;
                }
                return objectForCurrentThread;
            }
        }

        /// <summary>
        /// Для всех объектов, сохраненные в кеше вызывается Dispose
        /// </summary>
        public void Dispose()
        {
            lock (_sync)
            {
                var cache = _cache;
                _cache = null;
                if (cache is null)
                    return;
                foreach (var cachedObject in cache)
                    ExceptionHelper.ExceptionCatcher(() => cachedObject.Value.Dispose());
            }
        }
    }
}
