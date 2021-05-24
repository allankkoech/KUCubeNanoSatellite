#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

String serialBuf;

void setup() 
{
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

    while (!rf95.init()) 
	{
		Serial.println("LoRa radio init failed");
		while (1);
    }

    Serial.println("LoRa radio Transmitter init OK!");

    if (!rf95.setFrequency(RF95_FREQ)) 
	{
		Serial.println("LoRa Transmitter SetFrequency failed");
		while (1);
    }

    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
    
    rf95.setTxPower(23, false);

	
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{

	if(Serial.available()>0)
	{
		Serial.println("Sending to rf95_server");
		// Send a message to rf95_server

		serialBuf = Serial.readStringUntil('\n');

		sendPackets(serialBuf);

		// Now wait for a reply
		uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
		uint8_t len = sizeof(buf);

		Serial.print("RH_RF95_MAX_MESSAGE_LEN = "); Serial.println(RH_RF95_MAX_MESSAGE_LEN);

		Serial.println("Waiting for reply..."); delay(10);

		if (rf95.waitAvailableTimeout(1000))
		{ 
			// Should be a reply message for us now   
			if (rf95.recv(buf, &len))
			{
				Serial.print("Got reply: ");
				Serial.println((char*)buf);
				Serial.print("RSSI: ");
				Serial.println(rf95.lastRssi(), DEC);    
			}

			else
			{
				Serial.println("Receive failed");
			}
		}

		else
		{
			Serial.println("No reply, is there a listener around?");
		}
	}
}

void sendPackets(String message)
{
	// Transmit radio packets
	uint8_t data[message.length()];
	message.toCharArray(data, message.length());
	rf95.send(data, sizeof(data));
	rf95.waitPacketSent();
	Serial.println("Data Sent ...");
}