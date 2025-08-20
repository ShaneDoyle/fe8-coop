#include "C_Code.h"
#include "include/unitlistscreen.h"
#define barracksRecruitableUnits ((const char *)0x9000000)

/********** Barracks.c **********/


/* Original */
extern UnitIconWait unit_icon_wait_table[];
#define GetInfo(id) (unit_icon_wait_table[(id) & ((1<<7)-1)])
extern int unused_0203e760; // P2 flag.

enum
{
	IS_BARRACKS = 0,
	BARRACKS_ROW,
	BARRACKS_TAB,
	WINDOW_COLOUR_OVERRIDE

};
extern u8 gUnused_0203E884[10]; // Barracks Unit List.
extern int gUnused_0859166C; // Should do barracks or not.

/* New */
int Barracks_GetLineOffset(){
	return (gShopState->bg2_off)/16;
}
int Barracks_GetListIndex(){
    return (gShopState->head_loc) + (Barracks_GetLineOffset());
}

// Barrack Unit information (TODO: Make this read from class tables instead of hardcoding our own values).
typedef struct {
	const char *name;
	int classId;
	int price;
	int standingMapSprite;
	int weapon[2];
	int startStats[8];
} barrackUnitDef;

static barrackUnitDef barrackUnits[8][20] = {
    // WORD
    {
        {"Cavalier",  	CLASS_CAVALIER,   	3000, 0x04, {ITYPE_SWORD, ITYPE_LANCE}, {20, 6, 4, 6, 3, 6, 1, 7}},
        {"Mercenary", 	CLASS_MERCENARY,  	3000, 0x0A, {ITYPE_SWORD, -1}, {20, 5, 6, 6, 3, 2, 1, 5}},
        {"Myrmidon",   CLASS_MYRMIDON,		2500, 0x0E, {ITYPE_SWORD, -1}, {19, 4, 6, 7, 3, 1, 2, 5}},
        {"Thief",   CLASS_THIEF,   			3500, 0x08, {ITYPE_SWORD, -1}, {18, 3, 7, 7, 3, 1, 1, 6}},
    },
    // LANCE
    {
        {"Cavalier",  	CLASS_CAVALIER,   		3000, 0x04, {ITYPE_SWORD, ITYPE_LANCE}, {20, 6, 4, 6, 3, 6, 1, 7}},
        {"Knight",      CLASS_ARMOR_KNIGHT,		2500, 0x06, {ITYPE_LANCE, -1}, 			{22, 7, 3, 2, 3, 8, 1, 4}},
        {"Pegasus Kn.", CLASS_PEGASUS_KNIGHT,	3000, 0x39, {ITYPE_LANCE, -1}, 			{19, 4, 7, 7, 3, 1, 4, 7}},
		{"Soldier", 	CLASS_SOLDIER,        	2500, 0x3F, {ITYPE_LANCE,  -1},			{22, 8, 6, 4, 3, 1, 1, 5}},
        {"Wyvern Rider",CLASS_WYVERN_RIDER,   	3500, 0x19, {ITYPE_LANCE, -1}, 			{21, 6, 4, 5, 3, 6, 0, 6}},
    },
    // AXE
    {
        {"Brigand", CLASS_BRIGAND, 2500, 0x33, {ITYPE_AXE, -1}, {22, 6, 4, 6, 3, 2, 0, 5}},
        {"Fighter", CLASS_FIGHTER, 2500, 0x31, {ITYPE_AXE, -1}, {22, 7, 5, 4, 3, 1, 0, 5}},
        {"Pirate",  CLASS_PIRATE,  2500, 0x34, {ITYPE_AXE, -1}, {22, 5, 7, 6, 3, 2, 0, 5}},
    },
    // BOW
    {
        {"Archer", CLASS_ARCHER, 2500, 0x14, {ITYPE_BOW, -1}, {22, 7, 5, 4, 3, 1, 0, 5}},
    },
    // STAFF
    {
        {"Cleric", CLASS_CLERIC, 2500, 0x3B, {ITYPE_STAFF, -1}, {19, 3, 5, 6, 3, 1, 4, 5}},
        {"Priest", CLASS_PRIEST, 2500, 0x37, {ITYPE_STAFF, -1}, {19, 3, 5, 6, 3, 1, 4, 5}},
        {"Troubadour", CLASS_TROUBADOUR, 3500, 0x3C, {ITYPE_STAFF, -1}, {19, 3, 5, 6, 3, 1, 4, 5}},
    },
    // ANIMA
    {
        {"Mage", CLASS_MAGE, 2500, 0x1E, {ITYPE_ANIMA, -1}, {18, 5, 4, 5, 0, 1, 3, 5}},
    },
    // LIGHT
    {
        {"Monk", CLASS_MONK, 2500, 0x36, {ITYPE_LIGHT, -1}, {19, 3, 5, 6, 3, 1, 4, 5}},
    },
    // DARK
    {
        {"Shaman", CLASS_SHAMAN, 3000, 0x26, {ITYPE_DARK, -1}, {20, 6, 3, 3, 0, 3, 3, 5}},
    },
};

static int barrackUnitCounts[8] = {
    4, // ITYPE_SWORD has 3 units
    5, // ITYPE_LANCE has 3 units
    3, // ITYPE_AXE has 3 units
    1, // ITYPE_BOW has 3 units
    3, // ITYPE_STAFF has 3 units
    1, // ITYPE_LIGHT has 1 unit
    1, // ITYPE_ANIMA has 1 unit
    1, // ITYPE_DARK has 1 unit
};

int Barracks_CountUnitsByWeapon(int weaponType) {
	return barrackUnitCounts[weaponType];
}


void Barracks_DrawUnitWeaponIcon(u16* BgOut, int IconIndex, int OamPalBase){
	// If unit has no entry for other weapon to use.
	if(IconIndex == -1)
	{
		return;
	}
	
	IconIndex += 112;
	if (IconIndex < 0) {
        BgOut[0]  = 0;
        BgOut[1]  = 0;
        BgOut[32] = 0;
        BgOut[33] = 0;
    } else {
        u16 Tile = GetIconTileIndex(IconIndex) + OamPalBase;

        BgOut[0]  = Tile++;
        BgOut[1]  = Tile++;
        BgOut[32] = Tile++;
        BgOut[33] = Tile;
    }
}

