#include "pch.h"
#include "Logger.h"

#undef GetCurrentDirectory
#pragma warning(push)
// NLog imports from netstandard - we support it, but the compiler still complains
// Make it shut up:
#pragma warning(disable: 4691)


using namespace System::Diagnostics;
using namespace NLog;
using namespace NLog::Targets;


FileTarget^ NetModAPI::Logger::CreateFileLogger(String^ log_path, String^ log_suffix) {
    FileTarget^ log_file = gcnew FileTarget("logfile"+log_suffix);
    log_file->FileName = Layouts::Layout::FromString(log_path + "latest" + log_suffix + ".txt");
    log_file->ArchiveOldFileOnStartup = true;
    log_file->Footer = Layouts::Layout::FromString("End of log");
    log_file->ArchiveFileName = Layouts::Layout::FromString(log_path + "log" + log_suffix + ".{#}.txt");
    log_file->Layout = Layouts::Layout::FromString("${longdate} ${uppercase:${level}} ${message} ${exception:format=tostring}");
    log_file->MaxArchiveFiles = 5;
    log_file->CreateDirs = true;
    log_file->AutoFlush = true;

    return log_file;
}



static NetModAPI::Logger::Logger() {
    config = gcnew Config::LoggingConfiguration();

    // Targets where to log to: File and Console
    FileTarget^ log_file = CreateFileLogger(log_path, "");
    FileTarget^ error_log_file = CreateFileLogger(log_path, "-error");
    ColoredConsoleTarget^ logConsole = gcnew ColoredConsoleTarget("logconsole");
    logConsole->DetectConsoleAvailable = true;

    // Rules for mapping loggers to targets
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, logConsole, "*");
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, log_file, "global");
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, error_log_file, "error");

    // Apply config           
    LogManager::Configuration = config;
    log = LogManager::GetLogger("global");
    log->Info("Started logging");

    error_log = LogManager::GetLogger("error");
    error_log->Info("Started Error logging");
}

ILogger^ NetModAPI::Logger::GetLogger(String^ name) {
    if (name == nullptr || !loggers->ContainsKey(name)) return log;
    return loggers[name];
}

ILogger^ NetModAPI::Logger::GetErrorLogger(String^ name) {
    if (name == nullptr || !error_loggers->ContainsKey(name)) return error_log;
    return error_loggers[name];
}

void NetModAPI::Logger::AddLogger(String^ name) {
    auto logger = CreateFileLogger(log_path, "@" + name);
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, logger, name);

    auto error_logger = CreateFileLogger(log_path, "@" + name + "-error");
    config->AddRule(LogLevel::Debug, LogLevel::Fatal, error_logger, name + "-error");

    // Apply config
    LogManager::Configuration = config;

    loggers->Add(name, LogManager::GetLogger(name));
    error_loggers->Add(name, LogManager::GetLogger(name + "-error"));
}

void NetModAPI::Logger::LogInfo(String^ msg, [System::Runtime::InteropServices::Optional]String^ logger) {
    GetLogger(logger)->Info(msg);
}

void NetModAPI::Logger::LogDebug(String^ msg, [System::Runtime::InteropServices::Optional]String^ logger) {
    GetLogger(logger)->Debug(msg);
}

void NetModAPI::Logger::LogWarn(String^ msg, [System::Runtime::InteropServices::Optional]String^ logger) {
    GetLogger(logger)->Warn(msg);
}

void NetModAPI::Logger::LogError(String^ msg, Exception^ exception, [System::Runtime::InteropServices::Optional]String^ logger) {
    GetErrorLogger(logger)->Error(exception, msg);
    Debugger::Break();
}

#pragma warning(pop)
