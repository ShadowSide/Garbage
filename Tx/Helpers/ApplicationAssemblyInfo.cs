using Delimon.Win32.IO;
using Global.BusinessCommon.Helpers.Monads;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;

namespace Global.BusinessCommon.Helpers.FileSystem
{
    public static class ApplicationAssemblyInfo
    {
        /// <summary>
        /// Полный путь к директории экзешника
        /// </summary>
        public static string CurrentExecutableDirectory => TryChain.Try(nameof(CurrentExecutableDirectory),
            () => Path.GetDirectoryName(DisposableHelpers.GetFromDisposable(System.Diagnostics.Process.GetCurrentProcess, _ => _.MainModule.FileName)),
            () => AppDomain.CurrentDomain.BaseDirectory,
            () => Path.GetDirectoryName(Assembly.GetEntryAssembly().Location),
            () => Path.GetDirectoryName(Environment.GetCommandLineArgs().FirstOrDefault()));

        /// <summary>
        /// Имя экзешника
        /// </summary>
        public static string CurrentExecutableName => TryChain.Try(nameof(CurrentExecutableName),
            () => Path.GetFileName(DisposableHelpers.GetFromDisposable(System.Diagnostics.Process.GetCurrentProcess, _ => _.MainModule.FileName)),
            () => Path.GetFileName(Assembly.GetEntryAssembly().Location),
            () => Path.GetFileName(Environment.GetCommandLineArgs().FirstOrDefault()));

        /// <summary>
        /// Полный путь к экзешнику с именем файла
        /// </summary>
        public static string CurrentExecutablePath => TryChain.Try(nameof(CurrentExecutablePath),
            () => DisposableHelpers.GetFromDisposable(System.Diagnostics.Process.GetCurrentProcess, _ => _.MainModule.FileName),
            () => Assembly.GetEntryAssembly().Location,
            () => Environment.GetCommandLineArgs().FirstOrDefault());
    }
}
