
//#include "include/gbafe.h" 
#include "gbafe.h" 
#define CHAR_NEWLINE 0x01
const bool isBarracks = true;
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
