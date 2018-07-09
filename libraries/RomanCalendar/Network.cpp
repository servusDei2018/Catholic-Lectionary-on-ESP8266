#include "Network.h"
extern "C" {
#include "user_interface.h"
}

String Network::getDHCPAddressAsString() {
  char myIpString[24];
  IPAddress myIp = WiFi.localIP();
  sprintf(myIpString, "%d.%d.%d.%d", myIp[0], myIp[1], myIp[2], myIp[3]);
  return String(myIpString);
}

bool Network::connect() {
	String storedSSID = WiFi.SSID();
	String storedPSK  = WiFi.psk();
	
	I2CSerial.print(F("Stored SSID = "));
	I2CSerial.println(storedSSID);
	
	if (storedSSID == "") {
		I2CSerial.println(F("No SSID stored"));
		return false;
	}

	WiFi.persistent(false);
	
	wl_status_t status = WiFi.status();

	if(status == WL_CONNECTED) {
		WiFi.disconnect(false);
		delay(1000);
	} 
	
	I2CSerial.print(F("\nTry connecting to WiFi with SSID '")); I2CSerial.print(storedSSID); I2CSerial.println(F("'"));

	WiFi.mode(WIFI_STA);
	delay(1000);
	
	WiFi.begin(storedSSID.c_str(), storedPSK.c_str()); // reading data from EEPROM, last saved credentials
	int timeout = 60;
	while (WiFi.status() != WL_CONNECTED && timeout != 0) {
		delay(500);
		I2CSerial.print(".");
		timeout--;
	}
	
	status = WiFi.status();
	if(status == WL_CONNECTED) {
		I2CSerial.print ( F("Connected to ") );
		I2CSerial.println ( WiFi.SSID() );
		I2CSerial.print ( F("IP address: ") );
		I2CSerial.println ( WiFi.localIP() );
		//I2CSerial.print(F("\nConnected successfully to SSID '")); I2CSerial.print(WiFi.SSID()); I2CSerial.println(F("'"));
	} 
	else {
		I2CSerial.print(F("\nCould not connect to WiFi. state=")); I2CSerial.println(String(status));
		WiFi.printDiag(I2CSerial);
		return false;
		
		//I2CSerial.println("Please press WPS button on your router.\n Press any key to continue...");
		//bool connected = false;
		//wdt_reset();
		//delay(10000);
		//connected = startWPSPBC();			
		//
		//if (!connected) {
		//	I2CSerial.println("Failed to connect with WPS :-(");  
		//	return false;
	}
	
	return true;
}

bool Network::startWPSPBC() {
  //WiFi.persistent(true);
  I2CSerial.println(F("WPS config start"));
  bool wpsSuccess = WiFi.beginWPSConfig();
  WiFi.mode(WIFI_STA);
  delay(3000);
  if(wpsSuccess) {
	  // Well this means not always success :-/ in case of a timeout we have an empty ssid
      String newSSID = WiFi.SSID();
	  String newPSK = WiFi.psk();
      if(newSSID.length() > 0) {
        // WPSConfig has already connected in STA mode successfully to the new station. 
        I2CSerial.print(F("WPS finished. Connected successfull to SSID '")); I2CSerial.print(newSSID); I2CSerial.println(F("'"));
		
		wifi_set_opmode_current(STATION_MODE);
		struct station_config stationConfig = {0};
		strncpy((char*)stationConfig.ssid, newSSID.c_str(), 32);
		strncpy((char*)stationConfig.password, newPSK.c_str(), 64);
		wifi_station_set_config(&stationConfig);		
      } else {
        wpsSuccess = false;
      }
  }
  return wpsSuccess; 
}

bool Network::get_ntp_time(time64_t* t)
{
	wl_status_t status = WiFi.status();
	if(status != WL_CONNECTED) {
		return false;
    }
	
	udp.begin(localPort);                          // Start listening for UDP messages on port 123
	//get a random server from the pool
	WiFi.hostByName(ntpServerName, timeServerIP); 

	I2CSerial.println(timeServerIP);

	sendNTPpacket(timeServerIP); // send an NTP packet to a time server
	// wait to see if a reply is available

	int cb;
	delay(1000);
	cb = udp.parsePacket();
	if (!cb) return false;

	I2CSerial.print(F("packet received, length="));
	I2CSerial.println(cb);
	// We've received a packet, read the data from it
	udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

	//the timestamp starts at byte 40 of the received packet and is four bytes,
	// or two words, long. First, esxtract the two words:

	unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
	unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
	// combine the four bytes (two words) into a long integer
	// this is NTP time (seconds since Jan 1 1900):
	unsigned long secsSince1900 = highWord << 16 | lowWord;
	I2CSerial.print(F("Seconds since Jan 1 1900 = " ));
	I2CSerial.println(secsSince1900);

	// now convert NTP time into everyday time:
	I2CSerial.print("Unix time = ");
	// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
	const unsigned long seventyYears = 2208988800UL;
	// subtract seventy years:
	unsigned long epoch = secsSince1900 - seventyYears;
	// print Unix time:
	I2CSerial.println(epoch);
	*t = (time64_t)epoch;

	// print the hour, minute and second:
	I2CSerial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
	I2CSerial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
	I2CSerial.print(':');
	if ( ((epoch % 3600) / 60) < 10 ) {
	// In the first 10 minutes of each hour, we'll want a leading '0'
	I2CSerial.print('0');
	}
	I2CSerial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
	I2CSerial.print(':');
	if ( (epoch % 60) < 10 ) {
	// In the first 10 seconds of each minute, we'll want a leading '0'
	I2CSerial.print('0');
	}
	I2CSerial.println(epoch % 60); // print the second

	udp.stop();
	
	return true;
}

// send an NTP request to the time server at the given address
unsigned long Network::sendNTPpacket(IPAddress& address)
{
  I2CSerial.println(F("sending NTP packet..."));
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void Network::wifi_sleep( void ) {
	//ESP8266------
	//WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
	WiFi.forceSleepBegin();
	delay(1);
	//-------------
}