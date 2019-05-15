/****************************************************************************************	
**  This is example LINX firmware for use with the Arduino Uno with the serial 
**  interface enabled.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
**  BSD2 License.
****************************************************************************************/

//Include All Peripheral Libraries Used By LINX
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>

//includes for custom command
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3
#define TEMPERATURE_PRECISION 9 // 9 means 1°C, 10 means 0.5°C resolution, 11 means 0.25, 12 means 0.125 °C;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int numberOfDevices; // Number of temperature devices found
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

//Include Device Specific Header From Sketch>>Import Library (In This Case LinxChipkitMax32.h)
//Also Include Desired LINX Listener From Sketch>>Import Library (In This Case LinxSerialListener.h)
#include <LinxArduinoUno.h>
#include <LinxSerialListener.h>
 
//Create A Pointer To The LINX Device Object We Instantiate In Setup()
LinxArduinoUno* LinxDevice;

//Initialize LINX Device And Listener
void setup()
{
  //Instantiate The LINX Device
  LinxDevice = new LinxArduinoUno();
  
  //The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
  LinxSerialConnection.Start(LinxDevice, 0);

   //Custom Commands
   sensors.begin();
   numberOfDevices = sensors.getDeviceCount();
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
    {
    sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    }
  }  
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxSerialConnection.CheckForCommands();
  
  //Your Code Here, But It will Slow Down The Connection With LabVIEW
  LinxSerialConnection.AttachCustomCommand(0x01, led);
  LinxSerialConnection.AttachCustomCommand(0x02, DS18B20_gettemp);
  LinxSerialConnection.AttachCustomCommand(0x03, DS18B20_converttemp);
  LinxSerialConnection.AttachCustomCommand(0x05, DS18B20_getdevcount);
}

int led(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response){
  pinMode(13, OUTPUT);
  if (input[0]==1){
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
  else{
  digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
  };
  *numResponseBytes = 0;
return 0;
};

int DS18B20_converttemp(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response){
sensors.requestTemperatures(); // Send the command to get temperatures
response[0]= 0;
*numResponseBytes = 1;
return 0;
};


int DS18B20_gettemp(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response){
  sensors.requestTemperatures(); // Send the command to get temperatures
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){    // It responds almost immediately. Let's print out the data
      response[i]= sensors.getTempC(tempDeviceAddress); // getthe value float, x8 for .125 precision

    }; 
  };
*numResponseBytes = numberOfDevices;
return 0;
};


int DS18B20_getdevcount(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response){
  *numResponseBytes = 1;
return 0;
};
