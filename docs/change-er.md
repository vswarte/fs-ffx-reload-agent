# Changes that need to be done for ER support
 - [x] Replace pointers
   - [x] Pointers
     - [x] PatchFxrPointers (offset 0x2059160)
     - [x] WtfFxr (offset 0x2082270)
     - [x] GetFxrAllocator (offset 0x2014bd0)
     - [x] CSSfxImp (offset 0x3c81ba8)
   - [x] Fix structures
 - [ ] Upgrade to 1.05.0
    - [ ] Replace pointers
    - [ ] Method pointers
        - [ ] PatchFxrPointers
        - [ ] WtfFxr
        - [ ] GetFxrAllocator
        - [ ] CSSfxImp
    - [ ] Fix structures
 - [x] Testing this garbage (1.03.2)
 - [ ] Testing this garbage (1.05.0)

## Changes to method pointers

DS3
```c++
PatchFxrPointers fpPatchFxrPointers = (PatchFxrPointers)0x1421479b0;
WtfFxr fpWtfFxr = (WtfFxr)0x14214a8a0;
GetFxrAllocator fpGetFxrAllocator = (GetFxrAllocator) 0x14025ed90;
SprjSfxImp** CSSfxImp = (SprjSfxImp**) 0x1447843c8;
```

ER 1.3.2.0
```c++
PatchFxrPointers fpPatchFxrPointers = (PatchFxrPointers) BaseAddress + 0x2059160;
WtfFxr fpWtfFxr = (WtfFxr) BaseAddress + 0x2082270;
GetFxrAllocator fpGetFxrAllocator = (GetFxrAllocator) BaseAddress + 0x2014bd0;
SprjSfxImp** CSSfxImp = (SprjSfxImp**) BaseAddress + 0x3c81ba8;
```

## Structures

DS3
```c++
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
```

ER 1.3.2.0
```c++
struct FXRWrapper {
    FXRRoot* fxr; // 0x0
    uint64_t unk; // 0x8
};

struct FXRListNode {
    FXRListNode* next; // 0x0
    FXRListNode* prev; // 0x8
    uint64_t hash; // 0x10
    FXRWrapper* fxrw; // 0x18
    void* unk20; // 0x20
};

struct FXRList {
    FXRListNode* head; // 0x0
    FXRListNode* tail; // 0x8
    void* unk10; // 0x10
};

struct FXRResourceContainer {
    void* allocator1; // 0x0
    void* sceneCtrl; // 0x8
    void* unk10; // 0x10
    void* allocator2; // 0x18
    FXRList* fxrList; // 0x20
    uint64_t resCount; // 0x28
};

struct GXFfxGraphicsResourceManager {
    void* vftable; // 0x0
    char unk[0x158]; // 0x8
    FXRResourceContainer* resContainer; // 0x160
};

struct GXFfxSceneCtrl {
    void* vftable; // 0x0
    void* sgEntity; // 0x8
    void* allocator; // 0x10
    void* ffxManager; // 0x18
    uint64_t unk; // 0x20
    GXFfxGraphicsResourceManager* graphicsResourceManager; // 0x28
};

struct CSSfxImp {
    void* vftable; // 0x0
    char unk[0x58]; // 0x8
    GXFfxSceneCtrl* sceneCtrl; // 0x60
};
```
