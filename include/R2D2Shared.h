#ifndef R2D2_SHARED_H
#define R2D2_SHARED_H

#include <Arduino.h>

struct DriveCommand {
    int16_t driveX;  // Left drive motor speed
    int16_t driveY; // Right drive motor speed
    uint16_t buttons;
    uint32_t seq;
};

struct DomeCommand {
    int16_t domeSpeed;  // Dome rotation speed
    uint16_t buttons;
    uint32_t seq;
};

#endif // R2D2_SHARED_H