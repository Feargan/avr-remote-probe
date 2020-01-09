#ifndef WLAN_H_INCLUDED
#define WLAN_H_INCLUDED

#include <inttypes.h>

#define WLAN_RESET_DDR DDRD
#define WLAN_RESET PD2
#define WLAN_RESET_PORT PORTD

#define WLAN_MAX_AP_ENTRIES 4

#define WLAN_OK 0x0
#define WLAN_INIT_ERROR_NO_ANSWER 0x1
#define WLAN_INIT_ERROR_CORRUPTED_ANSWER 0x2
#define WLAN_ERROR -1
#define WLAN_ERROR_UART_TIMEOUT 0x3
#define WLAN_EVENT_DISCONNECT 0x1

#define WLAN_CHIP_OK 0
#define WLAN_AP_CONNECTED 1
#define WLAN_TCP_CONNECTED 2

typedef struct
{
    char SSID[32];
    int8_t Encryption;
    int8_t RSSID;
} WlanInfo;

typedef void (*RecvCallback)(uint8_t, char*);

WlanInfo WLAN_AccessPoints[WLAN_MAX_AP_ENTRIES];

uint8_t WLAN_GetState();
uint8_t WLAN_Init();
int8_t WLAN_ListAccessPoints();
int8_t WLAN_JoinAccessPoint(const char* SSID, const char* Password);
int8_t WLAN_TcpConnect(const char* IP_Address, uint16_t Port);
int8_t WLAN_TcpSend(const char* Message);
int8_t WLAN_TcpSend_P(const char* Message);
int8_t WLAN_Disconnect();
//void WLAN_SetRecvListener(RecvCallback Recv);
//void WLAN_SetEventListener(void (*OnEventFn)(uint8_t)));
int8_t WLAN_Listen(RecvCallback OnRecv); // expand to: return type -> int - indicating possible errors, so that it can be used simultaneously as error checker and packet receiver, extended: max timeout

#endif // WLAN_H_INCLUDED
