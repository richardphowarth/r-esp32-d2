#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Bluepad32.h>
#include "R2D2Shared.h"

// Replace with your MAC addresses

uint8_t motorNode[] = { 0x94, 0x54, 0xC5, 0x63, 0x58, 0xA0 };
uint8_t domeNode[] = { 0x8c, 0x4b, 0x14, 0x04, 0x66, 0xc0 };

GamepadPtr myGamepad;

const int LED_PIN = 2; // Pin for the LED

uint32_t seqNum = 0;
uint32_t packetsSent = 0;
unsigned long lastsStatsTime = 0;

int16_t applyDeadzone(int16_t value, int16_t deadzone = 40) {
    return (abs(value) < deadzone) ? 0 : value;
}


void onConnectedGamepad(GamepadPtr gp) {
    myGamepad = gp;
    digitalWrite(LED_PIN, HIGH);
    Serial.printf("Gamepad connected: %s\n", myGamepad->getModelName().c_str());
}

void onDisconnectedGamepad(GamepadPtr gp) {
    Serial.printf("Gamepad disconnected: %s\n", gp->getModelName().c_str());
    digitalWrite(LED_PIN, LOW);
    myGamepad = nullptr;
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if(status != ESP_NOW_SEND_SUCCESS) {
        Serial.printf("Data send failed, status: %d\n", status);
    }
}

void setup(){
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    WiFi.mode(WIFI_STA);

    if(esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0; // Use the current channel
    peerInfo.encrypt = false;

    memcpy(peerInfo.peer_addr, motorNode, 6);
    esp_now_add_peer(&peerInfo);
    memcpy(peerInfo.peer_addr, domeNode, 6);
    esp_now_add_peer(&peerInfo);

    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BP32.forgetBluetoothKeys();
}

void loop(){
    BP32.update();
    seqNum++;

    DriveCommand driveCmd = {};
    DomeCommand domeCmd = {};

    if(myGamepad && myGamepad->isConnected()) {
        driveCmd.driveX = applyDeadzone(myGamepad->axisX());
        driveCmd.driveY = applyDeadzone(myGamepad->axisY());
        driveCmd.buttons = (myGamepad->a() << 0) | (myGamepad->b() << 1);
        driveCmd.seq = seqNum;

        domeCmd.domeSpeed = applyDeadzone(myGamepad->axisRX());
        domeCmd.buttons = (myGamepad->a() << 0) | (myGamepad->b() << 1);
        domeCmd.seq = seqNum;
    } else {
        driveCmd.driveX = 0;
        driveCmd.driveY = 0;
        driveCmd.buttons = 0;
        driveCmd.seq = seqNum;

        domeCmd.domeSpeed = 0;
        domeCmd.buttons = 0;
        domeCmd.seq = seqNum;
    }

    esp_now_send(motorNode, (uint8_t*)&driveCmd, sizeof(driveCmd));
    esp_now_send(domeNode, (uint8_t*)&domeCmd, sizeof(domeCmd));
    packetsSent++;

    if(millis() - lastsStatsTime > 5000){
        lastsStatsTime = millis();
        Serial.printf("[Controller] Uptime: %lus | Packets Sent: %lu | Free Heap: %u\n",
                      millis()/1000, packetsSent, ESP.getFreeHeap());
    }

    delay(20); // Adjust delay as needed for your application
}