// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

/*
==============================
All network code
==============================
*/

#define _GNU_SOURCE
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "ieth.h"
#include "net.h"

socket_t serverConnect(server_t *server, int socketType) {
    // Don't use sockets while watching demos
    if (eth.demoPlayback)
	return -1;

    socket_t sock;
    struct sockaddr_in sin;
    struct hostent *he;

    // Check hostname
    if ((he = gethostbyname(server->hostname)) == NULL) {
    	ethLog("irc: gethostbyname error");
        herror(server->hostname);
        return -1;
    }

    // Create socket
    sock = socket(AF_INET, socketType, 0);
    bcopy(he->h_addr, (char *)&sin.sin_addr, he->h_length);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server->port);

    // Connect the socket
    int result = connect(sock, (struct sockaddr *)&sin, sizeof(sin));
    if (result != 0) {
    	ethLog("net: connect error to %s", server->hostname);
    	perror("eth: connect error");
        return -1;
    }

    // Set non-blocking socket
    fcntl(sock, F_SETFL, O_NONBLOCK);

	return sock;
}

qboolean netSendRaw(socket_t socket, char *buffer, int size) {
	if (send(socket, buffer, size, 0) < 0) {
		ethLog("net: send raw error\n");
        return qfalse;
	}

	#ifdef ETH_DEBUG
		ethDebug("net: send packet: size: %i data: [%s]", size, buffer);
	#endif

	return qtrue;
}

// Send text with a trailing '\n'
qboolean netSendText(socket_t socket, char *format, ...) {
	char buffer[512];
	char *trailing = "\n";
	
	va_list arglist;
	va_start(arglist, format);
		int size = vsnprintf(buffer, sizeof(buffer) - strlen(trailing), format, arglist);
	va_end(arglist);

	// Sanity check
	if (size > strlen(buffer)) {
		ethLog("error: net send buffer exceed\n");
		return qfalse;
	}

	strncat(buffer, trailing, sizeof(buffer) - strlen(trailing));
	
	return netSendRaw(socket, buffer, strlen(buffer));
}


// This function get all servers infos from the master servers
// Return false if error
static server_t servers[MAX_SERVERS];	// All servers received
static int serverCount;				// Numbers of servers
static qboolean serversInit;			// True when request servers done

qboolean getServersList() {
	#define NET_RAW_SERVER_SIZE 7
	static socket_t socket = -1;
	static int requestTime = 0;
	
	// Connect to master server
	if (socket == -1) {
		socket = serverConnect(&NET_ET_MASTER_SERVER, SOCK_DGRAM);
		netSendText(socket, NET_SERVERS_REQUEST);
		requestTime = eth.cg_time;
		return qtrue;
	}

	// If connect error
	if (socket == -1) {
		ethLog("error: can't connect to ET master server\n");
		return qfalse;
	}

	// Parse response from master server
	if ((requestTime + NET_SERVERS_TIMEOUT) > eth.cg_time) {
		char buffer[(MAX_SERVERS * NET_RAW_SERVER_SIZE) + strlen(NET_SERVERS_RESPONSE) + 1];
	    memset(buffer, 0, sizeof(buffer));

	    int size = recv(socket, buffer, sizeof(buffer), 0);

	    // recv() error
		if (size == 0) {
			ethLog("error: getServerList: can't receive data\n");
			close(socket);
			socket = -1;
			return qfalse;
		}
	    
	    // No data received
		if (size == -1)
			return qtrue;

		// Invalid packet received
		if ((strncmp(buffer, NET_SERVERS_RESPONSE, strlen(NET_SERVERS_RESPONSE)) != 0)
				|| (strncmp(&buffer[size - strlen(NET_SERVERS_RESPONSE_END)], NET_SERVERS_RESPONSE_END, strlen(NET_SERVERS_RESPONSE_END)) != 0)) {
			ethLog("error: invalid packet marker from master server\n");
			close(socket);
			socket = -1;
			return qfalse;
		}
		
		int offset = strlen(NET_SERVERS_RESPONSE);

		// Get all servers
		while (offset < (size - strlen(NET_SERVERS_RESPONSE_END))) {
			if (buffer[offset] != '\\') {
				#ifdef ETH_DEBUG
					ethDebug("warning: getServerList: invalid server in packet\n");
				#endif
				return qtrue;
			}

			server_t *server = &servers[serverCount];
			snprintf(server->hostname, sizeof(server->hostname), "%i.%i.%i.%i", (unsigned char)buffer[offset + 1], (unsigned char)buffer[offset + 2], (unsigned char)buffer[offset + 3], (unsigned char)buffer[offset + 4]);
			server->port = htons(*(uint16_t *)&buffer[offset + 5]);
			serverCount++;
			offset += NET_RAW_SERVER_SIZE;
		}
		return qtrue;
	}

	// Request timeout
	if (serverCount == 0) {
		ethLog("timeout error: can't get servers from ET master server");
		close(socket);
		socket = -1;
		return qfalse;
	}

	// Success !
	#ifdef ETH_DEBUG
		ethDebug("getServerList: received %i servers from ET master server", serverCount);
	#endif
	serversInit = qtrue;
	close(socket);
	socket = -1;
	return qtrue;
}


