#include <SPI.h>
#include <RH_RF95.h>
 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
 
#define RF95_FREQ 433.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
// Blinky on receipt
#define LED 13
 
void setup() 
{
    pinMode(LED, OUTPUT);     
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);
    
    while (!Serial);
    Serial.begin(9600);
    delay(100);
    
    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
    
    while (!rf95.init()) {
        Serial.println("LoRa radio init failed");
        while (1);
    }
    Serial.println("LoRa radio init OK!");
    
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("setFrequency failed");
        while (1);
    }

    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
    
    rf95.setTxPower(23, false);
}
 
void loop()
{
    if (rf95.available())
    {
        // Should be a message for us now   
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        
        if (rf95.recv(buf, &len))
        {
            digitalWrite(LED, HIGH);
            RH_RF95::printBuffer("Received: ", buf, len);
            Serial.print("Got: ");
            Serial.println((char*)buf);
            Serial.print("RSSI: ");
            Serial.println(rf95.lastRssi(), DEC);
            
            // Send a reply
            char x[] = "And hello back to you";
            uint8_t *data = (uint8_t *)x;
            rf95.send(data, sizeof(data));
            rf95.waitPacketSent();
            Serial.println("Sent a reply");
            digitalWrite(LED, LOW);
        }

        else
        {
            Serial.println("Receive failed");
        }
    }
}