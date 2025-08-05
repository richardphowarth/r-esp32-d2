#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "R2D2Shared.h"

const int LED_PIN = 2; // Pin for the LED

DriveCommand latestDriveCmd = {0, 0, 0, 0};
uint32_t lastSeq = 0;

unsigned long lastPacketTime = 0;
unsigned long lastBlinkTime = 0;

bool blinkState = false;

uint32_t packetsReceived = 0;
uint32_t packetsMissed = 0;
unsigned long lastStatsTime = 0;

void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
    DriveCommand cmd;
    memcpy(&cmd, data, sizeof(cmd));

    if(cmd.seq != lastSeq){
        if(cmd.seq > lastSeq + 1) {
            packetsMissed += (cmd.seq - lastSeq - 1);
        }
        latestDriveCmd = cmd;
        lastSeq = cmd.seq;
        lastPacketTime = millis();
        packetsReceived++;
    }
}

void setup(){
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    WiFi.mode(WIFI_STA);

    if(esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed");
        return;
    }

    esp_now_register_recv_cb(onDataRecv);
}

void loop(){
    unsigned long now = millis();

    //failsafe: stop if no packet received in 500ms

    if(now - lastPacketTime > 500) {
            latestDriveCmd.driveX = 0;
            latestDriveCmd.driveY = 0;
            latestDriveCmd.buttons = 0;
            latestDriveCmd.seq = lastSeq + 1; // Increment sequence number
    } else {
        int speed = abs(latestDriveCmd.driveY);
        int brightness = map(speed, 0, 512, 0, 255);
        if(latestDriveCmd.driveY < 0){
            analogWrite(LED_PIN, brightness); // forward solid led
        } else if(latestDriveCmd.driveY > 0){
            if(now - lastBlinkTime > 300) {
                blinkState = !blinkState;
                lastBlinkTime = now;
            }
            analogWrite(LED_PIN, blinkState ? brightness : 0); // backward blinking led
        } else {
            analogWrite(LED_PIN, 0); // stop led off
        }
    }

    if (now - lastStatsTime > 5000) {
        lastStatsTime = now;
        Serial.printf("[Drive] Uptime: %lus | Packets: %lu | Missed: %lu | Free Heap: %u\n",
                      now/1000, packetsReceived, packetsMissed, ESP.getFreeHeap());
    }

    delay(10); // Prevents watchdog timeout
}
