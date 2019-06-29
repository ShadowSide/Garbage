using System;
using System.Threading;
using System.Threading.Tasks;
using Global.BusinessCommon.Helpers.ExceptionHelpers;
using GlobalInterfaces.Processing;

namespace Global.BusinessCommon.Helpers.Processing
{
    /// <summary>
    /// Класс, принимающий вызываемую сущность (Action) и периодически запускающий ее в рамках длительной задачи.
    /// </summary>
    public class TimedActionLongRunningTask : ActionLongRunningTask
    {
        public TimedActionLongRunningTask(TimeSpan interval, Action<CancellationToken> work, TimeSpan? stopTimeout = null /*Infinite*/)
            : base(work, new PeriodicTaskScheduling(interval), stopTimeout)
        {
        }

        public TimedActionLongRunningTask(TimeSpan interval, Action work, TimeSpan? stopTimeout = null /*Infinite*/)
            : base(work, new PeriodicTaskScheduling(interval), stopTimeout)
        {
        }
    }
}