void Barracks_AddUnitToParty(void) {
    u8 rand100 = 10; //DivRem(AdvanceGetLCGRNValue(), 101);

    struct Unit* unit;
    short summonerNum, i;

	int index = gUnused_0203E884[1];
	int tab = gUnused_0203E884[2];
	int maxHp = barrackUnits[tab][index].startStats[0];
	int str   = barrackUnits[tab][index].startStats[1];
	int skill = barrackUnits[tab][index].startStats[2];
	int spd   = barrackUnits[tab][index].startStats[3];
	int luck  = barrackUnits[tab][index].startStats[4];
	int def   = barrackUnits[tab][index].startStats[5];
	int res   = barrackUnits[tab][index].startStats[6];
	int move  = barrackUnits[tab][index].startStats[7];



    // 3. Set up unit definition
    unit = NULL;

    // 3.1. Character/Class/Faction/Level/Position
	int unitIndex = 0x80;
	for(int i=0; i<10; i++)
	{
		unit = GetUnitFromCharId(unitIndex);
		if (unit == NULL)
		{
			break;
		}
		unitIndex++;
	}
    gUnitDef1.charIndex       = unitIndex;
    gUnitDef1.classIndex      = barrackUnits[tab][gUnused_0203E884[1]].classId;
    gUnitDef1.leaderCharIndex = CHARACTER_NONE;
    gUnitDef1.autolevel       = FALSE;

    if (UNIT_FACTION(gActiveUnit) == FACTION_BLUE)
        gUnitDef1.allegiance = 0;

    else if (UNIT_FACTION(gActiveUnit) == FACTION_RED)
        gUnitDef1.allegiance = 2;

    else if (UNIT_FACTION(gActiveUnit) == FACTION_GREEN)
        gUnitDef1.allegiance = 1;

    gUnitDef1.level = 1; //gActiveUnit->level;

    gUnitDef1.xPosition = gActionData.xOther;
    gUnitDef1.yPosition = gActionData.yOther;

    gUnitDef1.redaCount = 0;
    gUnitDef1.redas = NULL;

    gUnitDef1.genMonster = FALSE;
    gUnitDef1.itemDrop = FALSE;
	
	// Give units starting weapon and a vulnerary.
	//int wep_slot = 0;
	for (i = 0; i < 2; i++)
	{
		int weaponType = barrackUnits[tab][gUnused_0203E884[1]].weapon[i];
		if (weaponType != -1)
		{	
			switch (weaponType)
			{
				case ITYPE_SWORD:
					gUnitDef1.items[i] = ITEM_SWORD_IRON;
					break;
				
				case ITYPE_LANCE:
					gUnitDef1.items[i] = ITEM_LANCE_IRON;
					break;
					
				case ITYPE_AXE:
					gUnitDef1.items[i] = ITEM_AXE_IRON;
					break;
					
				case ITYPE_BOW:
					gUnitDef1.items[i] = ITEM_BOW_IRON;
					break;
					
				case ITYPE_STAFF:
					gUnitDef1.items[i] = ITEM_STAFF_HEAL;
					break;
				
				case ITYPE_ANIMA:
					gUnitDef1.items[i] = ITEM_ANIMA_FIRE;
					break;
					
				case ITYPE_LIGHT:
					gUnitDef1.items[i] = ITEM_LIGHT_LIGHTNING;
					break;
					
				case ITYPE_DARK:
					gUnitDef1.items[i] = ITEM_DARK_FLUX;
					break;
			}
		}
		else
		{
			gUnitDef1.items[i] = ITEM_VULNERARY;
			i = 2;
		}
	}

    // 3.3. Ai (is null)
    for (i = 0; i < 4; ++i)
	{
        gUnitDef1.ai[i] = 0;
	}

    // 4. Load unit
    //unit = GetUnitFromCharId(gSummonConfig[summonerNum][1]);

    if (unit == NULL) {
        struct BattleUnit bu = gBattleActor;
        LoadUnits(&gUnitDef1);
        gBattleActor = bu;
    }

    // 5. Set level and weapon ranks
    unit = GetUnitFromCharId(unitIndex);

    for (i = 0; i < 4; ++i)
        unit->ranks[i] = 0;
	
	// Give units the weapon skill they can use.
	for (i = 0; i < 2; i++)
	{
		int weaponType = barrackUnits[tab][gUnused_0203E884[1]].weapon[i];
		int weaponToGive = 0;
		if (weaponType != -1)
		{
			unit->ranks[weaponType] = WPN_EXP_D;
		}
	}

    unit->level = 1;
    unit->maxHP = maxHp;
    unit->curHP = unit->maxHP;
    unit->pow = str;
    unit->skl = skill;
    unit->spd = spd;
    unit->lck = luck;
    unit->def = def;
    unit->res = res;
	
    //unit->ai1 = 1;
    //unit->exp   = UNIT_EXP_DISABLED;
}

void Barracks_PutUnitSprite(int layer, int x, int y, struct Unit * unit, int index) {
		
	int tab = gUnused_0203E884[2];
    u32 id = barrackUnits[tab][index].standingMapSprite;
    int chr = UseUnitSprite(id);

    if (x < -16 || x > DISPLAY_WIDTH)
        return;

    if (y < -32 || y > DISPLAY_HEIGHT)
        return;

    switch (GetInfo(id).size) {
    case UNIT_ICON_SIZE_16x16:
        PutSprite(layer, x, y, gObject_16x16, 0xC * 0x1000 + 0x880 + chr);
        break;

    case UNIT_ICON_SIZE_16x32:
        PutSprite(layer, x, y - 16, gObject_16x32, 0xC * 0x1000 + 0x880 + chr);
        break;

    case UNIT_ICON_SIZE_32x32:
        PutSprite(layer, x - 8, y - 16, gObject_32x32, 0xC * 0x1000 + 0x880 + chr);
        break;
    }
}

void Barracks_DisplayUnits(void) {
	int index = Barracks_GetLineOffset(); // Index of Unit list.
	u8 tab = gUnused_0203E884[2];
    for (int i=0; i<Barracks_CountUnitsByWeapon(tab); i++)
    {
        ApplyUnitSpritePalettes();
        EnablePaletteSync();
        ForceSyncUnitSpriteSheet(); // Unit animations.
        Barracks_PutUnitSprite(
            2,
            56,				// x.
            70 + (16 * i),	// y.
            GetUnit(0),
            i + index				// index (for standing sprite).
        );
    }
}

