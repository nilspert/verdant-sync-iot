#include "constants.h"

const int SERIAL_BAUD_RATE = 115200;
const int LOOP_DELAY = 60000;
const int TEMPERATURE_RANDOM_MIN = 0;
const int TEMPERATURE_RANDOM_MAX = 100;
const int TEMPERATURE_DECIMAL_PLACES = 2;

const char* INFO = "INFO";
const char* WARNING = "WARNING";
const char* ERROR = "ERROR";
const char* DEVICE = "DEVICE";

const char* ADD_BOARD_INFO_SUCCESS_MESSAGE = "Board information added successfully.";
const char* ADD_BOARD_INFO_ERROR_MESSAGE = "Failed to add board information.";

const char* ADD_AUTHORIZED_DEVICE_SUCCESS_MESSAGE = "Device successfully added to authorized devices.";
const char* ADD_AUTHORIZED_DEVICE_PENDING_MESSAGE = "Device is pending authorization. Data sending disabled.";
const char* ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE = "Failed to add device to authorized devices";

const char* SEND_TEMPERATURE_SUCCESS_MESSAGE = "Temperature data sent successfully.";
const char* SEND_TEMPERATURE_ERROR_MESSAGE = "Failed to send temperature data.";
