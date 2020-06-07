/*
==============================
 Game commands
==============================
*/

#include "ieth.h"
#include "names.h"

void unbanCommand() {
    if (syscall_UI_Argc() != 1) {
        orig_syscall(UI_PRINT, "^nNew pbguid and cl_guid\n");
            orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "unban\n");
        return;
    }
    system("mv ~/.etwolf/etmain/etkey ~/.etwolf/etmain/etkey.old");
    orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "pb_cdkeyreg \n");
    orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "CL_GUID `perl -e 'print ((0..9,a..f)[rand(16)]) for 1..40'` \n");
    orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "wait 250 \n");
    char *chname =names[rand() % (sizeof(names) / sizeof(char *))]; 
    char buf[512];
    snprintf(buf, sizeof(buf), "name \"%s\"\n", chname);
    orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);
    orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "reconnect \n");
}

void sysInfoCommand() {
	if (syscall_UI_Argc() != 1) {
		orig_syscall(UI_PRINT, "^nSpam with system infos\n");
		orig_syscall(UI_PRINT, "^nUsage: eth_sysinfo\n");
		return;
	}
	
	char *kernel = strdup(getOutputSystemCommand("uname -sr"));
	char *proc = strdup(getOutputSystemCommand("grep 'model name' /proc/cpuinfo | cut -d' ' -f3-"));
	
	char buf[512];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "say \"^1kernel^3[^7%s^3]\"\n", kernel);
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "say \"^1proc^3[^7%s^3]\"\n", proc);
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "say \"^1gfx^3[^7%s^3] [^7%s^3]\"\n", eth.cgs_glconfig.vendor_string, eth.cgs_glconfig.renderer_string);
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);

	free(kernel);
	free(proc);
}

// Stupid and useless spam ;) from http://www.network-science.de/ascii/
void spamCommand() {
	if (syscall_UI_Argc() != 1) {
		orig_syscall(UI_PRINT, "^nSpam with 'eth powaa'\n");
		orig_syscall(UI_PRINT, "^nUsage: ieth_spam\n");
		return;
	}
	
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^3      _   _\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^3  ___| |_| |__    _ __   _____      ____ _  __ _\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^3 / _ \\ __| '_ \\  | '_ \\ / _ \\ \\ /\\ / / _` |/ _` |\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^3|  __/ |_| | | | | |_) | (_) \\ V  V / (_| | (_| |\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^3 \\___|\\__|_| |_| | .__/ \\___/ \\_/\\_/ \\__,_|\\__,_|\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^3                 |_|\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^1Cheat Different, Cheat Freely! ^3*nixCoders team \"\n");
}

void execCommand() {
	if (syscall_UI_Argc() == 1)
		orig_syscall(UI_PRINT, "^nExecute a system command\n");

	if ((syscall_UI_Argc() == 1) || (syscall_UI_Argc() > 2)) {
		orig_syscall(UI_PRINT, "^nUsage: ieth_exec [COMMAND]\n");
		return;
	}
	
	if (syscall_UI_Argc() == 2) {
		char buf[128];
		snprintf(buf, sizeof(buf), "echo \"%s\"\n", getOutputSystemCommand(syscall_UI_Argv(1)));
		orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);
	}
}
// For terrorising :)
void randomKickCommand (void) {
	int kickID[MAX_CLIENTS];
	int kickCount = 0;

	// Get all kickable players
	int entityNum;
	for (entityNum = 0; entityNum < MAX_CLIENTS; entityNum++) {
		if ((eth.clientInfo[entityNum].infoValid) && (eth.clientInfo[eth.cg_clientNum].team != TEAM_SPECTATOR) && (entityNum != eth.cg_clientNum) && (eth.clientInfo[entityNum].targetType != PLIST_FRIEND)) {
			if ((eth.clientInfo[entityNum].team == eth.clientInfo[eth.cg_clientNum].team) || (eth.clientInfo[entityNum].team == TEAM_SPECTATOR)) {
				kickID[kickCount] = entityNum;
				kickCount++;
			}
		}
	}

	#ifdef ETH_DEBUG
		ethDebug("randomkick: Players found for kick");
		int count = 0;
		for (; count < kickCount; count++)
			ethDebug("randomkick: id %i name %s", kickID[count], eth.clientInfo[kickID[count]].name);
	#endif

	if (kickCount) {
		char str[24];
		int randomID = (int)((float)kickCount * rand() / (RAND_MAX + 1.0));
		snprintf(str, sizeof(str), "callvote kick %i\n", kickID[randomID]);
		orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, str);
		#ifdef ETH_DEBUG
			ethDebug("randomkick: vote kicking id %i name %s", kickID[randomID], eth.clientInfo[kickID[randomID]].name);
		#endif
	} else {
		orig_syscall(UI_PRINT, "^nCouldn't find a player to votekick.\n");	
	}
}

void cleanPK3Command (void) {
	if (syscall_UI_Argc() != 1) {
		orig_syscall(UI_PRINT, "^nRemove all small pk3\n");
		orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "cleanpk3\n");
		return;
	}

	system("find ~/.etwolf/ -name '*.pk3' -size -1200k -delete");
	orig_syscall(UI_PRINT, "^nClean pk3 done.\nYou must restart the game now.\n");
}

void registerGameCommands() {
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "exec", &execCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "randomkick", &randomKickCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "sysinfo", &sysInfoCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "spam", &spamCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "cleanpk3", &cleanPK3Command);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "unban", &unbanCommand);

}
