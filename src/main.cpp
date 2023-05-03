#include "logging.hpp"
#include "hooking.hpp"

ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

extern "C" void load() {
    Hooks::InstallHooks(getLogger());
}