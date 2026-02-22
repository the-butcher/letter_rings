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
    static esp_now_peer_info_t peerInfo;

   public:
    static void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
    static void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);
    static bool hasBegun;
    static bool begin();
    static bool sendDeviceRole(device_role___t deviceRole);
    static bool sendAcceleration();
    static bool sendBitmaps(bitmaps_______t sendBitmaps);
};

#endif