#pragma once
#include <string>


enum LogLevel { LVL_DEBUG, LVL_INFO, LVL_WARNING, LVL_ERROR, LVL_OFF };

/**
 * @class Logger
 * @brief Logger class
 *
 */
class Logger {
 public:
  /**
   * @brief Construct a new Logger object
   *
   * @param logName a name for the logger
   * @param minLvl only messages >= minLvl will be displayed
   */
  Logger(std::string& logName, LogLevel minLvl = LVL_INFO);

  /**
   * @brief Destroy the Logger object
   *
   */
  ~Logger(){};

  /**
   * @brief Log a message
   *
   * @param level the log level
   * @param szMessage the message to be logged
   * @param ... variadic
   */
  virtual void Log(LogLevel level, const char* szMessage, ...);

  /**
   * @brief Set the log level
   *
   * @param lvl
   */
  virtual void SetLoglevel(LogLevel lvl) { MinLogLvl_ = lvl; };

 private:
  std::string LoggerName_;
  LogLevel MinLogLvl_;

};

//! \brief mhh jaa, a global logger object
extern Logger SSBLLogger;



//! \brief Log a message
#define LOG(loglevel, ...) (SSBLLogger).Log(loglevel, __VA_ARGS__)

//! \brief Log a debug message
#define LOG_DEBUG(...) LOG(LogLevel::LVL_DEBUG, __VA_ARGS__)
//! \brief Log a info message
#define LOG_INFO(...) LOG(LogLevel::LVL_INFO, __VA_ARGS__)
//! \brief Log a warning message
#define LOG_WARNING(...) LOG(LogLevel::LVL_WARNING, __VA_ARGS__)
//! \brief Log a error message
#define LOG_ERROR(...) LOG(LogLevel::LVL_ERROR, __VA_ARGS__)
