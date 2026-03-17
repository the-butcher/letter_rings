#include "Nowsrv.h"

esp_now_peer_info_t Nowsrv::peerInfo;
bool Nowsrv::powered;
uint64_t Nowsrv::millisSend = 0;
uint64_t Nowsrv::millisRecv = 0;
uint64_t Nowsrv::destMillisWait = 2;

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

// https://randomnerdtutorials.com/esp-now-two-way-communication-esp32/
bool Nowsrv::powerup() {

    WiFi.mode(WIFI_STA);

    // String staString = WiFi.macAddress();
    Serial.print("sta: ");
    Serial.println(WiFi.macAddress());

    Nowsrv::powered = esp_now_init() == ESP_OK;

    if (Nowsrv::powered) {

        esp_now_register_send_cb(esp_now_send_cb_t(Nowsrv::OnDataSent));

        memcpy(Nowsrv::peerInfo.peer_addr, STA_ADDRESS_OUT, 6);
        Nowsrv::peerInfo.channel = 0;
        Nowsrv::peerInfo.encrypt = false;

        Nowsrv::powered &= esp_now_add_peer(&Nowsrv::peerInfo) == ESP_OK;

        esp_now_register_recv_cb(esp_now_recv_cb_t(Nowsrv::OnDataRecv));
    }

    return Nowsrv::powered;

}

bool Nowsrv::depower() {

    esp_now_deinit();

    esp_now_unregister_send_cb();
    esp_now_unregister_recv_cb();

    // TODO :: WIFI off (?)

    return true;

}

/**
 * both left and right send acceleration when in accel mode
 */
bool Nowsrv::sendDeviceData() {

#if USE_SERIAL_SYNC_OUTPUT == true
    Serial.print(DEVICE____________SIDE);
    Serial.print(", ms: ");
    Serial.print(String(millis()));
    Serial.print(", send data on core ");
    Serial.println(xPortGetCoreID());
#endif

    Nowsrv::millisSend = millis();

    // device_role___e deviceRole = Device::getDeviceRole();
    // Serial.print("outgoing device role ");
    // Serial.println(deviceRole);

    device_data___t deviceData = {
        Device::getDeviceRole(),
        Device::currBitmaps,
        Orientation::getAccelA()
    };

    esp_err_t result = esp_now_send(STA_ADDRESS_OUT, (uint8_t*)&deviceData, sizeof(acceleration__t));
    return result == ESP_OK;
}

void Nowsrv::OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
    Orientation::setAccelAMillisWait(-1); // set a negative value, will either be calculated upon recv, or remain -1 in case of failure which can prevent extra-delays in the case
    if (status == 0) {
#if USE_SERIAL_SYNC_OUTPUT == true
        Serial.print(DEVICE____________SIDE);
        Serial.print(", ms: ");
        Serial.print(String(millis()));
        Serial.print(", sent data on core ");
        Serial.println(xPortGetCoreID());
#endif
    } else {
#if USE_SERIAL_SYNC_OUTPUT == true
        Serial.print(DEVICE____________SIDE);
        Serial.print(", ms: ");
        Serial.print(String(millis()));
        Serial.print(", fail data on core ");
        Serial.println(xPortGetCoreID());
#endif
    }
}

/**
 * data receive callback
 */
void Nowsrv::OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {

    Nowsrv::millisRecv = millis();
    Orientation::setAccelAMillisWait((Nowsrv::millisRecv - Nowsrv::millisSend) - Nowsrv::destMillisWait);

    device_data___t incominingDeviceData;
    memcpy(&incominingDeviceData, incomingData, sizeof(device_data___t));
    Orientation::setAccelB(incominingDeviceData.acceleration); // will recalculate coefficient

    // Serial.print("incoming device role ");
    // Serial.print(incominingDeviceData.deviceRole);
    // Serial.print(", ACCEPT__ROLE_DOWNGRADE ");
    // Serial.println(ACCEPT__ROLE_DOWNGRADE ? "true" : "false");

    // the other device has sent to be pri -> switch to sec if not already
    if (incominingDeviceData.deviceRole == DEVICE_ROLE_____PRI && Device::getDeviceRole() != DEVICE_ROLE_____SEC) {
        bool success = Device::setDeviceRole(DEVICE_ROLE_____SEC);
        // Serial.print("set DEVICE_ROLE_____SEC due to incoming DEVICE_ROLE_____PRI, ");
        // Serial.println(success);
    }
    // the other device has sent to be any -> switch to any if in sec
    if (incominingDeviceData.deviceRole == DEVICE_ROLE_____ANY && Device::getDeviceRole() == DEVICE_ROLE_____SEC) {
        bool success = Device::setDeviceRole(DEVICE_ROLE_____ANY);
        // Serial.print("set DEVICE_ROLE_____ANY due to incoming DEVICE_ROLE_____ANY, ");
        // Serial.println(success);
    }


    if (Device::getDeviceRole() == DEVICE_ROLE_____SEC) {          // must accept bitmaps when in sec
        Device::currBitmaps = incominingDeviceData.bitmaps;
        // Serial.print("incoming bitmapA ");
        // Serial.print(incominingDeviceData.bitmaps.bitmapA.bitmap);
        // Serial.print(" offset ");
        // Serial.print(incominingDeviceData.bitmaps.bitmapA.offset);
        // Serial.print(", incoming bitmapB ");
        // Serial.print(incominingDeviceData.bitmaps.bitmapB.bitmap);
        // Serial.print(" offset ");
        // Serial.print(incominingDeviceData.bitmaps.bitmapB.offset);
        // Serial.print(", orientation ");
        // Serial.println(incominingDeviceData.bitmaps.orientation);
    } else {
        // make a role decision based on orientation threshold, only when not in SEC (must remain passive in SEC)
        if (Orientation::isAboveCoefficientThreshold()) {          // should be PRI
            if (Device::getDeviceRole() != DEVICE_ROLE_____PRI) {  // but is not
                bool success = Device::setDeviceRole(DEVICE_ROLE_____PRI);     // set to pri
                // Serial.print("set DEVICE_ROLE_____PRI due to coefficient threshold, ");
                // Serial.println(success);
            }
        } else {                                                   // should be ANY
            if (Device::getDeviceRole() != DEVICE_ROLE_____ANY) {  // but is not
                bool success = Device::setDeviceRole(DEVICE_ROLE_____ANY);        // set to any
                // Serial.print("set DEVICE_ROLE_____ANY due to coefficient threshold, ");
                // Serial.println(success);
            }
        }
    }

#if USE_SERIAL_SYNC_OUTPUT == true
    Serial.print(DEVICE____________SIDE);
    Serial.print(", ms: ");
    Serial.print(String(millis()));
    Serial.print(", recv data on core ");
    Serial.print(xPortGetCoreID());
    Serial.print(", millisWait: ");
    Serial.print(String(Orientation::getAccelA().millisWait));
    Serial.print(", millisWait: ");
    Serial.println(String(Orientation::getAccelB().millisWait));
#endif

}