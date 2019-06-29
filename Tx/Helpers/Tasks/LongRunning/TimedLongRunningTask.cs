using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Global.BusinessCommon.Helpers.Processing
{
    /// <summary>
    /// Класс периодически запускающий Process в рамках длительной задачи.
    /// </summary>
    public abstract class TimedLongRunningTask : LongRunningTask
    {
        public TimedLongRunningTask(TimeSpan interval, TimeSpan? stopTimeout = null /*Infinite*/)
            : base(new PeriodicTaskScheduling(interval), stopTimeout)
        {
        }

        protected abstract override void Process(CancellationToken token);
    }
}