static char names[MAX_NAMES][MAX_QPATH];
static int nameCount;
static qboolean namesInit;
qboolean getNames(server_t server) {
	static socket_t socket = -1;
	static int requestTime = 0;
	char sname[MAX_QPATH];

	if (socket == -1) {
		server_t *server = &servers[(int)((float)serverCount * rand() / RAND_MAX)];
		socket = serverConnect(server, SOCK_DGRAM);
		netSendText(socket, NET_STATUS_REQUEST);
		requestTime = eth.cg_time;
		return qtrue;
	}

	// If connect error
	if (socket == -1)
		return qtrue;

	// Parse data
	if ((requestTime + NET_STATUS_TIMEOUT) > eth.cg_time) {
		char buffer[8192];
	    memset(buffer, 0, sizeof(buffer));

	    int size = recv(socket, buffer, sizeof(buffer), 0);

	    // recv() error
		if (size == 0) {
			ethLog("error: getNames: can't receive data");
			close(socket);
			socket = -1;
			return qfalse;
		}

	    // No data received
		if (size == -1)
			return qtrue;

		#ifdef ETH_DEBUG
			ethDebug("getNames: received len: %i data: %s", size, buffer);
		#endif

		// Invalid packet received
		if (strncmp(buffer, NET_STATUS_RESPONSE, strlen(NET_STATUS_RESPONSE)) != 0) {
			ethLog("error: getNames: invalid server packet marker");
			close(socket);
			socket = -1;
			return qfalse;
		}

		char *str = buffer;
		str += strlen(NET_STATUS_RESPONSE) + 1;
		str = strchr(str, '\n') + 1;	// Skip server infos
		
		// Check for empty server
		if (!strlen(str))
			return qtrue;

		// Get all names
		while (str && (str < buffer + size) && (nameCount < MAX_NAMES)) {
			char line[128];
			memset(line, 0, sizeof(line));
			strncpy(line, str, strchr(str, '\n') - str);
			
			// Next line
			str = strchr(str, '\n') + 1;
			
			// Search name
			char *start = strchr(line, '"') + 1;
			char *end = strrchr(line, '"') - 1;
			int len = end - start + 1;

			// Check infos
			if ((start == NULL) || (end == NULL) || (start == end)) {
				ethLog("warning: getNames: invalid player infos received");
				return qtrue;
			}

			// Get name
			strncpy(names[nameCount], start, len);
			names[nameCount][len] = '\0';

			// Don't add bad names
			int i, b;
			for (b = 0, i = 0; i < len; i++)
				if (names[nameCount][i] < 0x20) { // XXX: (> 0x7f) is (< 0)
					b = 1; // extended ascii 
					break; 
				}
			if (b)
				continue;

			strncpy(sname, names[nameCount], sizeof(sname));
			Q_CleanStr(sname);
			if ((strlen(sname) <= 3) || strcasestr(sname, "bot"))
				continue;
			
			nameCount++; // k
		}

		#ifdef ETH_DEBUG
			ethDebug("getNames: total names: %i", nameCount);
		#endif

		return qtrue;
	}
	
	// Retry if we need more names
	if (nameCount < MAX_NAMES) {
		close(socket);
		socket = -1;
		return qtrue;
	}
	
	// Success !
	namesInit = qtrue;
	close(socket);
	socket = -1;
	return qtrue;
}

// Return false if error
qboolean setRandomName() {
	char cmd[256], *name;
	static int lastNameChange;

	// Wait before set a random name
	if ((lastNameChange + (seth.value[VAR_RANDOMNAME_DELAY] * 1000)) > eth.cg_time)
		return qtrue;

	if (seth.value[VAR_RANDOMNAME] == 2) {
		int count1, count2;
		int availableNickIDs[MAX_CLIENTS];
		for (count1 = 0, count2 = 0; count1 < MAX_CLIENTS; count1++) {
			if (eth.clientInfo[count1].infoValid)
				availableNickIDs[count2++] = count1;
		}
		count2--;
		if (count2 > 0)
			strncpy(eth.clientInfo[eth.cg_clientNum].name, eth.clientInfo[availableNickIDs[rand() % count2]].name, sizeof(eth.clientInfo->name));
		else
			return qfalse;
	} else if (seth.value[VAR_RANDOMNAME] == 1) {
		// Get all servers
		if (!serversInit)
			return getServersList();

		// Get names from random servers
		if (!namesInit)
			return getNames(servers[(int)((float)serverCount * rand() / RAND_MAX)]);

		// Change name
		do name = names[rand() % nameCount];
		while (getIdByName(name, 0) != -1);

		strncpy(eth.clientInfo[eth.cg_clientNum].name, name, sizeof(eth.clientInfo->name));
	}

	snprintf(cmd, sizeof(cmd), "name \"%s\"\n", eth.clientInfo[eth.cg_clientNum].name);
	syscall_CG_SendConsoleCommand(cmd);

	lastNameChange = eth.cg_time;
	return qtrue;
}

