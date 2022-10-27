/*
   -- HalloweenCoffin --

   This source code of graphical user interface
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.8 or later version
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/
     - for ANDROID 4.11.1 or later version;
     - for iOS 1.9.1 or later version;

   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

#include "PinConfig.h"
#include "CoffinMP3Player.h"
#include "CoffinServos.h"
#include "LEDs.h"
#include "Ultrasonic.h"

//#define DEBUG_ULTRASONIC

#ifndef DEBUG_ULTRASONIC
#undef DEBUG_PRINTLN
#undef DEBUG_PRINT
#define DEBUG_PRINTLN(a)
#define DEBUG_PRINT(a)
#endif

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP8266WIFI_LIB_POINT
#include <ESP8266WiFi.h>
#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "Coffin"
#define REMOTEXY_WIFI_PASSWORD ""
#define REMOTEXY_SERVER_PORT 6377


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 144 bytes
{ 255,5,0,6,0,137,0,16,176,1,3,3,4,10,16,43,36,26,3,2,
27,12,15,27,178,26,3,2,10,64,14,25,24,26,3,2,36,64,14,26,
30,26,129,0,3,2,18,6,24,65,117,103,101,110,0,129,0,27,2,14,
6,8,76,105,99,104,116,0,129,0,49,2,10,6,24,84,111,110,0,129,
0,8,58,19,6,24,68,101,99,107,101,108,0,129,0,34,58,19,6,24,
83,107,101,108,101,116,116,0,67,0,36,93,25,5,8,17,6,129,0,1,
92,34,6,8,69,110,116,102,101,114,110,117,110,103,58,0,3,4,48,9,
13,46,2,26 };

// this structure defines all the variables and events of your control interface 
struct {

    // input variables
    uint8_t Eyes; // =0 if select position A, =1 if position B, =2 if position C, ... 
    uint8_t Light; // =0 if select position A, =1 if position B, =2 if position C, ... 
    uint8_t Cover; // =0 if select position A, =1 if position B, =2 if position C, ... 
    uint8_t Corpse; // =0 if select position A, =1 if position B, =2 if position C, ... 
    uint8_t Ton; // =0 if select position A, =1 if position B, =2 if position C, ... 

    // output variables
    char Distance[6];  // string UTF8 end zero 

    // other variable
    uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
///////////////////////////////////////////// 


void SetupRemote()
{
    RemoteXY_Init();
    RemoteXY.Distance[0] = '-';
    RemoteXY.Distance[1] = 0;
}

void LoopRemote()
{
    RemoteXY_Handler();
}

bool RemoteXYIsConnected()
{
    return (RemoteXY.connect_flag);
}


void DoRemoteControlledAction()
{
    if (!AllServosFinished())
        return;

    static uint8_t shadowEyes = 42;
    if (shadowEyes != RemoteXY.Eyes)
    {
        shadowEyes = RemoteXY.Eyes;
        if (RemoteXY.Eyes == 0)
            SetEye(off);
        else if (RemoteXY.Eyes == 1)
            SetEye(glow);
        else if (RemoteXY.Eyes == 2)
            SetEye(hypno);
    }

    static uint8_t shadowLight = 42;
    if (shadowLight != RemoteXY.Light)
    { 
        shadowLight = RemoteXY.Light;
        if (RemoteXY.Light == 0)
            SetLight(false);
        else if (RemoteXY.Light == 1)
            SetLight(true); 
    }

    static uint8_t shadowCover = 42;
    if (shadowCover != RemoteXY.Cover)
    {
        shadowCover = RemoteXY.Cover;
        if (RemoteXY.Cover == 0)
            CoverOpen();
        else if (RemoteXY.Cover == 1)
            CoverClose(); 
    }

    static uint8_t shadowCorpse = 42;
    if (shadowCorpse != RemoteXY.Corpse)
    {
        shadowCorpse = RemoteXY.Corpse;
        if (RemoteXY.Corpse == 0)
            CorpseUp();
        else if (RemoteXY.Corpse == 1)
            CorpseDown(); 
    }

    static uint8_t shadowMusic = 42;
    if (shadowMusic != RemoteXY.Ton)
    {
        shadowMusic = RemoteXY.Ton;
        if (RemoteXY.Ton == 0)
            CoffinPlayMP3(KNARREN);
        else if (RemoteXY.Ton == 1)
            CoffinPlayMP3(WANTYOUTODIE);
        else if (RemoteXY.Ton == 2)
            CoffinPlayMP3(TODESSCHREI);
        else if (RemoteXY.Ton == 3)
            CoffinPlayMP3(SCHREI);
    }

    String shadowDistance = "-";
    String dist = String(distance);
    if (dist.compareTo(shadowDistance) != 0)
    {
        DEBUG_PRINTLN(dist);
        shadowDistance = dist;
        int min = (5 < dist.length()) ? 5 : dist.length();
        int i;
        for (i = 0; i < min; ++i)
            RemoteXY.Distance[i] = dist[i];
        RemoteXY.Distance[i] = 0;
    }
}