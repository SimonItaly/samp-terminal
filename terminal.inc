
/*
 *    A SA:MP Terminal Emulator
 *	  (c) Simon, December 2014
 *
 *	  The contents of this file are subject to the Mozilla Public License
 *	  Version 1.1 (the "License"); you may not use this file except in
 *	  compliance with the License. You may obtain a copy of the License at
 *	  http://www.mozilla.org/MPL/
 *
 *	  Software distributed under the License is distributed on an "AS IS"
 *	  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *	  License for the specific language governing rights and limitations
 *	  under the License.
 *
 *    The Original Code is the SA:MP Terminal Emulator include.
 *
 */

//------------------------------------------------------------------------------

#if defined _terminal_included
	#endinput
#endif
#define _terminal_included

#if !defined STREAMER_TYPE_OBJECT
	#error terminal.inc requires Incognito's Streamer plugin
#endif

#if !defined STRLIB_INC
	#error terminal.inc requires Slice's strlib include
#endif

#if !defined _Y_ITERATE_LOCAL_VERSION
	#error terminal.inc requires Y_Less YSI include
#endif

//------------------------------------------------------------------------------

#define MAX_TERMINALS                           10
#define MAX_TERM_LINES                          17

#define MAX_TERM_USERNAME                       10
#define MAX_TERM_HOSTNAME                       10

//Max characters is ~32, consider min 64 for colour codes
#define MAX_TERM_LINE_LEN                       64

#define MAX_TERM_ATTEMPTS                       3

#define TERMINAL_LOCKDOWN_TIME                  60

#define KEY_TERMINAL                            KEY_SECONDARY_ATTACK

#define UNIX:%1(%2)          	 \
			forward unix_%1(%2); \
			public unix_%1(%2)

#if !defined strcpy
	#define strcpy(%0,%1,%2) \
			strcat((%0[0] = '\0', %0), %1, %2)
#endif

//------------------------------------------------------------------------------

//Short day names
static const short_weekDays[7][11] =
{
	"sab",
	"dom",
	"lun",
	"mar",
	"mer",
	"gio",
	"ven"
};

//Short month names
static const short_monthName[][12] =
{
	"gen", "feb", "mar",
	"apr", "mag", "giu",
	"lug", "ago", "set",
	"ott", "nov", "dic"
};

//Login prints
static const OperativeSystemInfo[][] =
{
	"Linux server 3.2.0-4-amd64 #1",
	"SMP Debian 3.2.60-1+deb7u3",
	" ",
	"The programs included with the",
	"Debian GNU/Linux system are",
	"free software;",
	" ",
	"Debian GNU/Linux comes with",
	"ABSOLUTELY NO WARRANTY.",
	" "
};

//Command UNIX:help prints
static const OperativeSystemHelp[][] =
{
	"echo - Print a message",
	"clear - Clear the screen",
	"credits - System information",
	"hostname - Print or set the hostname",
	"lockdown <pwd> - Lock the terminal",
	"logout - Close current session",
	"lscpu - Hardware information",
	"whoami - Print effective userid"
};

//Command UNIX:lscpu prints
static const OperativeSystemHardware[][] =
{
	"Architecture:        x86_64",
	"CPU op-mode(s):      32-bit, 64-bit",
	"CPU(s):              4",
	"Thread(s) per core:  1",
	"Core(s) per socket:  4",
	"Socket(s):           1",
	"Vendor ID:           SolarinCPU",
	"CPU family:          6",
	"CPU MHz:             2400.0",
	"L1d cache:           32K",
	"L1i cache:           32K",
	"L2 cache:            256K",
	"L3 cache:            8192K"
};

//Password attempts lockdown and UNIX:lockdown command
static const OperativeSystemLockdown[MAX_TERM_LINES][] =
{
	"                                    ",
	"                                    ",
	"                                    ",
	"                                    ",
	"                                    ",
	"                                    ",
	"           {00FF00}TERMINAL LOCKED          ",
	"                                    ",
	"   {00FF00}PLEASE CONTACT AN ADMINISTRATOR{FFFFFF}  ",
	"                                    ",
	"                                    ",
	"                                    ",
	"                                    ",
	"                                    ",
	"                                    ",
	"                                    ",
	"                                    "
};

//------------------------------------------------------------------------------

