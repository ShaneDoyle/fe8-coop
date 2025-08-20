
//#include "include/gbafe.h" 
#include "gbafe.h" 
#define CHAR_NEWLINE 0x01
const bool isBarracks = false;
const char* barracksRecruitableUnits[10] = {
"Cavalier",
"Knight",
"Mercenary",
"Mymidom",
"Archer",
"Wyvern Rider",
"Mage",
"Shaman",
"Fighter",
"UNKNOWN"};

extern int barrackUnitCounter;
extern u8 Img_PhaseChangeUnk[];
extern u8 Img_PhaseChangePlayer[];
extern u16 Pal_PhaseChangePlayer[];
extern u8 Img_PhaseChangeEnemy[];
extern u16 Pal_PhaseChangeEnemy[];
extern u8 Img_PhaseChangeOther[];
extern u16 Pal_PhaseChangeOther[];
extern u16 Pal_085A06B8[];
extern u8 Img_PhaseChangeSquares[];

void ShopSt_SetHeadLocBak(int unk);

struct FaceProc {
    /* 00 */ PROC_HEADER;

    /* 2C */ const struct FaceData* pFaceInfo;
    /* 30 */ u32 displayBits;
    /* 34 */ s16 xPos;
    /* 36 */ s16 yPos;

    /* 38 */ void* sprite;

    /* 3C */ u16 oam2;
    /* 3E */ u16 faceId;
    /* 40 */ u8 faceSlot;
    /* 41 */ u8 spriteLayer;

    /* 44 */ ProcPtr unk_44;
    /* 48 */ struct FaceBlinkProc* pBlinkProc;
};