#include "Blesrv.h"

BLEServer* Blesrv::pServer;
BLEService* Blesrv::pService;
BLECharacteristic* Blesrv::pLabelCharacteristic;
BLECharacteristic* Blesrv::pWordCharacteristic;
BLECharacteristic* Blesrv::pModusCharacteristic;
BLECharacteristic* Blesrv::pLightCharacteristic;
BLECharacteristic* Blesrv::pCoefPCharacteristic;
BLECharacteristic* Blesrv::pCoefGCharacteristic;
String Blesrv::macAdress;

/**
 * bluetooth connection callbacks
 */
class BlesrvCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("something connected");
        Display::setNeedsConfigRedraw(); // not actually a config redraw, but the display should still wake up
    };
    void onDisconnect(BLEServer* pServer) {
        pServer->getAdvertising()->start();
        Serial.println("something disconnected");
        Display::setNeedsConfigRedraw(); // not actually a config redraw, but the display should still wake up
    }
};

/**
 * label write callback (incoming label values)
 */
class LabelCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        String newString = (char*)newValue;
        Device::label = newString;

    }
};

/**
 * word write callback (incoming word values)
 */
class WordsCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        String newString = (char*)newValue;
        Device::word = newString;

    }
};

/**
 * modus write callback (incoming modus values)
 */
class ModusCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        uint8_t bModus = newValue[0];
        if (bModus >= MODUS________CHARS && bModus <= MODUS________ACCEL) {
            Device::setCurrModus((modus_________e)bModus);
            Display::setNeedsConfigRedraw();
        }

    }
};

/**
 * light write callback (incoming light values)
 */
class LightCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        if (Matrices::setBrightness(newValue[0])) {
            Display::setNeedsConfigRedraw();
        }

    }

};

/**
 * coefP write callback (incoming coefP values)
 */
class CoefPCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        if (Orientation::setCoefPThreshold(newValue[0] / 100.0)) {
            Display::setNeedsConfigRedraw();
        }

    }

};

/**
 * coefG write callback (incoming coefG values)
 */
class CoefGCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        if (Orientation::setCoefGThreshold(newValue[0] / 100.0)) {
            Display::setNeedsConfigRedraw();
        }

    }

};

bool Blesrv::isConnected() {
    return Blesrv::pServer->getConnectedCount() > 0;
}

bool Blesrv::powerup() {

    BLEDevice::init(DEVICE____________NAME);

    Blesrv::pServer = BLEDevice::createServer();
    Blesrv::pServer->setCallbacks(new BlesrvCallbacks());

    Blesrv::pService = pServer->createService(BLEUUID(COMMAND_SERVICE___UUID), 30, 0);

    // setup label characteristic, write only
    Blesrv::pLabelCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_LABEL_____UUID, BLECharacteristic::PROPERTY_WRITE);
    Blesrv::pLabelCharacteristic->setCallbacks(new LabelCallbacks());

    // setup word characteristic, write only
    Blesrv::pWordCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_WORD______UUID, BLECharacteristic::PROPERTY_WRITE);
    Blesrv::pWordCharacteristic->setCallbacks(new WordsCallbacks());

    // setup modus characteristic, read and write, can be changed from both device and app
    Blesrv::pModusCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_MODUS_____UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    Blesrv::pModusCharacteristic->addDescriptor(new BLE2902());
    Blesrv::pModusCharacteristic->setCallbacks(new ModusCallbacks());
    Blesrv::writeModus();

    // setup light characteristic, read and write, can be changed from both device and app
    Blesrv::pLightCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_LIGHT_____UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    Blesrv::pLightCharacteristic->addDescriptor(new BLE2902());
    Blesrv::pLightCharacteristic->setCallbacks(new LightCallbacks());
    Blesrv::writeLight();

    // setup coefP characteristic, read and write, can be changed from both device and app
    Blesrv::pCoefPCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_COEFP_____UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    Blesrv::pCoefPCharacteristic->addDescriptor(new BLE2902());
    Blesrv::pCoefPCharacteristic->setCallbacks(new CoefPCallbacks());
    Blesrv::writeCoefP();

    // setup coefG characteristic, read and write, can be changed from both device and app
    Blesrv::pCoefGCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_COEFG_____UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    Blesrv::pCoefGCharacteristic->addDescriptor(new BLE2902());
    Blesrv::pCoefGCharacteristic->setCallbacks(new CoefGCallbacks());
    Blesrv::writeCoefG();

    Blesrv::pService->start();

    // add service and all characteristics to advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(COMMAND_SERVICE___UUID);
    pAdvertising->addServiceUUID(COMMAND_LABEL_____UUID);
    pAdvertising->addServiceUUID(COMMAND_WORD______UUID);
    pAdvertising->addServiceUUID(COMMAND_MODUS_____UUID);
    pAdvertising->addServiceUUID(COMMAND_LIGHT_____UUID);
    pAdvertising->addServiceUUID(COMMAND_COEFP_____UUID);
    pAdvertising->addServiceUUID(COMMAND_COEFG_____UUID);

    pServer->getAdvertising()->start();

    const uint8_t* bleAdress = esp_bt_dev_get_address();
    char bleStr[32];
    sprintf(bleStr, "%02X:%02X:%02X:%02X:%02X:%02X", (int)bleAdress[0], (int)bleAdress[1], (int)bleAdress[2], (int)bleAdress[3], (int)bleAdress[4], (int)bleAdress[5]);
    Blesrv::macAdress = String(bleStr);

    Serial.print("ble: ");
    Serial.println(Blesrv::macAdress);

    return true;

}

bool Blesrv::writeModus() {
    int cModus = Device::getCurrModus();
    Blesrv::pModusCharacteristic->setValue(cModus);
    if (Blesrv::isConnected()) {
        Blesrv::pModusCharacteristic->notify();
        return true;
    } else {
        return false;
    }
}

bool Blesrv::writeLight() {
    int cLight = Matrices::getBrightness();
    Blesrv::pLightCharacteristic->setValue(cLight);
    if (Blesrv::isConnected()) {
        Blesrv::pLightCharacteristic->notify();
        return true;
    } else {
        return false;
    }
}

bool Blesrv::writeCoefP() {
    int cCoefP = (int)round(Orientation::getCoefPThreshold() * 100);
    Blesrv::pCoefPCharacteristic->setValue(cCoefP);
    if (Blesrv::isConnected()) {
        Blesrv::pCoefPCharacteristic->notify();
        return true;
    } else {
        return false;
    }
}

bool Blesrv::writeCoefG() {
    int cCoefG = (int)round(Orientation::getCoefGThreshold() * 100);
    Blesrv::pCoefGCharacteristic->setValue(cCoefG);
    if (Blesrv::isConnected()) {
        Blesrv::pCoefGCharacteristic->notify();
        return true;
    } else {
        return false;
    }
}
