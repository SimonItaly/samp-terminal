
#include <a_samp>
#include <streamer>
#include <strlib>

#include "terminal.inc"

main()
{
	CreateTerminal("tower", "root", "5789", -1768.05, 800.840209, 160.05, 0.000000, 0.000000, 90.0, 31, 0, .logged = false);
}

/*
hook OnPlayerEnterTerminal(playerid, termid)
{
	ShowInfoBox(playerid, "Premi ~k~~VEHICLE_ENTER_EXIT~ per usare il terminale", -1);
}

hook OnPlayerExitTerminal(playerid, termid)
{
	HideInfoBox(playerid);
}

hook OnPlayerShowTerminal(playerid, termid)
{
	HideInfoBox(playerid);
}

hook OnPlayerHideTerminal(playerid, termid)
{
	if(IsPlayerNearTerminal(playerid))
		ShowInfoBox(playerid, "Premi ~k~~VEHICLE_ENTER_EXIT~ per usare il terminale", -1);
}
*/
