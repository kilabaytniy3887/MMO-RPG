#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <ctime>
#include <sstream>
#include <algorithm>

struct LogEvent {
    std::string timestamp;
    std::string level;
    std::string source;
    std::string message;

    static std::string getCurrentTimestamp() {
        std::time_t now = std::time(nullptr);
        std::tm timeInfo;
#ifdef _MSC_VER
        localtime_s(&timeInfo, &now);
#else
        localtime_r(&now, &timeInfo);
#endif
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);
        return buf;
    }
};

// --- Adapter Pattern: Formatters ---
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const LogEvent& event) = 0;
};

class TextFormatter : public ILogFormatter {
public:
    std::string format(const LogEvent& event) override {
        return "[" + event.timestamp + "] [" + event.level + "] [" + event.source + "] " + event.message;
    }
};

class JsonFormatter : public ILogFormatter {
private:
    std::string escape(const std::string& str) {
        std::string res;
        for (char c : str) {
            if (c == '"') res += "\\\"";
            else if (c == '\\') res += "\\\\";
            else if (c == '\n') res += "\\n";
            else if (c == '\r') res += "\\r";
            else if (c == '\t') res += "\\t";
            else res += c;
        }
        return res;
    }
public:
    std::string format(const LogEvent& event) override {
        return "{\"timestamp\": \"" + escape(event.timestamp) + "\", \"level\": \"" + escape(event.level) +
            "\", \"source\": \"" + escape(event.source) + "\", \"message\": \"" + escape(event.message) + "\"}";
    }
};

class CsvFormatter : public ILogFormatter {
public:
    std::string format(const LogEvent& event) override {
        auto sanitize = [](std::string s) {
            std::replace(s.begin(), s.end(), ';', ',');
            std::replace(s.begin(), s.end(), '\n', ' ');
            std::replace(s.begin(), s.end(), '\r', ' ');
            return s;
            };
        return event.timestamp + ";" + event.level + ";" + event.source + ";" + sanitize(event.message);
    }
};

// --- Loggers ---
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEvent& event) = 0;
    virtual void setFormatter(std::shared_ptr<ILogFormatter> fmt) = 0;
};

class NullLogger : public ILogger {
public:
    void log(const LogEvent&) override {}
    void setFormatter(std::shared_ptr<ILogFormatter>) override {}
};

class ConsoleLogger : public ILogger {
private:
    std::shared_ptr<ILogFormatter> formatter;
public:
    ConsoleLogger(std::shared_ptr<ILogFormatter> fmt) : formatter(fmt) {}
    void log(const LogEvent& event) override {
        if (formatter) std::cout << formatter->format(event) << "\n";
    }
    void setFormatter(std::shared_ptr<ILogFormatter> fmt) override { formatter = fmt; }
};

class FileLogger : public ILogger {
private:
    std::ofstream file;
    std::shared_ptr<ILogFormatter> formatter;
public:
    FileLogger(const std::string& filename, std::shared_ptr<ILogFormatter> fmt)
        : file(filename, std::ios::app), formatter(fmt) {
        if (!file.is_open()) {
            std::cerr << "Failed to open log file: " << filename << "\n";
        }
    }
    ~FileLogger() override {
        if (file.is_open()) file.close();
    }
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    void log(const LogEvent& event) override {
        if (file.is_open() && formatter) {
            file << formatter->format(event) << "\n";
            file.flush();
        }
    }
    void setFormatter(std::shared_ptr<ILogFormatter> fmt) override { formatter = fmt; }
};

// --- Proxy Pattern ---
class LoggingProxy {
private:
    std::unique_ptr<ILogger> currentLogger;
    std::shared_ptr<ILogFormatter> formatter;
public:
    LoggingProxy() : formatter(std::make_shared<TextFormatter>()) {
        currentLogger = std::make_unique<ConsoleLogger>(formatter);
    }

    void setLogger(std::unique_ptr<ILogger> logger) {
        currentLogger = std::move(logger);
        if (currentLogger) currentLogger->setFormatter(formatter);
    }

    void setFormatter(std::shared_ptr<ILogFormatter> fmt) {
        formatter = fmt;
        if (currentLogger) currentLogger->setFormatter(formatter);
    }

    std::shared_ptr<ILogFormatter> getFormatter() const { return formatter; }

    void log(const std::string& level, const std::string& source, const std::string& message) {
        if (currentLogger) {
            LogEvent event;
            event.level = level;
            event.source = source;
            event.message = message;
            event.timestamp = LogEvent::getCurrentTimestamp();
            currentLogger->log(event);
        }
    }
};