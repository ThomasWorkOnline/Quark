namespace Quark
{
	public static class Logger
	{
		public enum LogLevel : byte
		{
			Trace = 0,
			Info,
			Warn,
			Error,
			Critical
		}

		public static void Log(string text, LogLevel logLevel = LogLevel.Info) => InternalCalls.NativeLog(text, logLevel);
	}
}
