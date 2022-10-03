
#include <a_samp>
#include <streamer>
#include <strlib>
#include <YSI_Data/y_iterate>

#include "terminal.inc"

main()
{
	CreateTerminal("tower", "root", "5789", -1768.05, 800.840209, 160.05, 0.000000, 0.000000, 90.0, 31, 0, .logged = false);
}

/*
public OnPlayerEnterTerminal(playerid, termid)
{
	ShowInfoBox(playerid, "Premi ~k~~VEHICLE_ENTER_EXIT~ per usare il terminale", -1);
}

public OnPlayerExitTerminal(playerid, termid)
{
	HideInfoBox(playerid);
}

public OnPlayerShowTerminal(playerid, termid)
{
	HideInfoBox(playerid);
}

public OnPlayerHideTerminal(playerid, termid)
{
	if(IsPlayerNearTerminal(playerid))
		ShowInfoBox(playerid, "Premi ~k~~VEHICLE_ENTER_EXIT~ per usare il terminale", -1);
}
*/