void Barracks_DrawItemMenuLine(struct Text* text, int item, s8 isUsable, u16* mapOut) {
	int tab = gUnused_0203E884[2];
    Text_SetParams(text, 0, (isUsable ? TEXT_COLOR_SYSTEM_WHITE : TEXT_COLOR_SYSTEM_GRAY));
    Text_DrawString(text, barrackUnits[tab][item-1].name);
    PutText(text, mapOut + 2);
}

/***** bmshop.c *****/
void DisplayShopUiArrows(void) {
	if(gUnused_0203E884[IS_BARRACKS])
	{
		Barracks_DisplayUnits();
	}
	
	if (ShouldDisplayUpArrow())
        DisplayUiVArrow(
            120, 64,
            OBJ_PALETTE(OBJPAL_SHOP_SPINARROW) + OBJ_CHAR(OBJCHR_SHOP_SPINARROW),
            true);

    if (ShouldDisplayDownArrow())
        DisplayUiVArrow(
            120, 152,
            OBJ_PALETTE(OBJPAL_SHOP_SPINARROW) + OBJ_CHAR(OBJCHR_SHOP_SPINARROW),
            false);
}

void Shop_Loop_BuyKeyHandler(struct ProcShop * proc) {
    u8 head_loc;
    u32 cursor_at_head;
    int price;
    int a;
    int b;

    Shop_TryMoveHandPage();

    BG_SetPosition(2, 0, ShopSt_GetBg2Offset());

    head_loc = proc->head_loc;
    cursor_at_head = ShopSt_GetHeadLoc() != head_loc;

    proc->head_loc = ShopSt_GetHeadLoc();
    proc->hand_loc = ShopSt_GetHandLoc();

    proc->head_idx = proc->head_loc;
    proc->hand_idx = proc->hand_loc;

    a = proc->head_loc;
    a *= 16;

    b = ((proc->hand_loc * 16)) - 72;

    DisplayUiHand(56, a - b);

    if ((proc->helpTextActive != 0) && (cursor_at_head != 0))
    {
        a = (proc->head_loc * 16);
        b = ((proc->hand_loc * 16) - 72);
        StartItemHelpBox(56, a - b, proc->shopItems[proc->head_loc]);
    }
    DisplayShopUiArrows();

    if (IsShopPageScrolling() != 0)
        return;

    if (proc->helpTextActive != 0)
    {
        if (gKeyStatusPtr->newKeys & (B_BUTTON | R_BUTTON))
        {
            proc->helpTextActive = 0;
            CloseHelpBox();
        }
        return;
    }

    if (gKeyStatusPtr->newKeys & R_BUTTON)
    {
		struct Unit *unit;
		
		// Just get first valid unit, doesn't really matter.
		for (int i=FACTION_BLUE + 1; i<FACTION_GREEN; i++)
		{
			unit = GetUnit(i);
			if (UNIT_IS_VALID(unit))
			{
				break;
			}
		}

        proc->helpTextActive = 1;
        a = (proc->head_loc * 16);
        b = ((proc->hand_loc * 16) - 72);
        StartItemHelpBox(56, a - b, proc->shopItems[proc->head_loc]);
		proc->head_loc = 0;
		
        return;
    }

    price = GetItemPurchasePrice(proc->unit, proc->shopItems[proc->head_loc]);

    if (gKeyStatusPtr->newKeys & A_BUTTON)
    {
        if (price > (int)GetPartyGoldAmount())
        {
            StartShopDialogue(0x8B2, proc);
            // SHOP_TYPE_ARMORY: "You don't have the money![.][A]"
            // SHOP_TYPE_VENDOR: "You're short of funds.[A]"
            // SHOP_TYPE_SECRET_SHOP: "Heh! Not enough money![A]"

            Proc_Goto(proc, 1);
        }
        else
        {
            SetTalkNumber(price);
            StartShopDialogue(0x8B5, proc);
            // SHOP_TYPE_ARMORY: "How does [.][G] gold[.][NL]sound to you?[.][Yes]"
            // SHOP_TYPE_VENDOR: "That's worth [.][G] gold.[NL]Is that all right?[Yes]"
            // SHOP_TYPE_SECRET_SHOP: "That is worth [G] gold.[NL]Is that acceptable?[.][Yes]"

            Proc_Break(proc);
        }
        return;
    }

    if (gKeyStatusPtr->newKeys & B_BUTTON)
    {
        PlaySoundEffect(SONG_SE_SYS_WINDOW_CANSEL1);
        Proc_Goto(proc, PL_SHOP_SELL_NOITEM);
        return;
    }

	if(gUnused_0203E884[IS_BARRACKS])
	{
		bool refreshTab = false;
		u8 tab = gUnused_0203E884[2];
		
		// Do weapon tab swapping.
		if (gKeyStatusPtr->repeatedKeys & DPAD_RIGHT)
		{
			if(gUnused_0203E884[2] >= 7)
			{
				gUnused_0203E884[2] = 0;
			}
			else
			{
				gUnused_0203E884[2]++;
			}
            PlaySoundEffect(SONG_SE_SYS_WINDOW_SELECT1);
			refreshTab = true;
		}
			
		else if(gKeyStatusPtr->repeatedKeys & DPAD_LEFT)
		{
			if(gUnused_0203E884[2] <= 0)
			{
				gUnused_0203E884[2] = 7;
			}
			else
			{
				gUnused_0203E884[2]--;
			}
            PlaySoundEffect(SONG_SE_SYS_WINDOW_SELECT1);
			refreshTab = true;
		}
		
		if(refreshTab)
		{
			u8 tab = gUnused_0203E884[2];
			BG_Fill(gBG2TilemapBuffer, 0);
		
			// Reset the shop back, such as hand location, lines to draw, available items etc.
			ShopSt_SetHeadLocBak(0);
			gUnused_0203E884[1] = 0; // Reset custom indexer for unit selected.
			gShopState->head_loc = 0;
			gShopState->item_cnt = Barracks_CountUnitsByWeapon(tab);
			gShopState->lines = Barracks_CountUnitsByWeapon(tab);
			gShopState->hand_loc = 0;
			gShopState->px_per_line = 16;
			gShopState->trig = 4;
			gShopState->bg2_off = gShopState->hand_loc * 16;
					
			// Refreshes the text of units.
			DrawShopSoldItems(proc);
		}
	}
	
	// TODO: Is this shitty? This draws the icons at the top for the tabs.
	
	for (int i=0; i<8; i++)
	{
		if(gUnused_0203E884[2] == i)
		{
			Barracks_DrawUnitWeaponIcon(gBG0TilemapBuffer + TILEMAP_INDEX(7 + (i*2), 6), i, OAM2_PAL(1));
		}
		else
		{
			Barracks_DrawUnitWeaponIcon(gBG0TilemapBuffer + TILEMAP_INDEX(7 + (i*2), 6), i, OAM2_PAL(5));
		}
	}
	BG_EnableSyncByMask(BG0_SYNC_BIT);
}

