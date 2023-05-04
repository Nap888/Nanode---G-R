//Start of Gateway code
/// Based on the polleer code from <jc@wippler.nl>;
// 2011-11-23 <jc@wippler.nl>; http://opensource.org/licenses/mit-license.php
//
//
#include <JeeLib.h>;
 
#define LOOP_DELAY 5000 // Milliseconds
/*
  RF12 Communications
*/
#define RF12_GROUPID 212    // all nodes must be a member of the same group
                          // to communicate with each other
#define RF12_NODEID_GATEWAY  1      // Each node within a group must have a unique ID
#define RF12_NODEID_REMOTE 2
 
MilliTimer timer;
 
typedef struct {
  byte node;
  long time;
  float temp;
} Payload;
 
void setup () {
  Serial.begin(57600);
  Serial.println("Gateway");
 
  rf12_initialize(RF12_NODEID_GATEWAY, RF12_433MHZ, RF12_GROUPID);
}
 
void loop () {
  // send an empty packet to Nanode Remote
  rf12_sendNow(RF12_HDR_ACK | RF12_HDR_DST | RF12_NODEID_REMOTE, 0, 0);
 
  // wait up to 10 milliseconds for a reply
  timer.set(10);
  while (!timer.poll())
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof (Payload)) {
      // got a good ACK packet, print out its contents
 
      // Data from RFM12B returns in rf12_data
      const Payload* p = (const Payload*) rf12_data;
      Serial.print("Node: ");
      Serial.print((word) p->node);
      Serial.print(": ");
      Serial.println(p->temp);
      break;
    }
  delay(LOOP_DELAY);
}
//End of Gateway code