/* DS18B20 (KY-001) [S019] : http://rdiot.tistory.com/45 [RDIoT DEMO] */

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
 
LiquidCrystal_I2C lcd(0x27,20,4);  // LCD2004
OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)
 
void setup()
{
  lcd.init();  // initialize the lcd 
  lcd.backlight();
  lcd.print("start LCD2004");
 
  delay(1000);
 
  lcd.clear();
  Serial.begin(9600);
}
 
void loop()
{
  lcd.setCursor(0,0);
  lcd.print("S019:DS18B20");
 
  String r = "";
  String d = "";
  String t = "";
  String f = "";
 
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
  for( i = 0; i < 8; i++) {
    //r += " ";
    r += String(addr[i], HEX);
  }
 
  //lcd.setCursor(0,1);
  //lcd.print("ROM=" + (String)r );
 
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  
  type_s = 0; //DS18B20
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
 
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    d += String(data[i], HEX);
  
  }
 
  lcd.setCursor(0,1);
  lcd.print("D="+(String)d );
 
  lcd.setCursor(0,2);
  lcd.print("CRC=" + String(OneWire::crc8(data, 8), HEX) );
 
 int16_t raw = (data[1] << 8) data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
 
  t = (String)celsius;
  f = (String)fahrenheit;
 
  lcd.setCursor(0,3);
  lcd.print("T="+t+" F="+f);
 
}