void Shop_TryAddItemToInventory(struct ProcShop * proc) {
	if (gUnused_0203E884[IS_BARRACKS])
	{
		HandleShopBuyAction(proc);
		Proc_Goto(proc, PL_SHOP_BUY_DONE);
		Barracks_AddUnitToParty();
		return;
	}
	
    if (proc->unitItemCount >= UNIT_ITEM_COUNT)
    {
        if (HasConvoyAccess())
        {
            StartShopDialogue(0x8BE, proc);
            // SHOP_TYPE_ARMORY: "Looks like you're full.[.][A][NL2][NL]Send it to storage?[.][NL][Yes]"
            // SHOP_TYPE_VENDOR: "Your hands are full.[A][NL2][NL]Send it to storage?[.][NL][Yes]"
            // SHOP_TYPE_SECRET_SHOP: "Your hands are full.[A][NL2][NL]Send it to storage?[.][NL][Yes]"
        }
        else
        {
            StartShopDialogue(0x8C1, proc);
            // SHOP_TYPE_ARMORY: "Looks like you're full.[.][A]"
            // SHOP_TYPE_VENDOR: "Your hands are full.[A]"
            // SHOP_TYPE_SECRET_SHOP: "Your hands are full.[A]"

            Proc_Goto(proc, PL_SHOP_BUY_FULL_NO_INEVNTORY);
        }
        return;
    }

    UnitAddItem(proc->unit, proc->shopItems[proc->head_loc]);
    HandleShopBuyAction(proc);

    Proc_Goto(proc, PL_SHOP_BUY_DONE);
}

void GetStringTextBox(const char* _str, int *out_width, int *out_height) {
	if(gUnused_0203E884[IS_BARRACKS])
	{
		*out_width = 16 * 2;
		*out_height = 16 * 3;
		return;
	}
	
    char *str;

    *out_width = 0;
    *out_height = 0;

    str = StringInsertSpecialPrefixByCtrl();
    while (*str != 0 && *str != CHAR_NEWLINE) {
        int width = GetStringTextLen(str);

        if (*out_width < width)
            *out_width = width;
        *out_height += 16;

        str = GetStringLineEnd(str);
        if (*str == 0)
            break;
        str++;
    }
}

/***** helpbox.c *****/
int DrawHelpBoxWeaponLabels(int item) {
    /*
    Text_InsertDrawString(&gHelpBoxSt.text[0], 0, 8, GetWeaponTypeDisplayString(GetItemType(item)));
    Text_InsertDrawString(&gHelpBoxSt.text[0], 47, 8, GetStringFromIndex(0x500)); // TODO: msg id "Rng[.]"
    Text_InsertDrawString(&gHelpBoxSt.text[0], 97, 8, GetStringFromIndex(0x502)); // TODO: msg id "Wt"

    Text_InsertDrawString(&gHelpBoxSt.text[1], 0, 8, GetStringFromIndex(0x503)); // TODO: msg id "Mt"
    Text_InsertDrawString(&gHelpBoxSt.text[1], 47, 8, GetStringFromIndex(0x4F4)); // TODO: msg id "Hit[.]}"
    Text_InsertDrawString(&gHelpBoxSt.text[1], 97, 8, GetStringFromIndex(0x501)); // TODO: msg id "Crit"
    */

    // Recruit
   // const char* recruitStats[6] = {"Str", "Skill", "Spd", "Luck", "Def", "Res"};
   
   
   
    Text_InsertDrawString(&gHelpBoxSt.text[0], 0, 8, GetStringFromIndex(0x4E9)); // HP
    Text_InsertDrawString(&gHelpBoxSt.text[0], 97, 8, GetStringFromIndex(0x4F6)); // Move

    Text_InsertDrawString(&gHelpBoxSt.text[1], 0, 8, GetStringFromIndex(0x4EB)); // Str/Mag
    Text_InsertDrawString(&gHelpBoxSt.text[1], 47, 8, GetStringFromIndex(0x4EC)); // Skill
    Text_InsertDrawString(&gHelpBoxSt.text[1], 97, 8, GetStringFromIndex(0x4ED)); // Spd

    Text_InsertDrawString(&gHelpBoxSt.text[2], 0, 8, GetStringFromIndex(0x4EE)); // Luck
    Text_InsertDrawString(&gHelpBoxSt.text[2], 47, 8, GetStringFromIndex(0x4EF)); // Def
    Text_InsertDrawString(&gHelpBoxSt.text[2], 97, 8, GetStringFromIndex(0x4F0)); // Res
   

    return 2;
}



void DrawHelpBoxWeaponStats(int item) {
	
	// TODO: MAKE GLOBAL
	int index = item-1;
	int tab = gUnused_0203E884[2];
	int maxHp = barrackUnits[tab][index].startStats[0];
	int str   = barrackUnits[tab][index].startStats[1];
	int skill = barrackUnits[tab][index].startStats[2];
	int spd   = barrackUnits[tab][index].startStats[3];
	int luck  = barrackUnits[tab][index].startStats[4];
	int def   = barrackUnits[tab][index].startStats[5];
	int res   = barrackUnits[tab][index].startStats[6];
	int move  = barrackUnits[tab][index].startStats[7];
	
    // Recruit
    Text_InsertDrawNumberOrBlank(&gHelpBoxSt.text[0], 32 + 2, 7, maxHp); // HP
    Text_InsertDrawNumberOrBlank(&gHelpBoxSt.text[0], 129 - 6, 7, move); // Mov

    Text_InsertDrawNumberOrBlank(&gHelpBoxSt.text[1], 32 - 5, 7, str); // Str
    Text_InsertDrawNumberOrBlank(&gHelpBoxSt.text[1], 67 + 5, 7, skill); // Skill
    Text_InsertDrawNumberOrBlank(&gHelpBoxSt.text[1], 129 - 6, 7, spd); // Speed

    Text_InsertDrawNumberOrBlank(&gHelpBoxSt.text[2], 32 - 5, 7, luck); // Luck
    Text_InsertDrawNumberOrBlank(&gHelpBoxSt.text[2], 67 + 6, 7, def); // Def
    Text_InsertDrawNumberOrBlank(&gHelpBoxSt.text[2], 129 - 6, 7, res); // Res


}

