#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "R2D2Shared.h"

const int LED_PIN = 2; // Pin for the LED

DomeCommand latestCmd = { 0, 0, 0 };

uint32_t lastSeq = 0;
unsigned long lastPacketTime = 0;
unsigned long lastBlinkTime = 0;
bool blinkState = false;

uint32_t packetsReceived = 0;
uint32_t packetsMissed = 0;
unsigned long lastStatsTime = 0;

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {

    DomeCommand cmd;
    memcpy(&cmd, data, sizeof(cmd));

    if (cmd.seq != lastSeq) {
        if(cmd.seq > lastSeq + 1) {
            packetsMissed += (cmd.seq - lastSeq - 1);
        }
        latestCmd = cmd;
        lastSeq = cmd.seq;
        lastPacketTime = millis();
        packetsReceived++;
    }
}

void setup(){
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    Serial.println("Dome Node Starting...");
    digitalWrite(LED_PIN, LOW);

    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onDataRecv);
}

void loop() {
    unsigned long now = millis();

    if(now - lastPacketTime > 500){// safety cutoff
        analogWrite(LED_PIN, 0);
    } else {
        int brightness = map(abs(latestCmd.domeSpeed), 0, 512, 0, 255);

        if(latestCmd.buttons & 0x01) { //X button: fast blink
            if(now - lastBlinkTime > 200){
                blinkState = !blinkState;
                lastBlinkTime = now;
            }
            analogWrite(LED_PIN, blinkState ? brightness : 0);
        } else if(latestCmd.buttons & 0x02) { //Y button: slow blink
            if(now - lastBlinkTime > 500){
                blinkState = !blinkState;
                lastBlinkTime = now;
            }
            analogWrite(LED_PIN, blinkState ? brightness : 0);
        } else {
            analogWrite(LED_PIN, brightness);
        }
    }

    if (now - lastStatsTime > 5000) {
        lastStatsTime = now;
        Serial.printf("[Dome] Uptime: %lus | Packets: %lu | Missed: %lu | Free Heap: %u\n",
                      now/1000, packetsReceived, packetsMissed, ESP.getFreeHeap());
    }

    // Prevent watchdog timeout
    delay(10);
}
