/*
 * eecfg.h
 *
 * Created: 08.01.2020 17:28:23
 *  Author: Piotr
 */ 


#ifndef EECFG_H_
#define EECFG_H_

uint8_t EEMEM CfgBrightness = MAX_BRIGHTNESS;
uint8_t EEMEM  CfgBacklight  = 0;
char EEMEM CfgSSID[64] = { "ASUS-4F94" };
char EEMEM CfgPassword[64]  = { "KKKKKKKK" };
char EEMEM CfgBrokerAddr[64] = { "192.168.1.16" };
uint16_t EEMEM CfgBrokerPort = 7750;
char EEMEM CfgName1[8] = { 0 };
char EEMEM CfgName2[8] = { 0 };
char EEMEM CfgExpr1[128] = { 0 };
char EEMEM CfgExpr2[128] = { 0 };
	
#define CFG_INPUT1_ENABLED 0
#define CFG_INPUT2_ENABLED 1
#define CFG_OUTPUT1_ENABLED 2
#define CFG_OUTPUT2_ENABLED 3
#define CFG_NETWORK_ENABLED 4
	
uint8_t EEMEM CfgSwitches = 0;

#endif /* EECFG_H_ */