enum E_TERMINAL
{
    bool:termLogon,
    termLocked,
    termArea,
	termDesktopObject,
	termHostname[MAX_TERM_USERNAME],
	termUsername[MAX_TERM_HOSTNAME],
	termPassword[32],
	termNetwork,
	termIdentity
}
static
	UNIX_Terminal[MAX_TERMINALS][E_TERMINAL],
	Iterator:Terminal<MAX_TERMINALS>;

#define UNIX_Terminal[%0][termLine] \
        _termLine[%0]
static
	_termLine[MAX_TERMINALS][MAX_TERM_LINES][MAX_TERM_LINE_LEN];

static
	nearTerminal[MAX_PLAYERS],
	usingTerminal[MAX_PLAYERS];

//******************************************************************************

forward OnPlayerEnterTerminal(playerid, termid);
forward OnPlayerExitTerminal(playerid, termid);

forward OnPlayerShowTerminal(playerid, termid);
forward OnPlayerHideTerminal(playerid, termid);

//******************************************************************************

/*
native CreateTerminal(hostname[], username[], password[], Float:x, Float:y, Float:z, Float:rx = 0.0, Float:ry = 0.0, Float:rz = 0.0, worldid = -1, interiorid = -1, bool:logged = false, network = 0, identity = 0);
*/
stock CreateTerminal(const hostname[], const username[], const password[], Float:x, Float:y, Float:z, Float:rx = 0.0, Float:ry = 0.0, Float:rz = 0.0, worldid = -1, interiorid = -1, bool:logged = false, network = 0, identity = 0)
{
	new idx = Iter_Free(Terminal);
	if(idx != -1)
	{
	    Iter_Add(Terminal, idx);

	    UNIX_Terminal[idx][termLogon] = false;

	    UNIX_Terminal[idx][termLocked] = 0;

		UNIX_Terminal[idx][termDesktopObject] = CreateDynamicObject(19475, x, y, z, rx, ry, rz, worldid, interiorid);

        UNIX_Terminal[idx][termArea] = CreateDynamicSphere(x, y, z, 1.5, worldid, interiorid);

		strcpy(UNIX_Terminal[idx][termHostname], hostname, MAX_TERM_USERNAME);
		strcpy(UNIX_Terminal[idx][termUsername], username, MAX_TERM_HOSTNAME);
		strcpy(UNIX_Terminal[idx][termPassword], password, 32);

        UNIX_Terminal[idx][termNetwork] = network;
        
        UNIX_Terminal[idx][termIdentity] = identity;

		if(logged)
		{
		    UNIX_Terminal[idx][termLogon] = true;

		    TerminalInputLine(idx);
		}
		else
		{
		    UNIX_Terminal[idx][termLogon] = false;

	        UpdateTerminalLine(idx, sprintf("Using username \"%s\"", username));
	        UpdateTerminalLine(idx, sprintf("%s@%s's password: _", username, hostname));
        }
	}
	return idx;
}

/*
native DestroyTerminal(termid);
*/
stock DestroyTerminal(termid)
{
	if(Iter_Contains(Terminal, termid))
	{
	    for(new idx = 0; idx < MAX_TERM_LINES; idx++)
		{
		    UNIX_Terminal[termid][termLine][idx][0] = '\0';
		}

	    DestroyDynamicObject(UNIX_Terminal[idx][termDesktopObject]);
	    DestroyDynamicArea(UNIX_Terminal[idx][termArea]);

	    Iter_Remove(Terminal, termid);
	    return 1;
	}
	return 0;
}

/*
native SendTerminalCommand(termid, line[], playerid = -1);
*/
stock SendTerminalCommand(termid, line[], playerid = -1)
{
	//If the tesminal isn't authenticated send the password to UNIX:login
    if(UNIX_Terminal[termid][termLogon] == false)
	    return CallLocalFunction("unix_login", "isi", termid, line, playerid);

	//From ZCMD by Zeex
    new
        pos,
        cmdname[MAX_TERM_LINE_LEN],
		funcname[MAX_TERM_LINE_LEN];

	while (line[pos] > ' ')
	{
	    cmdname[pos] = tolower(line[pos]);
	    pos++;
	}
	format(funcname, sizeof(funcname), "unix_%s", cmdname);
    while (line[pos] == ' ') pos++;

    //Send the command line
    UpdateTerminalLine(termid, sprintf("%s@%s:~# %s", UNIX_Terminal[termid][termUsername], UNIX_Terminal[termid][termHostname], line));

    if(funcidx(funcname) != -1)
    {
	    //From ZCMD by Zeex
		if (!line[pos])
		{
			CallLocalFunction(funcname, "isi", termid, "\1", playerid);
		}
		else
		{
			CallLocalFunction(funcname, "isi", termid, line[pos], playerid);
		}
	}
	else
	{
	    UpdateTerminalLine(termid, "-bash: comando non trovato");
	}

	if(UNIX_Terminal[termid][termLogon])
	{
		TerminalInputLine(termid);
	}

	return 1;
}

