#include "Nowsrv.h"

esp_now_peer_info_t Nowsrv::peerInfo;
bool Nowsrv::powered;
uint64_t Nowsrv::millisSend = 0;
uint64_t Nowsrv::millisRecv = 0;
uint64_t Nowsrv::destMillisWait = 2;
uint8_t Nowsrv::roleTriggerTouchCount = 0;

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

/**
 * not used currently
 */
bool Nowsrv::depower() {

    esp_now_deinit();

    esp_now_unregister_send_cb();
    esp_now_unregister_recv_cb();

    WiFi.mode(WIFI_OFF);

    return true;

}

/**
 * both left and right send acceleration when in accel mode
 */
bool Nowsrv::sendDeviceData(modus_________e determModus) {

#if USE_SERIAL_SYNC_OUTPUT == true
    Serial.print(DEVICE____________SIDE);
    Serial.print(", ms: ");
    Serial.print(String(millis()));
    Serial.print(", send data on core ");
    Serial.println(xPortGetCoreID());
#endif

    Nowsrv::millisSend = millis();

    if (determModus == MODUS____GAMPM_PRI) {

        // send device data with bitmaps
        device_data_b_t deviceData = {
            Device::getDeviceRole(),
            Device::getCurrBitmaps(),
            Orientation::getMagnitudesA()
        };
        esp_err_t result = esp_now_send(STA_ADDRESS_OUT, (uint8_t*)&deviceData, sizeof(device_data_b_t));
        return result == ESP_OK;

    } else if (determModus == MODUS____GAMKN_PRI) {

        Nowsrv::roleTriggerTouchCount = 0; // reset every time GAMKN ledbar is sent, so the count has to restart upon a distinct new touch

        // send device data with ledbar
        device_data_l_t deviceData = {
            Device::getDeviceRole(),
            Device::getCurrLedbar(),
            Orientation::getMagnitudesA()
        };
        esp_err_t result = esp_now_send(STA_ADDRESS_OUT, (uint8_t*)&deviceData, sizeof(device_data_l_t));
        return result == ESP_OK;

    } else {

        // send plain device data
        device_data___t deviceData = {
            Device::getDeviceRole(),
            Orientation::getMagnitudesA()
        };
        esp_err_t result = esp_now_send(STA_ADDRESS_OUT, (uint8_t*)&deviceData, sizeof(device_data___t));
        return result == ESP_OK;

    }

}

void Nowsrv::OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
    Orientation::setMillisWaitA(-1); // set a negative value, will either be calculated upon recv, or remain -1 in case of failure which can prevent extra-delays in the case
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
    Orientation::setMillisWaitA((Nowsrv::millisRecv - Nowsrv::millisSend) - Nowsrv::destMillisWait);

    if (len == sizeof(device_data___t)) { // incoming, neither bitmaps nor ledbar

        device_data___t incominingDeviceData;
        memcpy(&incominingDeviceData, incomingData, sizeof(device_data___t));
        Orientation::setMagnitudesB(incominingDeviceData.magnitudes); // will recalculate coefP
        Nowsrv::handleIncomingDeviceRole(incominingDeviceData.deviceRole);
        if (Device::getDeviceRole() != DEVICE_ROLE_____PRI) {
            Device::setCurrDterm(DTERM_________NONE);
        }
        Nowsrv::handleRoleTriggers();

    } else if (len == sizeof(device_data_b_t)) { // incoming with bitmaps

        device_data_b_t incominingDeviceData;
        memcpy(&incominingDeviceData, incomingData, sizeof(device_data_b_t));
        Orientation::setMagnitudesB(incominingDeviceData.magnitudes); // will recalculate coefP
        Nowsrv::handleIncomingDeviceRole(incominingDeviceData.deviceRole);
        if (Device::getDeviceRole() == DEVICE_ROLE_____SEC) {         // must accept bitmaps when in sec
            Device::setCurrBitmaps(incominingDeviceData.bitmaps);     // sets dterm to GAMPM
        }
        Nowsrv::handleRoleTriggers();

    } else if (len == sizeof(device_data_l_t)) { // incoming with ledbar

        device_data_l_t incominingDeviceData;
        memcpy(&incominingDeviceData, incomingData, sizeof(device_data_l_t));
        Orientation::setMagnitudesB(incominingDeviceData.magnitudes); // will recalculate coefP
        Nowsrv::handleIncomingDeviceRole(incominingDeviceData.deviceRole);
        if (Device::getDeviceRole() == DEVICE_ROLE_____SEC) {         // must accept ledbars when in sec
            Device::setCurrLedbar(incominingDeviceData.ledbar);       // sets dterm to GAMKN
        }
        Nowsrv::handleRoleTriggers();

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

void Nowsrv::handleRoleTriggers() {
#if DEVICE____________LEFT == false
    if (touchRead(TOUCH______________PIN) > TOUCH________THRESHOLD) {
        Nowsrv::roleTriggerTouchCount++;
    }
#endif
    if (Device::getDeviceRole() != DEVICE_ROLE_____SEC) {          // may only check coefficient when not in SEC mode
        // make a role decision based on orientation threshold, only when not in SEC (must remain passive in SEC)
        if (Orientation::isAboveCoefPThreshold()) {                // should be PRI
            if (Device::getDeviceRole() != DEVICE_ROLE_____PRI) {  // but is not
                if (Device::setDeviceRole(DEVICE_ROLE_____PRI)) {  // set to pri accepted
                    Device::setCurrDterm(DTERM________GAMPM);      // will cause bitmaps to be sent, which will cause currDterm on the secondary device to be GAMPM as well
                }
            }
        } else if (Nowsrv::roleTriggerTouchCount > 3) {            // can only happen in the right device
            if (Device::getDeviceRole() != DEVICE_ROLE_____PRI) {  // but is not
                if (Device::setDeviceRole(DEVICE_ROLE_____PRI)) {  // set to pri accepted
                    Device::setCurrDterm(DTERM________GAMKN);      // will cause bitmaps to be sent, which will cause currDterm on the secondary device to be GAMPM as well
                }
            }
        } else {                                                   // should be ANY
            if (Device::getDeviceRole() != DEVICE_ROLE_____ANY) {  // but is not
                if (Device::setDeviceRole(DEVICE_ROLE_____ANY)) {  // set to any
                    Device::setCurrDterm(DTERM_________NONE);      // reset DTERM
                }
            }
        }
    }
}

void Nowsrv::handleIncomingDeviceRole(device_role___e incomingDeviceRole) {
    if (incomingDeviceRole == DEVICE_ROLE_____PRI && Device::getDeviceRole() != DEVICE_ROLE_____SEC) { // the other device has sent to be pri -> switch to sec if not already
        Device::setDeviceRole(DEVICE_ROLE_____SEC);
    }
    if (incomingDeviceRole == DEVICE_ROLE_____ANY && Device::getDeviceRole() == DEVICE_ROLE_____SEC) { // the other device has sent to be any -> switch to any if in sec
        Device::setDeviceRole(DEVICE_ROLE_____ANY);
    }
}