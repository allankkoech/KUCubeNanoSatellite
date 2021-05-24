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

	int p = 0;
	
	if(message.length()%12==0)
		p = message.length()/12;
	
	else
		p = message.length()/12+1;
	
	char pkts[24];
	sprintf(pkts, "No of packets to send = %d", p);
	Serial.println(pkts);
	
	char radiopacket[20] = "                   ";
	int i = 0; // index for packet slices

	while(p > 0)
	{
		String _buf = message.substring(i, i+12);

		// Make buffer size 19 characters long
		while(_buf.length() < 19) { _buf += " "; }

		_buf.toCharArray(radiopacket, 20);
		radiopacket[12] = '#';

		Serial.println(radiopacket);

		itoa(packetnum++, radiopacket+13, 10);
		Serial.print(">> Sending: "); Serial.println(radiopacket);
		radiopacket[19] = 0;
		
		delay(2);
		rf95.send((uint8_t *)radiopacket, 20);

		Serial.println("Waiting for packet to complete..."); delay(10);
		rf95.waitPacketSent();

		i+=12; // increment Packet Counter
		p -= 1; // Decrement packet number
	}
}