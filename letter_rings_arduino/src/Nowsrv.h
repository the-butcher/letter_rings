#ifndef Nowsrv_h
#define Nowsrv_h

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "Define.h"
#include "Device.h"
#include "Orientation.h"

class Nowsrv {
private:
    /**
     * the remote peer info
     */
    static esp_now_peer_info_t peerInfo;

    static uint64_t millisSend;
    static uint64_t millisRecv;
    /**
     * the expected latency
     */
    static uint64_t destMillisWait;


public:
    static void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
    static void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);
    static bool sendDeviceData(modus_________e determModus);
    static void handleIncomingDeviceRole(device_role___e incomingDeviceRole);
    static void handleRoleTriggers();

    static bool powered;
    static bool powerup();
    static bool depower();


};

#endif