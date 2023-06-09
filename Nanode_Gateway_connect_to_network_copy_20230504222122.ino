// Based on the poller code and  Simple demo for feeding some
// random data to Pachube. From <jc@wippler.nl>
// 2011-11-23 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
//
//
#include <EtherCard.h>
#include <JeeLib.h>
 
#define LOOP_DELAY 10000 // Milliseconds, time to wait between polling remote node
 
/*
  RF12 Communications
*/
#define RF12_GROUPID 212        // All nodes must be a member of the same group
#define RF12_NODEID_GATEWAY  1 // Each node within a group must have a unique ID
#define RF12_NODEID_REMOTE   2
#define RF12_WAIT 15 //ms Time to wait for Remote node to respond to a data request
 
// structure of data returned by each Remote node. Gateway and Remote must use same structure
typedef struct {
  byte node;
  long time;
  float temperature;
} Payload;
 
// Information required to connect to Xively account
// change these settings to match your own se
char website[] = "https://app.datacake.de";

#define FEED    "399733688"   // Put Your Feed ID here
#define APIKEY  "Ilxcnw2fQK8xyUX4S1VXQAbykGjOKPWY7xStWP0ghbSAMIt3" // Put Your APIKEY here
 
// Network interface setup info
// ethernet interface mac address, must be unique on your LAN
byte mymac[] = { 0xB4, 0xB5, 0x2F, 0x77, 0xED, 0x7B };

byte Ethernet::buffer[700];
Stash stash;
 
void sendXively(char *channel, int value){
   // by using a separate stash,
   // we can determine the size of the generated message ahead of time
   byte sd = stash.create();
 
   stash.print(channel);
   stash.print(",");
   stash.println(value);
   stash.save();
 
   // generate the header with payload - note that the stash size is used,
   // and that a "stash descriptor" is passed in as argument using "$H"
   Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
                    "Host: $F" "\r\n"
                    "X-PachubeApiKey: $F" "\r\n"
                    "Content-Length: $D" "\r\n"
                    "\r\n"
                    "$H"),
            website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);
 
   // send the packet - this also releases all stash buffers once done
   ether.tcpSend();
}
 
void setup () {
//Setup serial port for Arduino IDE monitor function (USB)
  Serial.begin(57600);
  Serial.println("nGateway");
 
//Setup RFM12B Radio for Gateway/Remote Communications
  rf12_initialize(RF12_NODEID_GATEWAY, RF12_433MHZ, RF12_GROUPID);
 
//Setup Network connection
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println( "Failed to access Ethernet controller");
 
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
 
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
 
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
 
  ether.printIp("SRV: ", ether.hisip);
 
}
 
MilliTimer timer;
uint32_t looptimer = 0;
 
void loop () {
    ether.packetLoop(ether.packetReceive());
 
    if (millis() > looptimer) {
      looptimer = millis() + LOOP_DELAY;
 
      // send an empty packet to Nanode Remote
      rf12_sendNow(RF12_HDR_ACK | RF12_HDR_DST |RF12_NODEID_REMOTE, 0, 0);
 
      // wait up to RF12_WAIT milliseconds for a reply from remote node
      timer.set(RF12_WAIT);
      while (!timer.poll())
        if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof (Payload)) {
          // got a good ACK packet, print out its contents
 
          // Data from RFM12B returns in rf12_data
          const Payload* p = (const Payload*) rf12_data;
 
          Serial.print("Remote Temperature - ");
          Serial.println(p->temperature);
 
          sendXively("Shed-Temperature",p->temperature);
          break;
        }
    }
}