// Hardcode prices for units.
inline int GetItemCost(int item) {
	
	if(gUnused_0203E884[IS_BARRACKS])
	{
		
		// Hack, don't support more than 20 due to array size limit.
		if(item > 20)
		{
			return 10000;
		}
		
		int tab = gUnused_0203E884[2];
		return barrackUnits[tab][item-1].price/2;
	}
	
    if (GetItemAttributes(item) & IA_UNBREAKABLE)
        return GetItemData(ITEM_INDEX(item))->costPerUse;
    else
        return GetItemData(ITEM_INDEX(item))->costPerUse * GetItemUses(item);
}

void StartShopScreen(struct Unit * unit, const u16 * inventory, u8 shopType, ProcPtr parent) {
    struct ProcShop * proc;
    const u16 * shopItems;
    int i;

    EndPlayerPhaseSideWindows();

    if (parent)
        proc = Proc_StartBlocking(gProcScr_Shop, parent);
    else
        proc = Proc_Start(gProcScr_Shop, PROC_TREE_3);

    proc->shopType = shopType;
    proc->unit = unit;

    shopItems = gDefaultShopInventory;
    if (inventory != 0)
        shopItems = inventory;

	if(gUnused_0203E884[IS_BARRACKS])
	{
		// Reset Barracks variables (except for IS_BARRACKS state).
		for (int i=1; i<10; i++)
		{
			gUnused_0203E884[i] = 0;
		}
		
		u8 tab = gUnused_0203E884[2];
		for (i = 0; i < Barracks_CountUnitsByWeapon(tab); i++)
		{
			proc->shopItems[i] = 1+i; // Hardcode entry 1->(amount of units that can use that weapon). At start, this will always be Sword.
		}
	}
	else
	{
		for (i = 0; i <= SHOP_ITEMS_MAX_AMT; i++)
			proc->shopItems[i] = MakeNewItem(*shopItems++);
	}

    UpdateShopItemCounts(proc);
}

/***** bmitem.c *****/
void DrawItemMenuLine(struct Text* text, int item, s8 isUsable, u16* mapOut) {
	
	// Draw Unit sprites and icons for weapons they can use.
	if (gUnused_0203E884[IS_BARRACKS])
	{
		Barracks_DrawItemMenuLine(text, item, true, mapOut);
			
		int tab = gUnused_0203E884[2];
		for(int i=0; i<2; i++)
		{
			Barracks_DrawUnitWeaponIcon(mapOut + 9 + 2 * i, barrackUnits[tab][item-1].weapon[i], OAM2_PAL(5)); // Draw weapons unit can use.
		}

		return;
	}
	
    Text_SetParams(text, 0, (isUsable ? TEXT_COLOR_SYSTEM_WHITE : TEXT_COLOR_SYSTEM_GRAY));
    Text_DrawString(text, GetItemName(item));

    PutText(text, mapOut + 2);

    PutNumberOrBlank(mapOut + 11, isUsable ? TEXT_COLOR_SYSTEM_BLUE : TEXT_COLOR_SYSTEM_GRAY, GetItemUses(item));

    DrawIcon(mapOut, GetItemIconId(item), 0x4000);
}

void DrawShopSoldItems(struct ProcShop * proc) {
    int item;
    int index;
    int i;

    SetTextFont(0);
    InitSystemTextFont();
	if(gUnused_0203E884[IS_BARRACKS])
	{
		
		// Refresh the items drawn on screen as different weapon tabs may have different number of units to buy.
		u8 tab = gUnused_0203E884[2];
		for (int i = 0; i < Barracks_CountUnitsByWeapon(tab); i++)
		{
			//if (i < 14 + 999)
			{
				proc->shopItems[i] = 1+i; //i+1;//MakeNewItem(*shopItems++);
			}
			//else
			{
				//proc->shopItems[i] = MakeNewItem(*shopItems++);
			}
		}
		
	
		//for (i = proc->hand_idx; i < proc->hand_idx + SHOP_TEXT_LINES; i++)
		for (i = 0; i < 20; i++)
		{
			index = DivRem(i, Barracks_CountUnitsByWeapon(tab) + 1);
			ClearText(&gShopItemTexts[index]);
		}

		for (i = 0; i < Barracks_CountUnitsByWeapon(tab); i++)
		{
			index = DivRem(i, Barracks_CountUnitsByWeapon(tab) + 1);
			item = proc->shopItems[i];

			if (item == 0)
				break;

			DrawShopItemPriceLine(
				&gShopItemTexts[index],
				item,
				proc->unit,
				gBG2TilemapBuffer + TILEMAP_INDEX(7, ((i * 2) & 0x1F))
			);
		}
	}
	else
	{
		for (i = proc->hand_idx; i < proc->hand_idx + SHOP_TEXT_LINES; i++)
		{
			index = DivRem(i, SHOP_TEXT_LINES + 1);
			ClearText(&gShopItemTexts[index]);
		}

		for (i = proc->hand_idx; i < proc->hand_idx + SHOP_TEXT_LINES; i++)
		{
			index = DivRem(i, SHOP_TEXT_LINES + 1);
			item = proc->shopItems[i];

			if (item == 0)
				break;

			DrawShopItemPriceLine(
				&gShopItemTexts[index],
				item,
				proc->unit,
				gBG2TilemapBuffer + TILEMAP_INDEX(7, ((i * 2) & 0x1F))
			);
		}
	}

    
    BG_SetPosition(BG_2, 0, (proc->hand_idx * 0x10) - 0x48);
    BG_EnableSyncByMask(BG2_SYNC_BIT);
}

/***** Multiplayer Fixes *****/
void SwitchPhases(void) {
    switch (gPlaySt.faction) {
    case FACTION_BLUE:
        gPlaySt.faction = FACTION_RED;
		
		// Handle swapping between the internal teams on FACTION_BLUE.
		unused_0203e760 = !unused_0203e760;
		if(unused_0203e760)
		{
			gPlaySt.config.windowColor = 2;
			gPlaySt.faction = FACTION_BLUE;
		}
		else
		{
			gPlaySt.config.windowColor = 0;
		}
		ReadGameSaveCoreGfx();
        break;

    case FACTION_RED:
        gPlaySt.faction = FACTION_GREEN;

        break;

    case FACTION_GREEN:
        gPlaySt.faction = FACTION_BLUE;

        if (gPlaySt.chapterTurnNumber < 999)
            gPlaySt.chapterTurnNumber++;

        ProcessTurnSupportExp();
    }
}

