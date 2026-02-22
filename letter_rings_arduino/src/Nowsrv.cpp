#include "Nowsrv.h"

esp_now_peer_info_t Nowsrv::peerInfo;
bool Nowsrv::hasBegun;

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
bool Nowsrv::begin() {

    WiFi.mode(WIFI_STA);

    Nowsrv::hasBegun = esp_now_init() == ESP_OK;

    if (Nowsrv::hasBegun) {

        esp_now_register_send_cb(esp_now_send_cb_t(Nowsrv::OnDataSent));

        memcpy(Nowsrv::peerInfo.peer_addr, STA_ADDRESS_OUT, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        Nowsrv::hasBegun &= esp_now_add_peer(&peerInfo) == ESP_OK;

        esp_now_register_recv_cb(esp_now_recv_cb_t(Nowsrv::OnDataRecv));
    }
    return Nowsrv::hasBegun;
}

bool Nowsrv::sendAcceleration() {
    esp_err_t result = esp_now_send(STA_ADDRESS_OUT, (uint8_t*)&Orientation::acceleration, sizeof(acceleration__t));
    return result == ESP_OK;
}

bool Nowsrv::sendBitmaps(bitmaps_______t sendBitmaps) {
    esp_err_t result = esp_now_send(STA_ADDRESS_OUT, (uint8_t*)&sendBitmaps, sizeof(bitmaps_______t));
    return result == ESP_OK;
}

bool Nowsrv::sendDeviceRole(device_role___t deviceRole) {
    esp_err_t result = esp_now_send(STA_ADDRESS_OUT, (uint8_t*)&deviceRole, sizeof(device_role___t));
    return result == ESP_OK;
}

void Nowsrv::OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
    // Serial.print("\r\nLast Packet Send Status:\t");
    // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    //   if (status ==0){
    //     success = "Delivery Success :)";
    //   }
    //   else{
    //     success = "Delivery Fail :(";
    //   }
}

void Nowsrv::OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    if (len == sizeof(acceleration__t)) {  // acceleration data from the other device
        acceleration__t incomingAcceleration;
        memcpy(&incomingAcceleration, incomingData, sizeof(acceleration__t));
        Orientation::calculateCoefficient(Orientation::acceleration, incomingAcceleration);
        if (Device::getDeviceRole() != DEVICE_ROLE_____SEC) {          // can not range role when in SEC, must remain passive
            if (Orientation::coefficient >= ACCELERATION_THRESHOLD) {  // should be PRI
                if (Device::getDeviceRole() != DEVICE_ROLE_____PRI) {  // but is not
                    Device::setDeviceRole(DEVICE_ROLE_____PRI);
                    Nowsrv::sendDeviceRole({DEVICE_ROLE_____SEC});  // tell other device to be SEC
                }
            } else {                                                   // should be ANY
                if (Device::getDeviceRole() != DEVICE_ROLE_____ANY) {  // but is not
                    Device::setDeviceRole(DEVICE_ROLE_____ANY);
                    Nowsrv::sendDeviceRole({DEVICE_ROLE_____ANY});  // reset other device to ANY
                }
            }
        }
    } else if (len == sizeof(bitmaps_______t)) {  // bitmaps from the other device
        bitmaps_______t incomingBitmaps;
        memcpy(&incomingBitmaps, incomingData, sizeof(bitmaps_______t));
        Device::currBitmaps = incomingBitmaps;
        // if (incomingBitmaps.bitmapA.bitmap == BITMAP_________NONE) {  // a reset from DEVICE_ROLE_____SEC to DEVICE_ROLE_____ANY
        //     Device::setDeviceRole(DEVICE_ROLE_____ANY);
        //     // Serial.print(DEVICE____________SIDE);
        //     // Serial.println("received void bitmaps");
        // } else {  // putting the device into DEVICE_ROLE_____SEC
        //     Device::setDeviceRole(DEVICE_ROLE_____SEC);
        //     Device::currBitmaps = incomingBitmaps;
        // }
    } else if (len == sizeof(device_role___t)) {  // device mode from the other device
        device_role___t incomingDevciveRole;
        memcpy(&incomingDevciveRole, incomingData, sizeof(device_role___t));
        Device::setDeviceRole(incomingDevciveRole.deviceRole);
    }
}