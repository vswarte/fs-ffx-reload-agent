#pragma once

#include <cstdint>

struct FXRRoot {
    char magic[4]; // 0x0
    short pad04; // 0x4
    short version; // 0x6
    uint32_t unk08; // 0x8
    uint32_t ffxId; // 0xc
    uint32_t sec1Offset; //0x10
    uint32_t sec1Count; // 0x14
    uint32_t sec2Offset; //0x18
    uint32_t sec2Count; // 0x1c
    uint32_t sec3offset; //0x20
    uint32_t sec3count; // 0x24
    uint32_t sec4offset; //0x28
    uint32_t sec4count; // 0x2c
    uint32_t sec5offset; //0x30
    uint32_t sec5count; // 0x34
    uint32_t sec6offset; //0x38
    uint32_t sec6count; // 0x3c
    uint32_t sec7offset; //0x40
    uint32_t sec7count; // 0x44
    uint32_t sec8offset; //0x48
    uint32_t sec8count; // 0x4c
    uint32_t sec9offset; //0x50
    uint32_t sec9count; // 0x54
    uint32_t sec10offset; //0x58
    uint32_t sec10count; // 0x5c
    uint32_t sec11offset; //0x60
    uint32_t sec11count; // 0x64
};

struct FXRWrapper {
    FXRRoot* fxr;
    uint64_t unk;
};

struct FXRListNode {
    FXRListNode* next;
    FXRListNode* prev;
    uint64_t hash;
    FXRWrapper* fxrw;
    void* unk20; // 0x20
};

struct FXRList {
    FXRListNode* head;
    FXRListNode* tail;
    void* unk10;
};

struct FXRResourceContainer {
    void* allocator;
    void* sceneCtrl;
    FXRList* fxrList;
    uint64_t resCount;
};

struct GXFfxGraphicsResourceManager {
    void* vftable; // 0x00
    char unk[0x158]; // 0x08
    FXRResourceContainer* resContainer; // 0x160
};

struct GXFfxSceneCtrl {
    void* vftable; // 0x00
    void* sgEntity; // 0x08
    void* allocator; // 0x10
    void* ffxManager; // 0x18
    uint64_t unk; // 0x20
    GXFfxGraphicsResourceManager* graphicsResourceManager; // 0x28
};

struct SprjSfxImp {
    void* vftable; // 0x0
    char unk[0x50]; // 0x8
    GXFfxSceneCtrl* sceneCtrl; // 0x58
};

typedef void* (*PatchFxrPointers)(void*, void*, void*);
typedef void* (*WtfFxr)(void*);
typedef void* (*GetFxrArchive)();
typedef void* (*FxrAlloc)(void *, int, int);

void reload_fxr(int fxrId, char *buffer, int length);
void swap_fxr_entry(FXRWrapper* wrapper, char *buffer, int length);
bool sanity_checks();
