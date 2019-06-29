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
    public abstract class TimedFlexibleLongRunningTask : LongRunningTask
    {
        public TimedFlexibleLongRunningTask(TimeSpan? stopTimeout = null /*Infinite*/)
            : base(new PeriodicFlexibleTaskScheduling(), stopTimeout)
        {
            Scheduler = Scheduling as PeriodicFlexibleTaskScheduling ?? throw new NullReferenceException(nameof(Scheduling));
            Scheduler.IntervalCalculator = IntervalCalculator;
        }

        protected virtual void WakeUpProcessing() => Scheduler.WakeUpProcessing();

        protected abstract TimeSpan? IntervalCalculator();

        protected abstract override void Process(CancellationToken token);

        private readonly PeriodicFlexibleTaskScheduling Scheduler;
    }
}