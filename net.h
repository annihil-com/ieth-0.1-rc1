#ifndef NET_H_
#define NET_H_

// ET/q3 related
#define NET_PROTOCOL "84"
#define NET_Q3MARKER "\xff\xff\xff\xff"
#define NET_STATUS_REQUEST (NET_Q3MARKER "getstatus")
#define NET_STATUS_RESPONSE (NET_Q3MARKER "statusResponse")
#define NET_STATUS_TIMEOUT 1750
#define NET_SERVERS_REQUEST (NET_Q3MARKER "getservers " NET_PROTOCOL)
#define NET_SERVERS_RESPONSE (NET_Q3MARKER "getserversResponse")
#define NET_SERVERS_RESPONSE_END "\\EOT"
#define NET_SERVERS_TIMEOUT 3000
#define NET_ET_MASTER_SERVER (server_t){ "etmaster.idsoftware.com", 27950 }


typedef int socket_t;

typedef struct server_s {
	char hostname[64];
	int port;
} server_t;

// socket helper functions
socket_t serverConnect(server_t *server, int socketType);
qboolean netSendText(socket_t socket, char *format, ...);

//socket helper even more :)
typedef enum sAction_s {
    SACTION_INIT,
    SACTION_SEND,
    SACTION_GET,
    SACTION_CLOSE
} sAction_t;

typedef struct netMsg_s {
    socket_t socket;
    sAction_t sLastAction; 
    int lastCallTime;
} netMsg_t;

// Set a random name
#define MAX_SERVERS 4096
#define MAX_NAMES 128
qboolean setRandomName();

// Get spectators
void getSpectators();

#endif /*NET_H_*/
