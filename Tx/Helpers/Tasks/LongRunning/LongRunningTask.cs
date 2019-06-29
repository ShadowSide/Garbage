using Global.BusinessCommon.Helpers.ExceptionHelpers;
using GlobalInterfaces.Processing;
using Locator;
using System;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;

namespace Global.BusinessCommon.Helpers.Processing
{
    public abstract class LongRunningTask: ILongRunningTask
    {
        protected readonly object SyncRoot = new object();
        private readonly object SyncRootForCancelToken = new object();

        protected LongRunningTask(TaskScheduling scheduling = null /*SingleRunTaskScheduling*/, TimeSpan? stopTimeout = null /*Infinite*/)
        {
            Scheduling = scheduling ?? new SingleRunTaskScheduling();
            _stopTimeout = stopTimeout ?? TimeSpanConstants.Infinite;
        }

        public void Start()
        {
            using (CurrentLogger.ScopedTrace($"{GetType()}.{MethodBase.GetCurrentMethod()}"))
            {
                lock (SyncRoot)
                {
                    if (_disposed)
                        throw new ObjectDisposedException(nameof(LongRunningTask));
                    var task = _workerTask;
                    if (task != null)
                        return;
                    _cancelSource?.Dispose();
                    _cancelSource = new CancellationTokenSource();
                    var cancelToken = _cancelSource.Token;
                    CancelToken = cancelToken;
                    Scheduling.TaskName = GetType().ToString();
                    Scheduling.ProcessBody = (token) => Process(token);
                    task = new Task(() => Scheduling.SchedulingRun(cancelToken), cancelToken, TaskCreationOptions.LongRunning);
                    task.Start();
                    _workerTask = task;
                }
            }
        }

        public void Stop()
        {
            using (CurrentLogger.ScopedTrace($"{GetType()}.{MethodBase.GetCurrentMethod()}"))
            {
                lock (SyncRoot)
                {
                    if (_disposed)
                        throw new ObjectDisposedException(nameof(LongRunningTask));
                    InternalStop();
                }
            }
        }

        public void WaitForStopNoThrow(TimeSpan? stopTimeout = null)
        {
            using (CurrentLogger.ScopedTrace($"{GetType()}.{MethodBase.GetCurrentMethod()}"))
            {
                lock (SyncRoot)
                {
                    if (_disposed)
                        return;
                    ExceptionHelper.ExceptionCatcher(() =>
                    {
                        var task = _workerTask;
                        if (task == null)
                            return;
                        CurrentLogger.Trace($"Начинаем ожидание окончания задачи {GetType().Name} без её завершения ");
                        task.Wait(stopTimeout ?? TimeSpanConstants.Infinite);
                    }, where: nameof(LongRunningTask) + ":" + GetType() + "." + nameof(StopNoThrow));
                }
            }
        }

        public void StopNoThrow()
        {
            using (CurrentLogger.ScopedTrace($"{GetType()}.{MethodBase.GetCurrentMethod()}"))
            {
                lock (SyncRoot)
                {
                    if (_disposed)
                        return;
                    ExceptionHelper.ExceptionCatcher(InternalStop, where: nameof(LongRunningTask) + ":" + GetType() + "." + nameof(StopNoThrow));
                }
            }
        }

        protected void Cancel()
        {
            using (CurrentLogger.ScopedTrace($"{GetType()}.{MethodBase.GetCurrentMethod()}"))
            {
                _cancelSource.Cancel();
            }
        }

        public virtual void Dispose()
        {
            using (CurrentLogger.ScopedTrace($"{GetType()}.{MethodBase.GetCurrentMethod()}"))
            {
                lock (SyncRoot)
                {
                    if (_disposed)
                        return;
                    _disposed = true;
                    ExceptionHelper.ExceptionCatcher(InternalStop, where: nameof(LongRunningTask)+":"+GetType()+"."+ nameof(Dispose));
                    _cancelSource?.Dispose();
                }
            }
        }

        private void InternalStop()
        {
            using (CurrentLogger.ScopedTrace($"{GetType()}.{MethodBase.GetCurrentMethod()}"))
            {
                var task = _workerTask;
                if (task == null)
                    return;
                _workerTask = null;
                _cancelSource.Cancel();
                using (new RAII(task.Dispose))
                {
                    CurrentLogger.Trace($"Начинаем ожидание окончания задачи {GetType().Name}");
                    task.Wait(_stopTimeout);
                }
            }
        }

        /// <summary>
        /// Не хранит CancellationToken до первого старта задачи, а лишь null.
        /// После каждого старта задачи заменяется на новый.
        /// </summary>
        protected CancellationToken? CancelToken
        {
            get { lock (SyncRootForCancelToken) return _cancelToken; }
            private set { lock (SyncRootForCancelToken) _cancelToken = value; }
        }

        protected abstract void Process(CancellationToken token);

        protected internal TaskScheduling Scheduling { get; }

        private Task _workerTask;
        private CancellationTokenSource _cancelSource;
        private CancellationToken? _cancelToken;
        private bool _disposed;
        private readonly TimeSpan _stopTimeout;
    }
}
