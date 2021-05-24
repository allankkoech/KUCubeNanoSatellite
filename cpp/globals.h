#ifndef GLOBALS_H
#define GLOBALS_H

// sat (satellite) namespace
namespace sat {
// Intervals for checking for new data (sensor, camera, etc)
#define SYNC_INTERVAL_IN_MS 1000

// LoRa Transreceiver
#define TRANSRECEIVER_PORT_NAME "ttyUSB"
#define TRANSRECEIVER_USB_PORT ""

// GPS Serial Port
#define GPS_PORT_NAME "ttyACM"
#define GPS_USB_PORT ""
}

#endif // GLOBALS_H
