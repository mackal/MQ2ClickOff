// MQ2ClickOff.cpp : Clicks off user defined list of buffs
// Most people just use the client block buff feature so MQ2BuffTool is mostly useless now
// especially with the newer MQ2 command /removebuff Spell Name
// The one area it is still useful is removing a set of spells (ex. illusions, invis)
// This should work with the buff window closed

#include "../MQ2Plugin.h"
#include <vector>
#include <algorithm>
PreSetup("MQ2ClickOff");

void ClickOffCommand(PSPAWNINFO pChar, char szLine[]);
void LoadINISection(std::vector<int> &list, char szSection[]);
void DoClickOff(char szSet[]);
void PrintHelp();

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
    DebugSpewAlways("Initializing MQ2ClickOff");

    //Add commands, MQ2Data items, hooks, etc.
    AddCommand("/clickoff", ClickOffCommand);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2ClickOff");

    //Remove commands, MQ2Data items, hooks, etc.
    RemoveCommand("/clickoff");
}

void ClickOffCommand(PSPAWNINFO pChar, char szLine[])
{
    char szParm[MAX_STRING];
    GetArg(szParm, szLine, 1);
    if (szParm[0] == '\0' || !strnicmp(szParm, "/help", 5)) // empty parms or requesting help
        PrintHelp();
    else
        DoClickOff(szParm);
}

void PrintHelp()
{
    WriteChatf("MQ2ClickOff help stub");
    return;
}

void DoClickOff(char szSet[])
{
    PCHARINFO2 pChar2 = GetCharInfo2();
    if (!pChar2)
        return;
    std::vector<int> spell_ids;
    LoadINISection(spell_ids, szSet);
    if (spell_ids.empty()) // loaded nothing!
        return;

    std::vector<int>::iterator it;
    for (int iBuff = 0; iBuff < NUM_LONG_BUFFS; iBuff++) {
        if (pChar2->Buff[iBuff].SpellID == 0 || pChar2->Buff[iBuff].SpellID == -1)
            continue;
        it = std::find(spell_ids.begin(), spell_ids.end(), pChar2->Buff[iBuff].SpellID);
        if (it != spell_ids.end())
            pPCData->RemoveMyAffect(iBuff);
    }

    for (int iBuff = 0; iBuff < NUM_SHORT_BUFFS; iBuff++) {
        if (pChar2->ShortBuff[iBuff].SpellID == 0 || pChar2->ShortBuff[iBuff].SpellID == -1)
            continue;
        it = std::find(spell_ids.begin(), spell_ids.end(), pChar2->ShortBuff[iBuff].SpellID);
        if (it != spell_ids.end())
            pPCData->RemoveMyAffect(iBuff + NUM_LONG_BUFFS);
    }
}

void LoadINISection(std::vector<int> &list, char szSection[])
{
    char szKey[12];
    int counter = 0;
    int spell_id = -1;
    while (true) {
        sprintf(szKey, "Buff%d", ++counter);
        spell_id = GetPrivateProfileInt(szSection, szKey, -1, INIFileName);
        if (spell_id == -1)
            break;
        list.push_back(spell_id);
    }
}
