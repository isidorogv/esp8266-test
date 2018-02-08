  // ESP8266 mastering I2C devices

  // Compatible with the Arduino IDE 1.6.4 or higher
  // Library https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
  // Bavensky :3

  // Some parts of this sketch have been taken from:
  //    http://www.dfrobot.com/wiki/index.php/SRF02_Ultrasonic_sensor_%28SKU:SEN0005%29
  //    Bavensky's github page
  //    https://github.com/adafruit/Adafruit_HMC5883_Unified
  //    https://github.com/adafruit/Adafruit-MLX90614-Library
  //    https://github.com/adafruit/RTClib
  
  #include <Wire.h>                 // Arduino standar lib for serial com
  #include <LiquidCrystal_I2C.h>    // I2C LCD Library
  #include <Adafruit_MLX90614.h>    // Lib for IR thermometer GY-906
  #include <Adafruit_Sensor.h>      // Needed lib for comunication with some sensors
  #include <Adafruit_HMC5883_U.h>   // Lib for digital compass module HMC5883
  #include <RTClib.h>               // I2C Real Time Clock module based on DS1307 chipset
  
  #define SRF02_ADDRESS 0x70        // Ultrasonic SRF02 module address

  LiquidCrystal_I2C lcd2004(0x27,20,4);   // 20x4 LCD single model (no backlight)
  LiquidCrystal_I2C lcd1602(0x26,16,2);   // 16x2 backlighted LCD model
  LiquidCrystal_I2C lcd2004B(0x24,20,4);  // 20x4 backlighted LCD model
  
  // some pics for fun
  uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
 
  // variable for IR thermometer GY-906/MLX90614
  Adafruit_MLX90614 thermoir = Adafruit_MLX90614();

  // Assign a unique ID to this sensor at the same time
  Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

  // Instance and variable for RTC/DS1307 
  RTC_DS1307 rtc;
  char daysOfTheWeek[7][5] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

  // ---- Aux. functions ----

  int getecho(){
    
    int echoreading = 0;
 
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
      echoreading = Wire.read();   // receive high byte (overwrites previous reading)
      echoreading = echoreading << 8;  // shift high byte to be high 8 bits
      echoreading |= Wire.read();  // receive low byte as lower 8 bits
    }
    return echoreading;
  }

// ------ MAIN PROGRAM FLOW ------
  
  void setup()  {

    Wire.begin(0, 2);
   
    mag.begin();
    thermoir.begin();
    lcd2004.init();
    lcd2004.createChar(1, heart);
    lcd1602.init();
    lcd1602.backlight();
    lcd2004B.init();
    lcd2004B.backlight();

    lcd2004.home();
    lcd2004.print("Hello world! ");
    lcd2004.write(byte(1));
    lcd2004.setCursor(0, 1);
    lcd2004.print("  ESP-01 mastering");
    lcd2004.setCursor(0, 2);
    lcd2004.print(" many I2C devices ");
    lcd2004.setCursor(0, 3);
    lcd2004.print("Thanks for using!");

    lcd1602.home();
    lcd1602.print("Here LCD1602!!");

    lcd2004B.home();
    lcd2004B.print("Let's Goooooooo! ");
    
    /* Initialise the digital compass */
    if(!mag.begin()){
      // There was a problem detecting the HMC5883 ... check your connections/
      lcd2004B.print("Ooops, no HMC5883 detected...");
      lcd2004B.setCursor(0,1);
      lcd2004B.print("Check your wiring!");
      while(1);
    }
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call: 
    //rtc.adjust(DateTime(2017, 1, 15, 17, 51, 0));
    
    delay(5000);                 // Wait 5 seconds and then start readings
  }


  void loop()  {
    
    lcd2004.home();
    lcd2004.clear();
    lcd2004.print("Distance: ");
    lcd2004.print(getecho());   // show distance reading
    lcd2004.print(" cm");

    lcd2004.setCursor(0, 2); 
    lcd2004.print("TAmbient: "); 
    lcd2004.print(thermoir.readAmbientTempC());
    lcd2004.print("*C");
    lcd2004.setCursor(0, 3); 
    lcd2004.print("TObject: "); 
    lcd2004.print(thermoir.readObjectTempC()); 
    lcd2004.print("*C");

    lcd2004B.home();
    lcd2004B.clear();

    /* Get a new sensor/compass event */ 
    sensors_event_t event; 
    mag.getEvent(&event);
   
    /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
    lcd2004B.print("X: "); 
    lcd2004B.print(event.magnetic.x);
    lcd2004B.print(" uT "); 
    lcd2004B.setCursor(0,1);
    lcd2004B.print("Y: "); 
    lcd2004B.print(event.magnetic.y);
    lcd2004B.print(" uT "); 
    lcd2004B.setCursor(0,2);
    lcd2004B.print("Z: "); 
    lcd2004B.print(event.magnetic.z);
    lcd2004B.print(" uT "); 
  
    // Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
    // Calculate heading when the magnetometer is level, then correct for signs of axis.
    float heading = atan2(event.magnetic.y, event.magnetic.x);
    
    // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
    // Find yours here: http://www.magnetic-declination.com/
    // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
    // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
    float declinationAngle = 0.22;
    heading += declinationAngle;
    
    // Correct for when signs are reversed.
    if(heading < 0)
      heading += 2*PI;
      
    // Check for wrap due to addition of declination.
    if(heading > 2*PI)
      heading -= 2*PI;
     
    // Convert radians to degrees for readability.
    float headingDegrees = heading * 180/M_PI; 
  
    lcd2004B.setCursor(0,3);
    lcd2004B.print("Heading(deg): "); 
    lcd2004B.print(headingDegrees);

    DateTime now = rtc.now();

    lcd1602.clear();
    lcd1602.home();

    lcd1602.print("(");
    lcd1602.print(daysOfTheWeek[now.dayOfTheWeek()]);
    lcd1602.print(") ");
    lcd1602.print(now.day(), DEC);
    lcd1602.print('/');
    lcd1602.print(now.month(), DEC);
    lcd1602.print('/');
    lcd1602.print(now.year(), DEC);
    lcd1602.setCursor(0,1);
    lcd1602.print("    ");
    lcd1602.print(now.hour(), DEC);
    lcd1602.print(':');
    lcd1602.print(now.minute(), DEC);
    lcd1602.print(':');
    lcd1602.print(now.second(), DEC);
    
    delay(1000);               // wait a bit since people have to read the output :)
  }
  
// ----- END MAIN PROGRAM FLOW -----

