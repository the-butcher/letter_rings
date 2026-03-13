#include "Blesrv.h"

BLEServer* Blesrv::pServer;
BLEService* Blesrv::pService;
BLECharacteristic* Blesrv::pLabelCharacteristic;
BLECharacteristic* Blesrv::pWordCharacteristic;
BLECharacteristic* Blesrv::pModusCharacteristic;
BLECharacteristic* Blesrv::pLightCharacteristic;
BLECharacteristic* Blesrv::pCoeffCharacteristic;
String Blesrv::macAdress;

// // Function to convert a struct to a byte array
// // https://wokwi.com/projects/384215584338530305
// template <typename T>
// void serializeData(const T& inputStruct, uint8_t* outputBytes) {
//     memcpy(outputBytes, &inputStruct, sizeof(T));
// }

// // Function to convert a byte array to a struct
// // https://wokwi.com/projects/384215584338530305
// template <typename T>
// void deserializeData(const uint8_t* inputBytes, uint16_t offset, T& outputStruct) {
//     memcpy(&outputStruct, inputBytes + offset, sizeof(T));
// }

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
        if (bModus >= MODUS________WORDS && bModus <= MODUS________ACCEL) {
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
 * light write callback (incoming coeff values)
 */
class CoeffCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        if (Orientation::setCoefficientThreshold(newValue[0] / 100.0)) {
            Display::setNeedsConfigRedraw();
        }

    }

};

bool Blesrv::isConnected() {
    return Blesrv::pServer->getConnectedCount() > 0;
}

bool Blesrv::begin() {

    BLEDevice::init(BLE_DEVICE_NAME);

    Blesrv::pServer = BLEDevice::createServer();
    Blesrv::pServer->setCallbacks(new BlesrvCallbacks());

    Blesrv::pService = pServer->createService(COMMAND_SERVICE___UUID);

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

    // setup light characteristic, read and write, can be changed from both device and app
    Blesrv::pCoeffCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_COEFF_____UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    Blesrv::pCoeffCharacteristic->addDescriptor(new BLE2902());
    Blesrv::pCoeffCharacteristic->setCallbacks(new CoeffCallbacks());
    Blesrv::writeCoeff();

    Blesrv::pService->start();

    // add service and all characteristics to advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(COMMAND_SERVICE___UUID);
    pAdvertising->addServiceUUID(COMMAND_LABEL_____UUID);
    pAdvertising->addServiceUUID(COMMAND_WORD______UUID);
    pAdvertising->addServiceUUID(COMMAND_MODUS_____UUID);
    pAdvertising->addServiceUUID(COMMAND_LIGHT_____UUID);
    pAdvertising->addServiceUUID(COMMAND_COEFF_____UUID);
    pServer->getAdvertising()->start();

    const uint8_t* bleAdress = esp_bt_dev_get_address();
    char bleStr[32];
    sprintf(bleStr, "%02X:%02X:%02X:%02X:%02X:%02X", (int)bleAdress[0], (int)bleAdress[1], (int)bleAdress[2], (int)bleAdress[3], (int)bleAdress[4], (int)bleAdress[5]);
    Blesrv::macAdress = String(bleStr);

    // WiFi.mode(WIFI_STA);
    // WiFi.begin();
    // String staString = WiFi.macAddress();
    // Serial.print("staAddress: ");
    // Serial.println(staString);

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

bool Blesrv::writeCoeff() {
    int cCoeff = (int)round(Orientation::getCoefficientThreshold() * 100);
    Blesrv::pCoeffCharacteristic->setValue(cCoeff);
    if (Blesrv::isConnected()) {
        Blesrv::pCoeffCharacteristic->notify();
        return true;
    } else {
        return false;
    }
}
