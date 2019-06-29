using Global.BusinessCommon.Helpers.ExceptionHelpers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using GlobalInterfaces.Processing;

namespace Global.BusinessCommon.Helpers.Processing
{
    /// <summary>
    /// Планировщик, через паузу запускающий ProcessBody в цикле, пока его не попросят остановиться через cancellationToken
    /// </summary>
    public class PeriodicFlexibleTaskScheduling : TaskScheduling
    {
        public void WakeUpProcessing()
        {
            _waitingCts?.Cancel();
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
                if (token.IsCancellationRequested)
                    return;
                using (var waitingCts = new CancellationTokenSource())
                using (var tokenSubscriber = token.Register(waitingCts.Cancel))
                using (var ctsAbsolvent = new RAII(() => _waitingCts = null))
                {
                    var waitingToken = waitingCts.Token;
                    _waitingCts = waitingCts;
                    var interval = (IntervalCalculator ?? throw new ArgumentNullException(nameof(IntervalCalculator)))();
                    if (interval != TimeSpan.Zero && !token.IsCancellationRequested)
                        ThreadHelper.Sleep(interval ?? TimeSpanConstants.Infinite, waitingToken);
                }
           }
        }

        internal Func<TimeSpan?> IntervalCalculator { get; set; }

        private volatile CancellationTokenSource _waitingCts;
    }
}
