using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Global.BusinessCommon.Helpers.Processing
{
    /// <summary>
    /// Базовый класс планировщика долгоработающих тасок
    /// </summary>
    public abstract class TaskScheduling
    {
        /// <summary>
        /// Тело, вызываемое в классе LongRunningTask и его потомках
        /// </summary>
        public abstract void SchedulingRun(CancellationToken token);

        /// <summary>
        /// Тело, исполняющее задачу, вызывается из метода SchedulingRun
        /// Класс LongRunningTask устанавливает метод, вызываемый из него
        /// </summary>
        internal Action<CancellationToken> ProcessBody
        {
            get
            {
                return _processBody;
            }
            set
            {
                if (value is null)
                    throw new ArgumentNullException(nameof(ProcessBody));
                _processBody = value;
            }
        }

        internal string TaskName { get; set; } = string.Empty;

        private Action<CancellationToken> _processBody;
    }
}
