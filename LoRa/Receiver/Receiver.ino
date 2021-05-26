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

            char d[len];
            String val = "";
            uint8_t ind = 0;

            Serial.println("\nRECEIVED DATA:");

            for(int i=0; i<len; i++)
            {
                d[i] = (char)buf[i];

                if(d[i]==':')
                {
                    if(ind==0)
                    {
                        Serial.print("\tTimestamp      : "); Serial.println(val);
                    }

                    if(ind==1)
                    {
                        Serial.print("\tLatitude       : "); Serial.print(val); Serial.println(" N");
                    }

                    if(ind==2)
                    {
                        Serial.print("\tLongitude      : "); Serial.print(val); Serial.println(" E");
                    }

                    if(ind==3)
                    {
                        Serial.print("\tCPU Temperature: "); Serial.print(val); Serial.println(" °C");
                    }

                    if(ind==4)
                    {
                        Serial.print("\tHumidity       : "); Serial.print(val); Serial.println(" %");
                    }

                    val="";
                    ind++;
                }

                else
                {
                    val += d[i];
                }
            }

            Serial.println("\n");
            
            // Send a reply
            uint8_t data[] = "OK";
            rf95.send(data, sizeof(data));
            rf95.waitPacketSent();
            digitalWrite(LED, LOW);
        }

        else
        {
            Serial.println("Receive failed");
        }
    }
}