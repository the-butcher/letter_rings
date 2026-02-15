#include "Blesrv.h"

BLEServer* Blesrv::pServer;
BLEService* Blesrv::pService;
BLECharacteristic* Blesrv::pLabelCharacteristic;
BLECharacteristic* Blesrv::pModusCharacteristic;
String Blesrv::macAdress;

// Function to convert a struct to a byte array
// https://wokwi.com/projects/384215584338530305
template <typename T>
void serializeData(const T& inputStruct, uint8_t* outputBytes) {
    memcpy(outputBytes, &inputStruct, sizeof(T));
}

// Function to convert a byte array to a struct
// https://wokwi.com/projects/384215584338530305
template <typename T>
void deserializeData(const uint8_t* inputBytes, uint16_t offset, T& outputStruct) {
    memcpy(&outputStruct, inputBytes + offset, sizeof(T));
}

class BlesrvCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        // do nothing
        Serial.println("something connected");
        Display::needsStatusRedraw = true;
    };
    void onDisconnect(BLEServer* pServer) {
        // do nothing
        pServer->getAdvertising()->start();
        Serial.println("something disconnected");
        Display::needsStatusRedraw = true;
    }
};

class LabelCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        size_t pDataLength = pCharacteristic->getLength();
        Serial.print("pDataLength: ");
        Serial.print(String(pDataLength));
        Serial.print(", core: ");
        Serial.println(xPortGetCoreID());

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        String newString = (char*)newValue;
        // newString.toUpperCase();
        Matrices::label = newString;

        Display::needsStatusRedraw = true;
    }
};

class ModusCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        size_t pDataLength = pCharacteristic->getLength();
        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();

        Serial.print("pDataLength: ");
        Serial.print(String(pDataLength));
        Serial.print(", core: ");
        Serial.print(xPortGetCoreID());
        Serial.print(", newValue: ");
        Serial.println(String(newValue[0]));

        uint8_t bModus = newValue[0];
        if (bModus >= MODUS________WORDS && bModus <= MODUS________FREQU) {
            // Display::clearLabel();
            // Display::clearFrequ();
            Device::modus = (modus_________e)bModus;
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

    Blesrv::pLabelCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_LABEL_____UUID, BLECharacteristic::PROPERTY_WRITE);
    Blesrv::pLabelCharacteristic->setCallbacks(new LabelCallbacks());

    Blesrv::pModusCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_MODUS_____UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    Blesrv::pModusCharacteristic->addDescriptor(new BLEDescriptor(COMMAND_MODUS_DSC_UUID, sizeof(modus_________e)));
    Blesrv::pModusCharacteristic->addDescriptor(new BLE2902());
    Blesrv::pModusCharacteristic->setCallbacks(new ModusCallbacks());
    // initial value
    int cModus = Device::modus;
    Blesrv::pModusCharacteristic->setValue(cModus);

    Blesrv::pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(COMMAND_SERVICE___UUID);
    pAdvertising->addServiceUUID(COMMAND_LABEL_____UUID);
    pAdvertising->addServiceUUID(COMMAND_MODUS_____UUID);
    pAdvertising->addServiceUUID(COMMAND_MODUS_DSC_UUID);
    pServer->getAdvertising()->start();

    const uint8_t* point = esp_bt_dev_get_address();
    char str[32];
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", (int)point[0], (int)point[1], (int)point[2], (int)point[3], (int)point[4], (int)point[5]);
    Blesrv::macAdress = String(str);

    return true;
}

bool Blesrv::writeModus() {
    if (Blesrv::isConnected()) {
        int cModus = Device::modus;
        Blesrv::pModusCharacteristic->setValue(cModus);
        Blesrv::pModusCharacteristic->notify();
        return true;
    } else {
        return false;
    }
}
