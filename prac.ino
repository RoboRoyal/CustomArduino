
//Dakota Abernathy

#include <FRAM_MB85RC_I2C.h> ////https://github.com/sosandroid/FRAM_MB85RC_I2C\\\\
#include <Wire.h>

#define FLASH 80
#define FLASH_SIZE 16 * 1024
#define EEPROM_SIZE 16 * 1024
#define BUTTON_0 2//pin  number
#define SWITCH_0 7

#define mem_0_addr 0x80
#define mem_1_addr 0x81

#define LED_RED    2
#define LED_GREEN  3
#define LED_BLUE   4

FRAM_MB85RC_I2C mem_0;
FRAM_MB85RC_I2C mem_1;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial); //wait for serial
  //init FRAM
  //init EEPROM
}

void error_out(byte code){
  switch(code){
    case 1:
    Serial.println(" data too long to fit in transmit buffer ");
    break;
    case 2:
    Serial.println(" received NACK on transmit of address ");
    break;
    case 3:
    Serial.println(" received NACK on transmit of data ");
    break;
    case 4:
    Serial.println(" Serial not accessible ");
    break;
    case 5:
    Serial.println(" Not referenced device ID ");
    break;
  //no 6
    case 7:
    Serial.println(" Fram chip unidentified ");
    break;
    case 8:
    Serial.println(" number of bytes asked to read null ");
    break;
    case 9:
    Serial.println(" bit position out of range ");
    break;
    case 10:
    Serial.println(" Not permitted operation ");
    break;
    case 11:
    Serial.println(" Out of memory range operation ");
    break;
    defualt:
    Serial.println(" unknown error ");
    break;
  }
}

void waitFor(const int pin){
  while (digitalRead(pin) == LOW) {
    delay(20);
  }
}
void randData(const int length, byte data[]){
  for(int i = 0; i < length; i++){
    data[i] = random(255);
  }
}
void writeFlash_help(const int start, const int num_bytes, byte * data, FRAM_MB85RC_I2C device){
  byte result = device.writeArray(start, num_bytes, data);
  if (result != 0){
    Serial.print("Write failed.");
    error_out(result);
  }
}

void writeFlash(const int start, const int num_bytes, byte data[]){
  if(start > FLASH_SIZE){
    writeFlash_help(start, num_bytes, data, mem_1);
  }else if(start + num_bytes < FLASH_SIZE){
    writeFlash_help(start, num_bytes, data, mem_0);
  }else{
    writeFlash_help(start, FLASH_SIZE - start, data, mem_0);
    writeFlash_help(start, FLASH_SIZE - start, &data[num_bytes - FLASH_SIZE + start], mem_1);
  }
}
void readFlash_help(const int start, const int num_bytes, byte * data, FRAM_MB85RC_I2C device){
  byte result = device.readArray(start, num_bytes, data);
  if (result != 0){
    Serial.print("Read failed.");
    error_out(result);
  }
}
void readFlash(const int start, const int num_bytes, byte data[]){
  if(start > FLASH_SIZE){
    readFlash_help(start, num_bytes, data, mem_1);
  }else if(start + num_bytes < FLASH_SIZE){
    readFlash_help(start, num_bytes, data, mem_0);
  }else{
    readFlash_help(start, FLASH_SIZE - start, data, mem_0);
    readFlash_help(start, FLASH_SIZE - start, &data[num_bytes - FLASH_SIZE + start], mem_1);
  }
}

void readEEPROM(const int start, const int num_bytes, byte data[]){
  byte result = eeprom.readArray(start, num_bytes, data);
  if (result != 0){
    Serial.print("Read failed.");
    error_out(result);
  }
}

void testEEPROM(){
    byte data[100];
    readEEPROM(100, 100, data);
    for(int i = 0; i < 100; i++){
      if(i != data[i]){
        Serial.print("ERROR! on test EEPROM");
      }
    }
    Serial.print("EEPROM test done");
};
void testLEDs(){
  digitalWrite(LED_RED, HIGH);
  delay(100);
  digitalWrite(LED_GREEN, HIGH);
  delay(100);
  digitalWrite(LED_BLUE, HIGH);
  delay(100);
  digitalWrite(LED_RED, LOW);
  delay(100);
  digitalWrite(LED_GREEN, LOW);
  delay(100);
  digitalWrite(LED_BLUE, LOW);
};


bool testFlash(const int number_of_bytes){
    Serial.println("Now testing I2C Flash...");
    //make array of length 'number_of_bytes' full of randomw data
    byte data[number_of_bytes];
    randData(number_of_bytes, data);
    //write it to flash
    Serial.print("Wriing ");
    Serial.print(number_of_bytes);
    Serial.println(" bytes");
    /*WRITE BYTES*/
    writeFlash(0, number_of_bytes, data);
    //read it back
    Serial.println("Reading data back");
    byte read_back_data[number_of_bytes];
    readFlash(0,number_of_bytes, read_back_data);
    Serial.println("Got data back");
    //compair
    Serial.println("GCompairing data");
    int diffrent = 0;
    for(int i = 0; i < number_of_bytes; i++){
      if(data[i] != read_back_data[i])
        diffrent++;
    }
  
}

void loop() {
  Serial.print("Start up Successful!");

  //test buttons
  Serial.print("Waiting for button press...");
  waitFor(BUTTON_0);
  Serial.print("Button pressed");
  Serial.print("Waiting for switch to toggle...");
  waitFor(SWITCH_0);
  Serial.print("Button pressed");
  
  testFlash(1024);
  testEEPROM();
  testLEDs();
  Serial.print("All tests complete");
}
