using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Global.BusinessCommon.Helpers.Processing
{
    /// <summary>
    /// Планировщик, один раз запускающий ProcessBody
    /// </summary>
    public class SingleRunTaskScheduling : TaskScheduling
    {
        public override void SchedulingRun(CancellationToken token)
        {
            try
            {
                ProcessBody(token);
            }
            catch (OperationCanceledException)
            {
                //ignore
            }
        }
    }
}