void PhaseIntro_InitGraphics(ProcPtr proc) {
    Decompress(Img_PhaseChangeUnk, BG_CHR_ADDR(0xA00));
    Decompress(Img_PhaseChangeSquares, BG_CHR_ADDR(BGCHR_PHASE_CHANGE_SQUARES));

    BG_SetPosition(0, 0, 0);
    BG_SetPosition(1, 0, 0);
    BG_SetPosition(2, 0, 0);
	


    switch (gPlaySt.faction)
    {
    case FACTION_BLUE:
		// Player 2 (Green - PLAYER_PHASE).
		if (unused_0203e760)
		{
			Decompress(Img_PhaseChangePlayer, BG_CHR_ADDR(BGCHR_PHASE_CHANGE_NAME));
			ApplyPalette(Pal_PhaseChangeOther, BGPAL_PHASE_CHANGE);
			ApplyPalette(Pal_PhaseChangeOther, 18);
		}
		// Default (Blue - PLAYER_PHASE).
		else
		{
			Decompress(Img_PhaseChangePlayer, BG_CHR_ADDR(BGCHR_PHASE_CHANGE_NAME));
			ApplyPalette(Pal_PhaseChangePlayer, BGPAL_PHASE_CHANGE);
			ApplyPalette(Pal_PhaseChangePlayer, 18);
		}
		break;

    case FACTION_RED:
        Decompress(Img_PhaseChangeEnemy, BG_CHR_ADDR(BGCHR_PHASE_CHANGE_NAME));
        ApplyPalette(Pal_PhaseChangeEnemy, BGPAL_PHASE_CHANGE);
        ApplyPalette(Pal_PhaseChangeEnemy, 18);
        break;

    case FACTION_GREEN:
        Decompress(Img_PhaseChangeOther, BG_CHR_ADDR(BGCHR_PHASE_CHANGE_NAME));
        ApplyPalette(Pal_PhaseChangeOther, BGPAL_PHASE_CHANGE);
        ApplyPalette(Pal_PhaseChangeOther, 18);
        break;
    }
}

int GetUnitSpritePalette(const struct Unit * unit)
{
    switch (UNIT_FACTION(unit)) {
     case FACTION_BLUE:
	 // Player 2 - Force Green Palette.
		if (unit->ai1 == 1)
			return 0xE; // Green
		return 0xC; // Blue

    case FACTION_RED:
        return 0xD; // Red

    case FACTION_GREEN:
        return 0xB; // Purple

    case FACTION_PURPLE:
        return 0xB;
    }
}

void ApplyUnitSpritePalettes(void)
{
    ApplyPalettes(gPal_MapSprite, 0x1C, 4);

	ApplyPalette(gPal_MapSpriteArena, 0x10 + OBJPAL_UNITSPRITE_PURPLE);
}

int GetPlayerSelectKind(struct Unit * unit) {
    u8 faction = gPlaySt.faction;

    if (!unit)
    {
        return PLAYER_SELECT_NOUNIT;
    }

    if (gBmSt.gameStateBits & BM_FLAG_PREPSCREEN)
    {
        if (!CanCharacterBePrepMoved(unit->pCharacterData->number))
        {
            return PLAYER_SELECT_4;
        }

        faction = FACTION_BLUE;
    }

    if (!unit)
    {
        return PLAYER_SELECT_NOUNIT;
    }

    if (UNIT_FACTION(unit) != faction)
    {
        return PLAYER_SELECT_NOCONTROL;
    }

    if (unit->state & US_UNSELECTABLE)
    {
        return PLAYER_SELECT_TURNENDED;
    }

    if (UNIT_CATTRIBUTES(unit) & CA_UNSELECTABLE)
    {
        return PLAYER_SELECT_TURNENDED;
    }

    if ((unit->statusIndex != UNIT_STATUS_SLEEP) && (unit->statusIndex != UNIT_STATUS_BERSERK))
    {
		// Only allow Units to be controllable for the active team (P1, P2 etc.).
		if(unit->ai1 == unused_0203e760)
		{
			return PLAYER_SELECT_CONTROL;
		}
    }

    return PLAYER_SELECT_NOCONTROL;
}

// Disable Barracks flag.
void Shop_ExitShopDialogue(struct ProcShop * proc)
{
	gUnused_0203E884[IS_BARRACKS] = false;
    if (proc->unit == 0) {
        StartShopDialogue(0x898, proc);
        // SHOP_TYPE_ARMORY: "Come back again.[A]"
        // SHOP_TYPE_VENDOR: "Hope to see you again![A]"
        // SHOP_TYPE_SECRET_SHOP: "[N/A]"
    } else {
        StartShopDialogue(0x8B8, proc);
        // SHOP_TYPE_ARMORY: "Come back again.[A]"
        // SHOP_TYPE_VENDOR: "Drop in again.[A]"
        // SHOP_TYPE_SECRET_SHOP: "Hee hee... Buh-bye![.][A]"
    }
}

// (Full Detour) World Map - Change Guide -> Barracks and activate Barracks flag.
s8 WorldMap_CallGuide(ProcPtr proc) {
    Proc_BlockEachMarked(PROC_MARK_WMSTUFF);
	struct Unit *unit;
	
	// Just get first valid unit, doesn't really matter.
	for (int i=FACTION_BLUE + 1; i<FACTION_GREEN; i++)
	{
		unit = GetUnit(i);
		if (UNIT_IS_VALID(unit))
		{
			break;
		}
	}
	
	if (unit != NULL)
	{
		gUnused_0203E884[IS_BARRACKS] = true;
		StartShopScreen(unit, NULL, SHOP_TYPE_ARMORY, proc);
	}
	
    return 0;
}

