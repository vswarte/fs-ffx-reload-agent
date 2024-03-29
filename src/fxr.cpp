#include "fxr.h"
#include "logging.h"

int offsetPatchFxrPointers = 0x2066670;
int offsetWtfFxr = 0x208f780;
int offsetGetFxrAllocator = 0x20220e0;
int offsetCSSfxImpPtr = 0x3c68768;

void reload_fxr(int fxrId, char *buffer, int length) {
    auto baseAddress = (uintptr_t) GetModuleHandleA("eldenring.exe");
    logging::write_line("Base address: %p", baseAddress);

    CSSfxImp* sfxImpPtr = *(CSSfxImp**)(baseAddress + offsetCSSfxImpPtr);

    logging::write_line("Running sanity checks...");
    if (!sanity_checks(sfxImpPtr)) {
        logging::write_line("Sanity checks failed");
        return;
    }

    auto fxrList = sfxImpPtr->sceneCtrl->graphicsResourceManager->resContainer->fxrList;
    auto node = fxrList->head;
    while (node != nullptr && node->next != (FXRListNode*) fxrList) {
        if (node->fxrw == nullptr){
            continue;
        }

        auto fxr = node->fxrw->fxr;
        if (fxr->ffxId == fxrId) {
            swap_fxr_entry(baseAddress, node->fxrw, buffer, length);
        }

        node = node->next;
    }
}

void swap_fxr_entry(uintptr_t baseAddress, FXRWrapper* wrapper, char *buffer, int length) {
    auto fpPatchFxrPointers = (PatchFxrPointers) (baseAddress + offsetPatchFxrPointers);
    auto fpWtfFxr = (WtfFxr) (baseAddress + offsetWtfFxr);
    auto fpGetFxrAllocator = (GetFxrAllocator) (baseAddress + offsetGetFxrAllocator);

    void *fxrAllocator = fpGetFxrAllocator();
    FxrAlloc fxrAlloc = (*(FxrAlloc**)(fxrAllocator))[10];

    void *fxrData = fxrAlloc(fxrAllocator, length, 0x10);
    memcpy(fxrData, buffer, length);

    fpPatchFxrPointers(fxrData, fxrData, fxrData);
    fpWtfFxr(fxrData);
    wrapper->fxr = (FXRRoot*) fxrData;
    logging::write_line("Replaced FXR");
}

bool sanity_checks(CSSfxImp* sfxImpPtr) {
    logging::write_line("Checking SfxImp != nullptr");
    if (sfxImpPtr == nullptr) {
        logging::write_line("SfxImp == nullptr");
        return false;
    }

    logging::write_line("Checking sceneCtrl != nullptr");
    auto sceneCtrl = sfxImpPtr->sceneCtrl;
    if (sceneCtrl == nullptr) {
        logging::write_line("sceneCtrl == nullptr");
        return false;
    }

    logging::write_line("Checking resMan != nullptr");
    auto resMan = sceneCtrl->graphicsResourceManager;
    if (resMan == nullptr) {
        logging::write_line("resMan == nullptr");
        return false;
    }

    logging::write_line("Checking resContainer != nullptr");
    auto resContainer = resMan->resContainer;
    if (resContainer == nullptr) {
        logging::write_line("resContainer == nullptr");
        return false;
    }

    logging::write_line("Checking fxrList != nullptr");
    auto fxrList = resContainer->fxrList;
    if (fxrList == nullptr) {
        logging::write_line("fxrList == nullptr");
        return false;
    }

    return true;
}
