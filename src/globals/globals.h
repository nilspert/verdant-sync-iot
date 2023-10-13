/**
 * File: globals.h
 * Author: Joonas Nislin
 * Date: 13.10.2023
 * Description: Header file for global module instances
 * This file declares external instances of global modules, allowing access from multiple files.
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../firebase_module/firebase_module.h"
#include "../aes_module/aes_module.h"
#include "../../config/config.h"
#include "../event_module/event_module.h"
#include "../api_manager/api_manager.h"

// Global definitions
#define MINIMUM_WATER_TANK_LEVEL 12.5

// Declare the global instance for EventModule
extern EventModule eventModule;

// Declare the global instance for ApiManager
extern ApiManager apiManager;

// Handle an event with severity, message and event type
extern void handleEvent(const char* severity, const char* message, const char* eventType);

#endif