int WMMenu_OnGuideDraw(struct MenuProc * menuProc, struct MenuItemProc * menuItemProc) {
    if (!(menuProc->state & MENU_STATE_NOTSHOWN))
    {
        if (!BmGuideTextShowGreenOrNormal())
        {
            Text_SetColor(&menuItemProc->text, TEXT_COLOR_SYSTEM_GREEN);
        }

        if (menuItemProc->availability == MENU_DISABLED)
        {
            Text_SetColor(&menuItemProc->text, TEXT_COLOR_SYSTEM_GRAY);
        }
		
		//if(gUnused_0203E884[0])
		{
			Text_DrawString(&menuItemProc->text, "Barracks");
		}
		//else
		{
		//	Text_DrawString(&menuItemProc->text, GetStringFromIndex(menuItemProc->def->nameMsgId));
		}

        PutText(
            &menuItemProc->text,
            BG_GetMapBuffer(menuProc->frontBg) + TILEMAP_INDEX(menuItemProc->xTile, menuItemProc->yTile)
        );
    }

    // return 0; // BUG?
}

//! FE8U = 0x080917D8
extern u8 gUnknown_0200F158;
extern struct SortedUnitEnt * gSortedUnits[];
void sub_809144C(struct UnitListScreenProc * proc) {
    if ((gKeyStatusPtr->newKeys & SELECT_BUTTON) != 0)
    {
		gSortedUnits[proc->unk_30]->unit->ai1 = 1;
        return;
    }
	
    if ((gKeyStatusPtr->heldKeys & L_BUTTON) != 0)
    {
        proc->unk_31 = 2;
    }
    else
    {
        proc->unk_31 = 1;
    }

    if ((gKeyStatusPtr->newKeys & R_BUTTON) != 0)
    {
        Proc_Goto(proc, 3);
        return;
    }

    if ((gKeyStatusPtr->newKeys & A_BUTTON) != 0)
    {
        switch (proc->mode)
        {
            case UNITLIST_MODE_PREPMENU:
                UnitList_TogglePrepDeployState(proc);

                break;

            case UNITLIST_MODE_SOLOANIM:
                UnitList_ToggleSoloAnimState(gSortedUnits[proc->unk_30]->unit, 1);
                UnitList_PutRow(proc, proc->unk_30, gBG0TilemapBuffer, proc->page, 0);

                break;

            case UNITLIST_MODE_FIELD:
                SetLastStatScreenUid(gSortedUnits[proc->unk_30]->unit->index);
                PlaySoundEffect(SONG_SE_SYS_WINDOW_SELECT1);
                Proc_Break(proc);

                break;

            default:
                break;
        }

        return;
    }

    if ((gKeyStatusPtr->repeatedKeys & DPAD_LEFT) != 0)
    {
        if (proc->mode == UNITLIST_MODE_SOLOANIM)
        {
            if ((gKeyStatusPtr->newKeys & DPAD_LEFT) == 0)
                return;

            UnitList_ToggleSoloAnimState(gSortedUnits[proc->unk_30]->unit, -1);
            UnitList_PutRow(proc, proc->unk_30, gBG0TilemapBuffer, proc->page, 0);

            return;
        }

        if (proc->page < 2)
            return;

        proc->pageTarget--;
        Proc_Goto(proc, 2);
        proc->unk_2d = 0;
        PlaySoundEffect(SONG_6F);

        return;
    }

    if ((gKeyStatusPtr->repeatedKeys & DPAD_RIGHT) != 0)
    {
        if (proc->mode == UNITLIST_MODE_SOLOANIM)
        {
            if ((gKeyStatusPtr->newKeys & DPAD_RIGHT) == 0)
                return;

            UnitList_ToggleSoloAnimState(gSortedUnits[proc->unk_30]->unit, +1);
            UnitList_PutRow(proc, proc->unk_30, gBG0TilemapBuffer, proc->page, 0);

            return;
        }

        if (proc->page < proc->unk_2e)
        {
            proc->pageTarget++;
            proc->unk_2d = 0;
            PlaySoundEffect(SONG_6F);
            Proc_Goto(proc, 2);
        }

        return;
    }

    if ((gKeyStatusPtr->repeatedKeys & DPAD_UP) != 0 ||
        ((gKeyStatusPtr->heldKeys & L_BUTTON) != 0 && (gKeyStatusPtr->newKeys2 & DPAD_UP) != 0))
    {
        if (proc->unk_30 == 0)
        {
            if ((gKeyStatusPtr->newKeys & DPAD_UP) == 0)
                return;

            PlaySoundEffect(SONG_SE_SYS_CURSOR_UD1);
            proc->unk_29 = 3;
            return;
        }

        proc->unk_30--;
        PlaySoundEffect(SONG_SE_SYS_CURSOR_UD1);

        if (proc->unk_2c < 2)
        {
            if (proc->unk_3e / 16 != 0)
            {
                if (proc->unk_2c == 0)
                {
                    proc->unk_30++;
                    proc->unk_2c = 1;
                }

                UnitList_PutRow(proc, proc->unk_3e / 16 - 1, gBG0TilemapBuffer, proc->page, 1);
                proc->unk_29 = 2;
                proc->unk_3e = -(proc->unk_31 * 4) + proc->unk_3e;
                BG_SetPosition(0, 0, (proc->unk_3e - 0x38) & 0xFF);

                if (proc->unk_2c == 0)
                {
                    proc->unk_2c++;
                }

                return;
            }
        }

        proc->unk_2c--;

        return;
    }

    if ((gKeyStatusPtr->repeatedKeys & DPAD_DOWN) != 0 ||
        ((gKeyStatusPtr->heldKeys & L_BUTTON) != 0 && (gKeyStatusPtr->newKeys2 & DPAD_DOWN) != 0))
    {
        if (proc->unk_30 < gUnknown_0200F158 - 1)
        {
            proc->unk_30++;
            PlaySoundEffect(SONG_SE_SYS_CURSOR_UD1);

            if (proc->unk_2c == 4 && proc->unk_30 != gUnknown_0200F158 - 1)
            {
                UnitList_PutRow(proc, 6 + proc->unk_3e / 16, gBG0TilemapBuffer, proc->page, 1);
                proc->unk_29 = 1;
                proc->unk_3e = proc->unk_3e + proc->unk_31 * 4;
                BG_SetPosition(0, 0, (proc->unk_3e - 0x38) & 0xFF);
                return;
            }

            proc->unk_2c++;
        }
    }

    return;
}

