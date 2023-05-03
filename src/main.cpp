#include "logging.hpp"
#include "hooking.hpp"
#include "assets.hpp"

#include "playlistcore/shared/PlaylistCore.hpp"

ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

// from playlistcore
std::string GetPlaylistsPath();

extern "C" void load() {
    Hooks::InstallHooks(getLogger());

    writefile(GetPlaylistsPath() + "/AnniversaryPack.bplist", IncludedAssets::playlist_bplist);
}
