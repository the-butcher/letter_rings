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
    static BLECharacteristic* pWordCharacteristic;
    static BLECharacteristic* pModusCharacteristic;
    static BLECharacteristic* pLightCharacteristic;

   public:
    /**
     * the device's bluetooth mac address
     */
    static String macAdress;
    static bool begin();
    /**
     * check if the device is currently connected (has at least 1 connection)
     */
    static bool isConnected();
    /**
     * write new modus value when changed on the device
     */
    static bool writeModus();
    /**
     * write new light value when changed on the device
     */
    static bool writeLight();
};

#endif