void getSpectators() {
	static netMsg_t specZ;
	char buffer[4096];
	struct timeval tv;
	fd_set socks;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	// Connect
	if (specZ.socket < 1 && (specZ.sLastAction == SACTION_CLOSE || specZ.sLastAction == SACTION_INIT)) {
		specZ.socket = serverConnect(&eth.server, SOCK_DGRAM);
		specZ.sLastAction = SACTION_INIT;
	}

	if (specZ.socket && specZ.sLastAction == SACTION_INIT) {
		FD_ZERO(&socks);
		FD_SET(specZ.socket, &socks);
		select(specZ.socket + 1, NULL, &socks, NULL, &tv); //check if we can
		// write to socket
		if (FD_ISSET(specZ.socket,&socks)) {
			netSendText(specZ.socket, NET_STATUS_REQUEST);
			specZ.lastCallTime = eth.cg_time;
			specZ.sLastAction = SACTION_SEND;
	    } else 
			return; // if not then w8 till next call and so on ...
	}

	// Only do request each NET_STATUS_TIMEOUT
	if ((specZ.lastCallTime + NET_STATUS_TIMEOUT) <= eth.cg_time) {
		close(specZ.socket);
		specZ.socket = -1;
		specZ.sLastAction = SACTION_CLOSE;
		return;
	}

	if (specZ.socket && specZ.sLastAction == SACTION_SEND) {
		FD_ZERO(&socks);
		FD_SET(specZ.socket, &socks);
		select(specZ.socket + 1, &socks, NULL, NULL, &tv); //check if we can
		// read socket
		if (FD_ISSET(specZ. socket, &socks)) {
			memset(buffer, 0, sizeof(buffer));
			int size = recv(specZ.socket, buffer, sizeof(buffer), 0);

			// If no data receive - shouldn't happen but k.
			if (size == -1)
			    return;
			
			// If valid packet
			if (strncmp(buffer, NET_STATUS_RESPONSE, strlen(NET_STATUS_RESPONSE)) != 0) {
				ethLog("warning: getSpectators: invalid packet");
				#ifdef ETH_DEBUG
					ethLog("net: getSpectators: invalid packet: [%s]", buffer);
				#endif
				return;
			}
		
			char *str = buffer;
			// Skip header
			str += strlen(NET_STATUS_RESPONSE) + 1;
			// Skip server infos
			str = strchr(str, '\n') + 1;
		
			// Reset spec infos
			seth.specCount = 0;
			memset(seth.specNames, 0, sizeof(seth.specNames));
		
			// Get all names
			while (str && (str < buffer + size)) {
			    char line[128];
			    memset(line, 0, sizeof(line));
			    strncpy(line, str, strchr(str, '\n') - str);
				
			    char xp[8];
			    memset(xp, 0, sizeof(xp));
			    unsigned int xpLen = strchr(str, ' ') - str;
			
			    // Sanity check
			    if (xpLen >= sizeof(xp)) {
					ethLog("warning: invalid player infos");
					break;
			    }
			
			    strncpy(xp, str, xpLen);

		    	int myXP;
		    	if (eth.mod->type == MOD_TCE)
		    		myXP = eth.cg_snap->ps.persistant[PERS_SCORE];
		    	else
		    		myXP = eth.cg_snap->ps.stats[STAT_XP];
				
			    if ((myXP == atoi(xp)) && (atoi(xp) != 0)) {
					char *start = strchr(line, '"') + 1;
					char *end = strrchr(line, '"') - 1;
					
					if ((start == NULL) || (end == NULL) || (start == end)) {
						ethLog("warning: invalid player infos");
						break;
					}
						
					char name[MAX_QPATH];
					memset(name, 0, sizeof(name));
					strncpy(name, start, end - start + 1);
					
					int cpt = 0;
					for (; cpt < MAX_CLIENTS; cpt++) {
						if (eth.clientInfo[cpt].infoValid && !strcmp(eth.clientInfo[cpt].name, name)
								&& (eth.clientInfo[cpt].team == TEAM_SPECTATOR)) {
							strncpy(seth.specNames[seth.specCount], name, sizeof(*seth.specNames));
							seth.specCount++;
							break;
						}
					}
			    }
				
			    // Next line
			    str = strchr(str, '\n') + 1;
			}
		}
	} // end of read 
}

