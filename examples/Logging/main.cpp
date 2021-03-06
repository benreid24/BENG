#include <BLIB/Logging.hpp>
#include <iostream>

int main() {
    bl::logging::Config::addFileOutput("example.log", bl::logging::Config::Trace);
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Info);

    // Can use the logger directly
    bl::logging::Logger::info() << "This is a log number: " << 5;

    // Or use macros which add file, function, and line
    BL_LOG_INFO << "This will have file info";
    BL_LOG_CRITICAL << "This is really bad";
    BL_LOG_ERROR << "This is an error";
    BL_LOG_WARN << "Just a warning";
    BL_LOG_INFO << "Some info";
    BL_LOG_DEBUG << "Low level, will go to file";
    BL_LOG_TRACE << "Low level, high volume";

    return 0;
}