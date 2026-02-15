#ifndef Blesrv_h
#define Blesrv_h

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <esp_bt_device.h>
#include <esp_bt_main.h>

#include "Define.h"
#include "Display.h"
#include "Matrices.h"

class Blesrv {
   private:
    static BLEServer* pServer;
    static BLEService* pService;
    static BLECharacteristic* pLabelCharacteristic;
    static BLECharacteristic* pModusCharacteristic;
    static BLECharacteristic* pLightCharacteristic;

   public:
    static String macAdress;
    static bool begin();
    static bool isConnected();
    /**
     * for when the modus was changed on the device
     */
    static bool writeModus();
    /**
     * for when the brightness changes
     */
    static bool writeLight();
};

#endif