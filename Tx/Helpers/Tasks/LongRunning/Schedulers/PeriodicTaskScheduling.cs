using Global.BusinessCommon.Helpers.ExceptionHelpers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Global.BusinessCommon.Helpers.Processing
{
    /// <summary>
    /// Планировщик, через паузу запускающий ProcessBody в цикле, пока его не попросят остановиться через cancellationToken
    /// </summary>
    public class PeriodicTaskScheduling : TaskScheduling
    {
        private readonly TimeSpan _interval;

        public PeriodicTaskScheduling(TimeSpan interval)
        {
            _interval = interval;
        }

        public override void SchedulingRun(CancellationToken token)
        {
           while (!token.IsCancellationRequested)
           {
                ExceptionHelper.ExceptionCatcher(() =>
                {
                    try
                    {
                        ProcessBody(token);
                    }
                    catch (OperationCanceledException)
                    {
                        //ignore
                    }
                }, where: nameof(PeriodicTaskScheduling) + " for : '" + TaskName + "'");
                ThreadHelper.Sleep(_interval, token);
           }
        }
    }
}
