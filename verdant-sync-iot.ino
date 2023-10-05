/**
 * File: verdant-sync-iot.ino
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file is the main Arduino sketch file for this project.
 * Serves as entry point and runs DeviceManager which contains main program.
 */

#include "src/device_manager/device_manager.h"

DeviceManager deviceManager;

void setup() {
    // Initialize the DeviceManager
    deviceManager.setup();
}

void loop() {
    // Continuously run DeviceManager loop, which manages device operations
    deviceManager.loop();
}
