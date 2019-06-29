using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Global.BusinessCommon.Helpers.Monads
{
    public static class DisposableHelpers
    {
        /// <summary>
        /// Конструирует Disposable объект и получает из него поля, указанные в infoGetter
        /// </summary>
        public static Info GetFromDisposable<DisposableSource, Info>(Func<DisposableSource> sourceMaker, Func<DisposableSource, Info> infoGetter)
            where DisposableSource : IDisposable
        {
            var _ = sourceMaker ?? throw new ArgumentNullException(nameof(sourceMaker));
            var __ = infoGetter ?? throw new ArgumentNullException(nameof(infoGetter));
            using (var ds = sourceMaker())
                return infoGetter(ds);
        }
    }
}
