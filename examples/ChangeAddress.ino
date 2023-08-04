#include <PZEM017v2.h>

/* Use software serial for the PZEM
 * Pin 4 Rx (Connects to the Tx pin on the PZEM)
 * Pin 5 Tx (Connects to the Rx pin on the PZEM)
*/
PZEM017v2 pzem(4, 5); //WMOS D1 4=D1, 5=D2
#define SET_ADDRESS 0x10  // Declare the address of device (
#define SET_SADDRESS 0x0001  // Declare your external shunt value for DC Meter. Default 0x0000 is 100A, replace to "0x0001" if using 50A shunt, 0x0002 is for 200A, 0x0003 is for 300A

void setup() {
  Serial.begin(115200);
}

void loop() {
     static uint8_t addr = SET_ADDRESS;
  
    if(!pzem017.setAddress(addr)) { 
      Serial.println("Pzem017 Error setting address.");
      if(!pzem017.setSAddress(SET_SADDRESS))   {  
        Serial.println("Pzem017 Error setting Saddress.");
        } 
    } 

    // Print out current custom address
    Serial.print("pzem current address:   0x");
    Serial.println(pzem.readAddress(), HEX);
    Serial.print("pzem current Saddress:   0x");
    Serial.println(pzem.readSAddress(), HEX);
  
    if(INCREMENT){
      addr++;
      if(addr >= PZEM_DEFAULT_ADDR)
        addr = 0x01;
    }
    Serial.println();
    delay(2000);
}

