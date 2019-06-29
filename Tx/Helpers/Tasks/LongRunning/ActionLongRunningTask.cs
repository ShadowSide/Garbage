using System;
using System.Threading;

namespace Global.BusinessCommon.Helpers.Processing
{
    /// <summary>
    /// Класс, принимающий вызываемую сущность (Action) и запускающий ее в рамках длительной задачи.
    /// </summary>
    public class ActionLongRunningTask: LongRunningTask
    {
        private readonly Action<CancellationToken> _work;

        public ActionLongRunningTask(Action<CancellationToken> work, TaskScheduling scheduling = null, TimeSpan? stopTimeout = null /*Infinite*/)
            : base(scheduling, stopTimeout)
        {
            _work = work ?? throw new ArgumentNullException(nameof(work));
        }

        public ActionLongRunningTask(Action work, TaskScheduling scheduling = null, TimeSpan? stopTimeout = null /*Infinite*/)
            : base(scheduling, stopTimeout)
        {
            if (work is null)
                throw new ArgumentNullException(nameof(work));
            _work = (_) => work();
        }

        protected override void Process(CancellationToken token)
        {
            _work(token);
        }
    }
}