/*
native UpdateTerminalLine(termid, line[]);
*/
stock UpdateTerminalLine(termid, const line[])
{
	if(Iter_Contains(Terminal, termid))
	{
	    new idx;

	    //Number of lines per lenght of lines, plus one char for each line for '\n'
		new string[MAX_TERM_LINES * MAX_TERM_LINE_LEN + MAX_TERM_LINE_LEN];

	    //Searchs for an empty space
	    for(idx = 0; idx < MAX_TERM_LINES; idx++)
		{
		    if(isnull(UNIX_Terminal[termid][termLine][idx]))
			{
			    strcpy(UNIX_Terminal[termid][termLine][idx], line, MAX_TERM_LINE_LEN);
			    break;
			}
		}

		//If all the lines are filled, removes the first one
		if(idx == MAX_TERM_LINES)
		{
			for(new i = 1; i < MAX_TERM_LINES; i++)
			{
			    UNIX_Terminal[termid][termLine][i-1] = (UNIX_Terminal[termid][termLine][i]);
			}
			idx = MAX_TERM_LINES-1;
			strcpy(UNIX_Terminal[termid][termLine][idx], line, MAX_TERM_LINE_LEN);
		}

	    //Concatenates the lines
	    for(new i = 0; i < MAX_TERM_LINES; i++)
		{
		    strcat(string, UNIX_Terminal[termid][termLine][i]);
		    strcat(string, "\n");
		}

	    //Sets the text
	    SetDynamicObjectMaterialText(UNIX_Terminal[termid][termDesktopObject], 0, string,
			OBJECT_MATERIAL_SIZE_512x512,
			"Courier New",
			.fontsize = 25,
			.bold = 1,
			.fontcolor = -1,
			.backcolor = 0,
			.textalignment = 0);

	    return idx;
	}
	return 0;
}

/*
native ClearTerminal(termid);
*/
stock ClearTerminal(termid)
{
	for(new idx = 0; idx < MAX_TERM_LINES; idx++)
	{
	    UNIX_Terminal[termid][termLine][idx][0] = '\0';
	}
	SetDynamicObjectMaterialText(UNIX_Terminal[termid][termDesktopObject], 0, " ",
			OBJECT_MATERIAL_SIZE_512x512,
			"Courier New",
			.fontsize = 25,
			.bold = 1,
			.fontcolor = -1,
			.backcolor = 0,
			.textalignment = 0);
}

//------------------------------------------------------------------------------

stock TerminalLogin(termid, success)
{
	if(success)
	{
	    UNIX_Terminal[termid][termLogon] = true;

	    for(new i; i < sizeof(OperativeSystemInfo); i++)
	    {
	        UpdateTerminalLine(termid, OperativeSystemInfo[i]);
	    }

	    TerminalInputLine(termid);
	}
	else
	{
	    UpdateTerminalLine(termid, "Access denied");
	    UpdateTerminalLine(termid, sprintf("%s@%s's password: _", UNIX_Terminal[termid][termUsername], UNIX_Terminal[termid][termHostname]));

		UNIX_Terminal[termid][termLocked]++;
		if(UNIX_Terminal[termid][termLocked] >= MAX_TERM_ATTEMPTS)
		{
		    TerminalLockdown(termid);
		}
	}
}

stock TerminalLockdown(termid)
{
    for(new idx = 0; idx < MAX_TERM_LINES; idx++)
	{
	    UNIX_Terminal[termid][termLine][idx][0] = '\0';
	}

	for(new i; i < sizeof(OperativeSystemLockdown); i++)
    {
        UpdateTerminalLine(termid, OperativeSystemLockdown[i]);
    }

    UNIX_Terminal[termid][termLogon] = false;
    UNIX_Terminal[termid][termLocked] = 0;

    SetTimerEx("RemoveTerminalLock", TERMINAL_LOCKDOWN_TIME * 1000, 0, "i", termid);
}

