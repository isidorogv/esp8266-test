  // ESP8266 mastering I2C devices

  // Compatible with the Arduino IDE 1.6.4
  // Library https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
  // Bavensky :3

  // Some parts of this sketch have been taken from:
  //    http://bildr.org/2011/01/hmc6352/
  //    http://www.dfrobot.com/wiki/index.php/SRF02_Ultrasonic_sensor_%28SKU:SEN0005%29
  //    Bavensky's github page
  
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  #include <Adafruit_MLX90614.h>

  #define SRF02_ADDRESS         0x70
  #define HMC5883L_ADDR 0x1E //0011110b, I2C 7bit address of HMC5883
  
  LiquidCrystal_I2C lcd2004(0x27,20,4); 
  LiquidCrystal_I2C lcd1602(0x26,16,2); 
  
  uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};

  int HMC6352SlaveAddress = 0x42;
  int HMC6352ReadAddress = 0x41; //"A" in hex, A command is: 
  float headingIntValue;
  int reading = 0;
  bool haveHMC5883L = false;

  Adafruit_MLX90614 thermoir = Adafruit_MLX90614();
  
// ---- Aux. functions ----
  
  bool detectHMC5883L(){
    // read identification registers
    Wire.beginTransmission(HMC5883L_ADDR); //open communication with HMC5883
    Wire.write(10); //select Identification register A
    Wire.endTransmission();
    Wire.requestFrom(HMC5883L_ADDR, 3);
    if(3 == Wire.available()) {
      char a = Wire.read();
      char b = Wire.read();
      char c = Wire.read();
      if(a == 'H' && b == '4' && c == '3')
        return true;
    }  
    return false;
  }


  void getecho(){

    // step 1: instruct sensor to read echoes
    Wire.beginTransmission(112); // transmit to device #112 (0x70)
    // the address specified in the datasheet is 224 (0xE0)
    // but i2c adressing uses the high 7 bits so it's 112
    Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)
    Wire.write(byte(0x51));      // command sensor to measure in "centimeters" (0x51)
    // use 0x51 for centimeters
    // use 0x52 for ping microseconds
    Wire.endTransmission();      // stop transmitting

    // step 2: wait for readings to happen
    delay(70);                   // datasheet suggests at least 65 milliseconds

    // step 3: instruct sensor to return a particular echo reading
    Wire.beginTransmission(112); // transmit to device #112
    Wire.write(byte(0x02));      // sets register pointer to echo #1 register (0x02)
    Wire.endTransmission();      // stop transmitting

    // step 4: request reading from sensor
    Wire.requestFrom(112, 2);  // request 2 bytes from slave device #112

    // step 5: receive reading from sensor
    if (2 <= Wire.available()) // if two bytes were received
    {
      reading = Wire.read();   // receive high byte (overwrites previous reading)
      reading = reading << 8;  // shift high byte to be high 8 bits
      reading |= Wire.read();  // receive low byte as lower 8 bits
    }
  }

  void getheading(){

        //"Get Data. Compensate and Calculate New Heading"
    Wire.beginTransmission(HMC6352SlaveAddress);
    Wire.write(HMC6352ReadAddress);            // The "Get Data" command
    Wire.endTransmission();

    //time delays required by HMC6352 upon receipt of the command
    //Get Data. Compensate and Calculate New Heading : 6ms
    delay(6);

    Wire.requestFrom(HMC6352SlaveAddress, 2); //get the two data bytes, MSB and LSB

    //"The heading output data will be the value in tenths of degrees
    //from zero to 3599 and provided in binary format over the two bytes."
    byte MSB = Wire.read();
    byte LSB = Wire.read();

    float headingSum = (MSB << 8) + LSB; //(MSB / LSB sum)
    headingIntValue = headingSum / 10; 
  }

// ------ MAIN PROGRAM FLOW ------
  
  void setup()  {
    // "The Wire library uses 7 bit addresses throughout. 
    //If you have a datasheet or sample code that uses 8 bit address, 
    //you'll want to drop the low bit (i.e. shift the value one bit to the right), 
    //yielding an address between 0 and 127."
    HMC6352SlaveAddress = HMC6352SlaveAddress >> 1; // I know 0x42 is less than 127, 
                                                    // but this is still required

    Wire.begin(0, 2);
    Serial.begin(9600);          // start serial communication at 9600bps
    
    thermoir.begin();
    lcd2004.init();
    lcd2004.createChar(1, heart);
    lcd1602.init();
    lcd1602.backlight();
    lcd2004.backlight();

    lcd2004.home();
    lcd2004.print("Hello world! ");
    lcd2004.write(byte(1));
    lcd2004.setCursor(0, 1);
    lcd2004.print("  ESP-01 mastering");
    lcd2004.setCursor(0, 2);
    lcd2004.print(" two I2C LCDs ");
    lcd2004.setCursor(0, 3);
    lcd2004.print("Thanks, ChiangMaiMC");

    lcd1602.home();
    lcd1602.print("Here LCD1602!!");
//    lcd1602.setCursor(0, 1);
//    lcd1602.print("Counting: ");
//    lcd1602.print(millis() / 1000);

    delay(5000);                 // Wait 5 seconds and then start readings

    lcd2004.clear();
    lcd1602.clear();
  }


  void loop()  {

    getheading();
    
    lcd2004.clear();
    lcd2004.home();
    lcd2004.print("Ambient = "); 
    lcd2004.print(thermoir.readAmbientTempC());
    lcd2004.print("*C");
    lcd2004.setCursor(0, 1); 
    lcd2004.print("Object  = "); 
    lcd2004.print(thermoir.readObjectTempC()); 
    lcd2004.print("*C");
    //lcd2004.setCursor(0, 2);
    //lcd2004.print("Ambient = ");
    //lcd2004.print(thermoir.readAmbientTempF()); 
  //lcd2004.print("*F\tObject = "); 
  //lcd2004.print(thermoir.readObjectTempF()); 
  //Serial.println("*F");

  //Serial.println();
  delay(500);
  
    //lcd1602.clear();
    //lcd1602.home();
    //lcd1602.print("Distance: ");
    //lcd1602.print(reading);   // print the reading
    //lcd1602.print(" cm");

    lcd1602.setCursor(0, 1);
    lcd1602.print("Heading: ");
    lcd1602.print(headingIntValue);   // print the heading
    
    delay(250);               // wait a bit since people have to read the output :)
  }
  
// ----- END MAIN PROGRAM FLOW -----

