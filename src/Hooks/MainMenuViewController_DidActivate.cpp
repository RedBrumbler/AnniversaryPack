#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

MAKE_AUTO_HOOK_MATCH(MainMenuViewController_DidActivate, &::GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}