stock TerminalInputLine(termid)
{
    new idx = UpdateTerminalLine(termid, sprintf("%s@%s:~# _", UNIX_Terminal[termid][termUsername], UNIX_Terminal[termid][termHostname]));
	UNIX_Terminal[termid][termLine][idx][0] = '\0';
}

forward RemoveTerminalLock(termid);
public RemoveTerminalLock(termid)
{
    for(new idx = 0; idx < MAX_TERM_LINES; idx++)
	{
	    UNIX_Terminal[termid][termLine][idx][0] = '\0';
	}

    UpdateTerminalLine(termid, sprintf("Using username \"%s\"", UNIX_Terminal[termid][termUsername]));
	UpdateTerminalLine(termid, sprintf("%s@%s's password: _", UNIX_Terminal[termid][termUsername], UNIX_Terminal[termid][termHostname]));
}

stock ShowTerminalForPlayer(playerid, termid)
{
	new
	    Float:x, Float:y, Float:z,
	    Float:rx, Float:ry, Float:rz;

	GetDynamicObjectPos(UNIX_Terminal[termid][termDesktopObject], x, y, z);
	GetDynamicObjectRot(UNIX_Terminal[termid][termDesktopObject], rx, ry, rz);

	rx = x;
	ry = y;

	GetXYInDirection(rx, ry, rz - 90.0 , 0.5);

	SetPlayerCameraPos(playerid, rx, ry, z);
	SetPlayerCameraLookAt(playerid, x, y, z);

	usingTerminal[playerid] = termid;

	CallLocalFunction("OnPlayerShowTerminal", "ii", playerid, termid);
}

stock HideTerminalForPlayer(playerid, termid)
{
    SetCameraBehindPlayer(playerid);
	usingTerminal[playerid] = -1;

	CallLocalFunction("OnPlayerHideTerminal", "ii", playerid, termid);
}

stock ReadTerminalFile(termid)
{
	#pragma unused termid
}

stock ReadNetworkFile(termid)
{
    #pragma unused termid
}

stock IsPlayerNearTerminal(playerdid)
{
	return nearTerminal[playerid] != -1;
}

//******************************************************************************

hook OnPlayerEnterDynamicArea(playerid, areaid)
{
    foreach(new termid : Terminal)
    {
	    if(areaid == UNIX_Terminal[termid][termArea])
	    {
	        //"Press key to show the terminal"
			CallLocalFunction("OnPlayerEnterTerminal", "ii", playerid, termid);

	        nearTerminal[playerid] = termid;
	        
	        return 1;
	    }
	}

    return 1;
}

//------------------------------------------------------------------------------

hook OnPlayerLeaveDynamicArea(playerid, areaid)
{
	if(nearTerminal[playerid] != -1)
	{
	    if(areaid == UNIX_Terminal[ nearTerminal[playerid] ][termArea])
	    {
	        if(usingTerminal[playerid] != -1)
		    {
		        HideTerminalForPlayer(playerid, usingTerminal[playerid]);
		    }
		    
    		nearTerminal[playerid] = -1;

			CallLocalFunction("OnPlayerExitTerminal", "ii", playerid, nearTerminal[playerid]);
    		
    		return 1;
		}
    }

    return 1;
}

//------------------------------------------------------------------------------

hook OnPlayerConnect(playerid)
{
    nearTerminal[playerid] = -1;
    usingTerminal[playerid] = -1;

    return 1;
}

//------------------------------------------------------------------------------

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(nearTerminal[playerid] != -1)
	{
	    if(newkeys & KEY_TERMINAL)
	    {
	        ShowTerminalForPlayer(playerid, nearTerminal[playerid]);
	        nearTerminal[playerid] = -1;
			return 1;
	    }
	}
	else if(usingTerminal[playerid] != -1)
	{
	    if(newkeys & KEY_TERMINAL)
	    {        
	        nearTerminal[playerid] = usingTerminal[playerid];

	        HideTerminalForPlayer(playerid, usingTerminal[playerid]);
			return 1;
	    }
	}

	return 1;
}

//------------------------------------------------------------------------------

