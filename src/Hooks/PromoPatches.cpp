#include "hooking.hpp"
#include "logging.hpp"
#include "assets.hpp"

#include "songloader/shared/API.hpp"
#include "playlistcore/shared/PlaylistCore.hpp"
#include "bsml/shared/Helpers/utilities.hpp"

#include "GlobalNamespace/DlcPromoPanelDataSO_MusicPackPromoInfo.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator_State.hpp"
#include "GlobalNamespace/LevelFilteringNavigationController.hpp"
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/DlcPromoPanelModel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "System/Nullable_1.hpp"
#include "System/Action_2.hpp"

using namespace GlobalNamespace;

bool packPromoButtonWasPressed = false;

MAKE_AUTO_HOOK_MATCH(LevelFilteringNavigationController_ShowPacksInSecondChildController, &LevelFilteringNavigationController::ShowPacksInSecondChildController, void, LevelFilteringNavigationController* self, ::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::IBeatmapLevelPack*>* beatmapLevelPacks) {
    if (!(self->levelPackIdToBeSelectedAfterPresent || packPromoButtonWasPressed))
        self->levelPackIdToBeSelectedAfterPresent = "BSMG's Fifth Anniversary Music Pack";
    LevelFilteringNavigationController_ShowPacksInSecondChildController(self, beatmapLevelPacks);
}

MAKE_AUTO_HOOK_MATCH(LevelSelectionFlowCoordinator_DidActivate, &LevelSelectionFlowCoordinator::DidActivate, void, LevelSelectionFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (!(!addedToHierarchy || !packPromoButtonWasPressed)) {
        packPromoButtonWasPressed = false;
        auto playlists = PlaylistCore::GetLoadedPlaylists();
        auto playlistItr = std::find_if(playlists.begin(), playlists.end(), [](auto p){ return p->name == "BSMG's Fifth Anniversary Music Pack"; });
        if (playlistItr != playlists.end()) {
            IBeatmapLevelPack* pack = (*playlistItr)->playlistCS->i_IBeatmapLevelPack();
            self->startState = LevelSelectionFlowCoordinator::State::New_ctor(
                ::System::Nullable_1<SelectLevelCategoryViewController::LevelCategory>(
                    SelectLevelCategoryViewController::LevelCategory::CustomSongs,
                    true
                ),
                pack,
                nullptr,
                nullptr
            );
        }
    }

    LevelSelectionFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}

// WARN: DOES NOT RUN ORIG
MAKE_AUTO_HOOK_ORIG_MATCH(MainMenuViewController_PackPromoButtonWasPressed, &MainMenuViewController::PackPromoButtonWasPressed, void, MainMenuViewController* self) {
    if (RuntimeSongLoader::API::HasLoadedSongs()) {
        packPromoButtonWasPressed = true;
        self->musicPackPromoButtonWasPressedEvent->Invoke(RuntimeSongLoader::API::GetCustomLevelsPack()->CustomLevelsPack->i_IBeatmapLevelPack(), nullptr);
    }
}

// WARN: DOES NOT RUN ORIG
MAKE_AUTO_HOOK_ORIG_MATCH(DlcPromoPanelModel_GetPackDataForMainMenuPromoBanner, &DlcPromoPanelModel::GetPackDataForMainMenuPromoBanner, DlcPromoPanelDataSO::MusicPackPromoInfo*, DlcPromoPanelModel* self, ByRef<bool> owned) {
    owned = false;

    auto packID = self->dlcPromoPanelData->get_defaultMusicPackPromo()->get_previewBeatmapLevelPack()->get_packID();
    if (self->playerDataModel->get_playerData()->currentDlcPromoId != packID) {
        self->playerDataModel->get_playerData()->SetNewDlcPromo(packID);
    }
    auto pack = DlcPromoPanelDataSO::MusicPackPromoInfo::New_ctor();
    pack->bannerImage = BSML::Utilities::LoadSpriteRaw(IncludedAssets::PromoImage_png);
    pack->beatmapLevelPack = reinterpret_cast<PreviewBeatmapLevelPackSO*>(self->dlcPromoPanelData->get_defaultMusicPackPromo()->get_previewBeatmapLevelPack());

    return pack;
}
