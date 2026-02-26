#include "Blesrv.h"

BLEServer* Blesrv::pServer;
BLEService* Blesrv::pService;
BLECharacteristic* Blesrv::pLabelCharacteristic;
BLECharacteristic* Blesrv::pWordCharacteristic;
BLECharacteristic* Blesrv::pModusCharacteristic;
BLECharacteristic* Blesrv::pLightCharacteristic;
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
        Display::needsStatusRedraw = true;
    };
    void onDisconnect(BLEServer* pServer) {
        pServer->getAdvertising()->start();
        Serial.println("something disconnected");
        Display::needsStatusRedraw = true;
    }
};

/**
 * label write callback (incoming label values)
 */
class LabelCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        // size_t pDataLength = pCharacteristic->getLength();
        // Serial.print("pDataLength: ");
        // Serial.print(String(pDataLength));
        // Serial.print(", core: ");
        // Serial.println(xPortGetCoreID());

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        String newString = (char*)newValue;
        // newString.toUpperCase();
        Device::label = newString;

        Display::needsStatusRedraw = true;
    }
};

/**
 * word write callback (incoming word values)
 */
class WordsCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        // size_t pDataLength = pCharacteristic->getLength();
        // Serial.print("pDataLength: ");
        // Serial.print(String(pDataLength));
        // Serial.print(", core: ");
        // Serial.println(xPortGetCoreID());

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        String newString = (char*)newValue;
        Device::word = newString;

        Display::needsStatusRedraw = true;
    }
};

/**
 * modus write callback (incoming modus values)
 */
class ModusCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        // size_t pDataLength = pCharacteristic->getLength();
        // Serial.print("pDataLength: ");
        // Serial.print(String(pDataLength));
        // Serial.print(", core: ");
        // Serial.print(xPortGetCoreID());
        // Serial.print(", newValue: ");
        // Serial.println(String(newValue[0]));

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        uint8_t bModus = newValue[0];
        if (bModus >= MODUS________WORDS && bModus <= MODUS________ACCEL) {
            Device::setCurrModus((modus_________e)bModus);
            Display::needsStatusRedraw = true;
        }
    }
};

/**
 * light write callback (incoming light values)
 */
class LightCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        // size_t pDataLength = pCharacteristic->getLength();
        // Serial.print("pDataLength: ");
        // Serial.print(String(pDataLength));
        // Serial.print(", core: ");
        // Serial.print(xPortGetCoreID());
        // Serial.print(", newValue: ");
        // Serial.println(String(newValue[0]));

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        uint8_t bLight = newValue[0];
        if (bLight >= 0 && bLight <= 15) {
            Matrices::brightness = bLight;
            Matrices::needsBrightnessUpdate = true;
            Display::needsStatusRedraw = true;
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
    Blesrv::pModusCharacteristic->addDescriptor(new BLEDescriptor(COMMAND_MODUS_DSC_UUID, sizeof(modus_________e)));
    Blesrv::pModusCharacteristic->addDescriptor(new BLE2902());
    Blesrv::pModusCharacteristic->setCallbacks(new ModusCallbacks());
    // initial value
    int cModus = Device::getCurrModus();
    Blesrv::pModusCharacteristic->setValue(cModus);

    // setup light characteristic, read and write, can be changed from both device and app
    Blesrv::pLightCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_LIGHT_____UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    Blesrv::pLightCharacteristic->addDescriptor(new BLEDescriptor(COMMAND_LIGHT_DSC_UUID, sizeof(uint8_t)));
    Blesrv::pLightCharacteristic->addDescriptor(new BLE2902());
    Blesrv::pLightCharacteristic->setCallbacks(new LightCallbacks());
    // initial value
    int cLight = Matrices::brightness;
    Blesrv::pLightCharacteristic->setValue(cLight);

    Blesrv::pService->start();

    // add service and all characteristics to advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(COMMAND_SERVICE___UUID);
    pAdvertising->addServiceUUID(COMMAND_LABEL_____UUID);
    pAdvertising->addServiceUUID(COMMAND_WORD______UUID);
    pAdvertising->addServiceUUID(COMMAND_MODUS_____UUID);
    pAdvertising->addServiceUUID(COMMAND_MODUS_DSC_UUID);
    pServer->getAdvertising()->start();

    const uint8_t* bleAdress = esp_bt_dev_get_address();
    char bleStr[32];
    sprintf(bleStr, "%02X:%02X:%02X:%02X:%02X:%02X", (int)bleAdress[0], (int)bleAdress[1], (int)bleAdress[2], (int)bleAdress[3], (int)bleAdress[4], (int)bleAdress[5]);
    Blesrv::macAdress = String(bleStr);

    // Serial.print("bleAddress: ");
    // Serial.println(Blesrv::macAdress);

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
    Blesrv::pModusCharacteristic->notify();
    return Blesrv::isConnected();
}

bool Blesrv::writeLight() {
    int cLight = Matrices::brightness;
    Blesrv::pLightCharacteristic->setValue(cLight);
    Blesrv::pLightCharacteristic->notify();
    return Blesrv::isConnected();
}