// World Map (List Handler) - Mainly for toggling units between teams at the World Map.
void sub_8099120(struct PrepItemScreenProc* proc) {
    int tmp = proc->unk_34;

    if (!(tmp & 0xf)) {

        if (gKeyStatusPtr->newKeys & SELECT_BUTTON) {
            GetUnitFromPrepList(proc->hoverUnitIdx)->ai1 = !GetUnitFromPrepList(proc->hoverUnitIdx)->ai1;
            PlaySoundEffect(SONG_SE_SHATTER_STONE);
            return;
        }
		
        if (gKeyStatusPtr->newKeys & R_BUTTON) {
            Proc_Break(proc);
            return;
        }

        if (gKeyStatusPtr->newKeys & A_BUTTON) {
            if (gGMData.state.bits.state_0) {
                switch (GetGMapBaseMenuKind()) {
                    case 3:
                        proc->selectedUnitIdx = proc->hoverUnitIdx;

                        if (((proc->hoverUnitIdx % 3) <= 1) && (proc->hoverUnitIdx < PrepGetUnitAmount() - 1)) {
                            proc->hoverUnitIdx++;
                        } else {
                            proc->hoverUnitIdx--;
                        }

                        break;

                    case 2:
                        if (UnitHasItem(GetUnitFromPrepList(proc->hoverUnitIdx), ITEM_MEMBERCARD)) {
                            PlaySoundEffect(SONG_SE_SYS_WINDOW_SELECT1);
                            Proc_Goto(proc, 13);
                            return;
                        }

                        PlaySoundEffect(SONG_6C);

                        return;

                    default:
                        Proc_Goto(proc, 0xd);
                        PlaySoundEffect(SONG_SE_SYS_WINDOW_SELECT1);
                        return;
                }
            } else {
                proc->selectedUnitIdx = proc->hoverUnitIdx;

                if (((proc->hoverUnitIdx % 3) < 2) && (proc->hoverUnitIdx < PrepGetUnitAmount() - 1)) {
                    proc->hoverUnitIdx++;
                } else {
                    proc->hoverUnitIdx--;
                }
            }

            SetUiCursorHandConfig(
                0,
                ((proc->selectedUnitIdx % 3) * 64) + 24,
                (proc->selectedUnitIdx / 3) * 16 + 4 - proc->unk_34,
                2
            );

            Proc_Goto(proc, 2);
            PlaySoundEffect(SONG_SE_SYS_WINDOW_SELECT1);
            return;
        }

        if (gKeyStatusPtr->newKeys & B_BUTTON) {
            if (gGMData.state.bits.state_0) {
                proc->unk_29 = 1;
            }

            Proc_Goto(proc, 13);
            PlaySoundEffect(SONG_SE_SYS_WINDOW_CANSEL1);
            return;
        }

        if (PrepItemScreen_DpadKeyHandler(proc) != 0) {
            UpdatePrepItemScreenFace(0, GetUnitFromPrepList(proc->hoverUnitIdx), 60, 76, 0x0503);
            sub_8099F7C(&gPrepItemTexts[15], TILEMAP_LOCATED(gBG0TilemapBuffer, 2, 9), GetUnitFromPrepList(proc->hoverUnitIdx), 2);
            BG_EnableSyncByMask(1);
        }
    }

    sub_8098FAC(proc);

    return;
}

int ShopTryMoveHand(int pos, int pre, bool hscroll_en) {
    int previous;

    if (pos < 0)
        pos = 0;

    if (pos >= pre)
        pos = pre - 1;

    previous = pos;

    if (gKeyStatusPtr->repeatedKeys & DPAD_UP)
    {
        if (pos == 0)
        {
            if (hscroll_en && (gKeyStatusPtr->newKeys & DPAD_UP))
                pos = pre - 1;
        }
        else
        {
            pos--;

			if(gUnused_0203E884[IS_BARRACKS])
			{
				gUnused_0203E884[1]--;
				if(gUnused_0203E884[1] <= 0)
				{
					gUnused_0203E884[1] = 0;
				}
			}
        }
    }
    else if (gKeyStatusPtr->repeatedKeys & DPAD_DOWN)
    {
        if (pos == (pre - 1))
        {
            if (hscroll_en && (gKeyStatusPtr->newKeys & DPAD_DOWN))
                pos = 0;
        }
        else
            pos++;

			if(gUnused_0203E884[IS_BARRACKS])
			{
				gUnused_0203E884[1]++;
				if(gUnused_0203E884[1] >= gShopState->lines)
				{
					gUnused_0203E884[1] = gShopState->lines;
				}
			}
    }

    if (previous != pos)
    {
        PlaySoundEffect(SONG_SE_SYS_CURSOR_UD1);
    }
    return pos;
}

int GetPhaseAbleUnitCount(int faction) {
    int count = 0;
    int id;
    for (id = faction + 1; id < faction + 0x40; id++) {
        struct Unit *unit = GetUnit(id);
        if (UNIT_IS_VALID(unit)) {
            u32 state = unit->state;
            u32 notAble = (
                US_UNSELECTABLE
                | US_DEAD
                | US_NOT_DEPLOYED
                | US_RESCUED
                | US_UNDER_A_ROOF
                | US_BIT16);
            if (!(state & notAble)) {
                if (unit->statusIndex != UNIT_STATUS_SLEEP
                    && unit->statusIndex != UNIT_STATUS_BERSERK)
                {
                    if (!(UNIT_CATTRIBUTES(unit) & CA_UNSELECTABLE)) {
						if(unit->ai1 == unused_0203e760)
						{
							
							count += 1;
						}
                    }
                }
            }
        }
    }
    return count;
}

// Disable portraits for generic units.
void UpdatePrepItemScreenFace(int slot, struct Unit* unit, u16 x, u16 y, u16 disp) {
    struct PrepItemScreenProc* proc = Proc_Find(ProcScr_PrepItemScreen);

    if (proc->pUnits[slot] != unit)
	{
        if (proc->pUnits[slot] != NULL) {
            EndFaceById(slot);
        }

        if (unit != NULL)
		{
				//StartFace2(slot, GetUnitPortraitId(unit), (s16)x, (s16)y, disp); //MultiplayerTag - Just disable faces (TODO: Load up generic unit portrait).
		}
    }
	else {
        if (unit != NULL) {
            SetFacePosition(slot, (s16)x, (s16)y);
            SetFaceDisplayBitsById(slot, disp);
        }
    }

    proc->pUnits[slot] = unit;

    proc->xFacePosBySlot[slot] = x;
    proc->yFacePosBySlot[slot] = y;
    proc->faceDispBySlot[slot] = disp;

    return;
}