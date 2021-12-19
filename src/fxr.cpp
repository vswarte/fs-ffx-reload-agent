#include "fxr.h"
#include "logging.h"

PatchFxrPointers fpPatchFxrPointers = (PatchFxrPointers)0x1421479b0;
WtfFxr fpWtfFxr = (WtfFxr)0x14214a8a0;
GetFxrArchive fpGetFxrGlobal = (GetFxrArchive) 0x14025ed90;
SprjSfxImp** SfxImpPtr = (SprjSfxImp**) 0x1447843c8;

void reload_fxr(int fxrId, char *buffer, int length) {
    if (!sanity_checks()) {
        logging::write_line("Sanity checks failed");
        return;
    }

    SprjSfxImp* sfxImp = *SfxImpPtr;
    auto fxrList = sfxImp->sceneCtrl->graphicsResourceManager->resContainer->fxrList;
    auto node = fxrList->head;
    while (node != nullptr && node->next != (FXRListNode*) fxrList) {
        if (node->fxrw == nullptr){
            continue;
        }

        auto fxr = node->fxrw->fxr;
        if (fxr->ffxId == fxrId) {
            swap_fxr_entry(node->fxrw, buffer, length);
        }

        node = node->next;
    }
}

void swap_fxr_entry(FXRWrapper* wrapper, char *buffer, int length) {
    void *fxrArchive = fpGetFxrGlobal();
    FxrAlloc fxrAlloc = (*(FxrAlloc**)(fxrArchive))[10];

    void *fxrData = fxrAlloc(fxrArchive, length, 0x10);
    memcpy(fxrData, buffer, length);

    fpPatchFxrPointers(fxrData, fxrData, fxrData);
    fpWtfFxr(fxrData);
    wrapper->fxr = (FXRRoot*) fxrData;
}

bool sanity_checks() {
    SprjSfxImp* sfxImp = *SfxImpPtr;
    if (sfxImp == nullptr) {
        logging::write_line("SfxImp == nullptr");
        return false;
    }

    auto sceneCtrl = sfxImp->sceneCtrl;
    if (sceneCtrl == nullptr) {
        logging::write_line("sceneCtrl == nullptr");
        return false;
    }

    auto resMan = sceneCtrl->graphicsResourceManager;
    if (resMan == nullptr) {
        logging::write_line("resMan == nullptr");
        return false;
    }

    auto resContainer = resMan->resContainer;
    if (resContainer == nullptr) {
        logging::write_line("resContainer == nullptr");
        return false;
    }

    auto fxrList = resContainer->fxrList;
    if (fxrList == nullptr) {
        logging::write_line("fxrList == nullptr");
        return false;
    }

    return true;
}
