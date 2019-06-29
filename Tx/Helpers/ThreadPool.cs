using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading;
using Global.BusinessCommon.Helpers.Containers;
using Global.BusinessCommon.Helpers.LinqExt;

namespace Global.BusinessCommon.Helpers.ThreeTier
{
    public static class ThreadPool
    {
        /// <summary>
        /// Добавляет задачу в тред пул трехзвенки
        /// </summary>
        /// <param name="priority">Приоритет исполнения задач из очереди задач. Пока не поддерживается, тк не используется BlockingPriorityQueue в реализации ThreadPoolImpl</param>
        public static void AddTask(Action task, int priority = 0)
        {
            _impl.Value.AddTask(task, priority);
        }

        /// <summary>
        /// Сообщает true, если текущий поток выполняется в тредпуле трехзвенки.
        /// </summary>
        /// <returns></returns>
        public static bool ThisThreadFromThisThreadPool
        {
            get
            {
                var impl = _impl;
                if (impl is null)
                    return false;
                if (!impl.IsValueCreated)
                    return false;
                return impl.Value.ThisThreadFromThisThreadPool();
            }
        }

        public static bool IsConstracted => _impl.IsValueCreated;

        public static void Close()
        {
            var impl = _impl;
            _impl = null;
            if (impl.IsValueCreated)
                impl.Value.Dispose();
        }

        private static Lazy<ThreadPoolImpl> _impl
            = new Lazy<ThreadPoolImpl>(() => new ThreadPoolImpl(), LazyThreadSafetyMode.ExecutionAndPublication);
    }

    class ThreadPoolImpl: IDisposable
    {
        /// <param name="priority">Приоритет исполнения задач из очереди задач. Пока не поддерживается, тк не используется BlockingPriorityQueue в реализации</param>
        internal void AddTask(Action task, int priority = 0)
        {
            if (_closed)
                return;
            _queue.Add(task);
        }

        internal ThreadPoolImpl()
        {
            _cancel = _cts.Token;
            _pool = Enumerable.Range(1, ThreadPoolSize).Select(_ => new Thread(Worker) {IsBackground = true}).ToList();
            _poolThreadManagedIds = _pool.Select(t => t.ManagedThreadId).ToHashSet();
            _pool.ForEach(t => t.Start());
        }

        private void Worker()
        {
            try
            {
                for(;;)
                try
                {
                    if (_cancel.IsCancellationRequested)
                        return;
                    Action task = null;
                    try
                    {
                        task = _queue.Take(_cancel);
                    }
                    catch (OperationCanceledException)
                    {
                        return;
                    }
                    task();
                }
                /*catch (OperationCanceledException)
                {
                }*/
                catch (Exception e)
                {
                    Locator.Services.Logger.LogEx(MethodBase.GetCurrentMethod().ToString(), e);
                }
            }
            catch
            { 
                //ignore
            }
        }

        internal bool ThisThreadFromThisThreadPool()
        {
            if(_pool is null)
                return false;
            try
            {
                var currentThreadManagedThreadId = Thread.CurrentThread?.ManagedThreadId;
                if (currentThreadManagedThreadId is null)
                    return false;
                if (_poolThreadManagedIds.Contains(currentThreadManagedThreadId.Value))
                    return true;
            }
            catch (Exception)
            {
                //ignore
            }
            return false;
        }

        public void Dispose()
        {
            try
            {
                _closed = true;
                var pool = _pool;
                _pool = null;
                if (pool is null)
                    return;
                _cts.Cancel();
                pool.ForEach(_ => _.Join());
                _cts.Dispose();
                _queue?.Dispose();
                _queue = null;
            }
            catch (Exception ex)
            {
                Locator.Services.Logger.LogEx(MethodBase.GetCurrentMethod().ToString(), ex);
            }
        }

        private int ThreadPoolSize => Math.Min(Environment.ProcessorCount * 2, MaxThreadPoolSize);
        private const int MaxThreadPoolSize = 8;

        private BlockingCollection<Action> _queue = new BlockingCollection<Action>();
        private readonly CancellationToken _cancel;
        private readonly CancellationTokenSource _cts = new CancellationTokenSource();
        private readonly HashSet<int> _poolThreadManagedIds;
        private List<Thread> _pool;
        private volatile bool _closed = false;
    }
}