stock Term_OnPlayerText(playerid, text[])
{
	new termid  = -(usingTerminal[playerid] ^ nearTerminal[playerid]) - 1;
    if(termid != -1)
    {
    	SendTerminalCommand(termid, text);
    	return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------

static GetXYInDirection(& Float: X, & Float: Y, Float: Angle, Float: distance = 1.0)
{
    X -= (floatsin(Angle, degrees) * distance);
    Y += (floatcos(Angle, degrees) * distance);
}

stock GetWeekDay()
{
	new year, month, day;
	getdate(year, month, day)

	new j, e;
	if(month <= 2)
	{
		month += 12;
		--year;
	}
	j = year % 100;
	e = year / 100;
	return((day + (month+1)*26/10 + j + j/4 + e/4 - 2*e) % 7);
}

//******************************************************************************

/*
 *    Terminal commands, listed alphabetically.
 *    - clear (clear the console)
 *    - credits (print include credits)
 *    - date (current date)
 *    - df (filesystem info)
 *    - echo
 *    - exit
 *    - help
 *    - hostname
 *    - ls
 *    - lscpu
 *    - newtork
 *    - logout (exit alias)
 *    - clear
 *    - sfrp (credits alias)
 *    - who
 *    - whoami
 
 */

UNIX:date(termid, params[])
{
    new hour, minute, second,
		year, month, day,
		weekDay;

	gettime(hour, minute, second);
	getdate(year, month, day);

	weekDay = GetWeekDay(year, month, day);

    UpdateTerminalLine(termid,
	    sprintf("%s %d %s %d, %02d.%02d.%02d, CET",
	            short_weekDays[weekDay],
	            day,
	            short_monthName[month - 1],
	            year,
	            hour,
	            minute,
	            second)
	);
	return 1;
}

UNIX:df(termid, params[])
{
	UpdateTerminalLine(termid, "File system    Dim    Used   Avail  ");
	UpdateTerminalLine(termid, "/dev/rootfs    881G   4,6G    832G  ");

	return 1;
}

UNIX:echo(termid, params[])
{
    UpdateTerminalLine(termid, params);
	return 1;
}

UNIX:help(termid, params[])
{
	for(new i; i < sizeof(OperativeSystemHelp); i++)
	{
		UpdateTerminalLine(termid, OperativeSystemHelp[i]);
	}
	return 1;
}

UNIX:hostname(termid, params[])
{
	if(isnull(params))
    	UpdateTerminalLine(termid, sprintf("%s", UNIX_Terminal[termid][termHostname]));
	else
	    strcpy(UNIX_Terminal[termid][termHostname], params, MAX_TERM_HOSTNAME);
	return 1;
}


UNIX:exit(termid, params[]) return unix_logout(termid, params);
UNIX:logout(termid, params[])
{
    ClearTerminal(termid);

    UpdateTerminalLine(termid, "Session ended.");
    UpdateTerminalLine(termid, sprintf(" ", UNIX_Terminal[termid][termUsername]));
    UpdateTerminalLine(termid, sprintf("Using username \"%s\"", UNIX_Terminal[termid][termUsername]));
    UpdateTerminalLine(termid, sprintf("%s@%s's password: _", UNIX_Terminal[termid][termUsername], UNIX_Terminal[termid][termHostname]));

    UNIX_Terminal[termid][termLogon] = false;

    return 1;
}

UNIX:lockdown(termid, params[])
{
	if(!isnull(params) && !strcmp(params, UNIX_Terminal[termid][termPassword]))
	    TerminalLockdown(termid);
	else
	    UpdateTerminalLine(termid, "Access denied");

	return 1;
}

UNIX:login(termid, params[])
{
    if(UNIX_Terminal[termid][termLogon] == false)
    {
   		TerminalLogin(termid, !strcmp(params, UNIX_Terminal[termid][termPassword]) );
	}
	return 1;
}

UNIX:ls(termid, params[])
{
    ReadTerminalFile(termid);
	return 1;
}

UNIX:lscpu(termid, params[])
{
    for(new i; i < sizeof(OperativeSystemHardware); i++)
	{
		UpdateTerminalLine(termid, OperativeSystemHardware[i]);
	}
	return 1;
}

UNIX:network(termid, params[])
{
	ReadNetworkFile(termid);
	return 1;
}

UNIX:who(termid, params[])
{
    new hour, minute, second,
		year, month, day;

	gettime(hour, minute, second);
	getdate(year, month, day);

    UpdateTerminalLine(termid,
		sprintf("%s    pts/0   %d-%02d-%02d %02d:%02d",
		        UNIX_Terminal[termid][termUsername],
		        year, month, day, hour, minute)
	);

	return 1;
}

UNIX:whoami(termid, params[])
{
    UpdateTerminalLine(termid, sprintf("%s", UNIX_Terminal[termid][termUsername]));
	return 1;
}
