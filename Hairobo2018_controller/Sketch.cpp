/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

#include <SPI.h>
#include <mcp_can.h>
#include <PS3USB.h>

/************************************************************************/

#define SPI_CS_PIN 8
#define CAN_BPS CAN_500KBPS

#define SEND_DATA_LENGTH 8

/************************************************************************/

void PrintHexadecimal(long num, unsigned int digit)
{
	const char _temp_char[16] =
	{
		'0' , '1' , '2' , '3' ,
		'4' , '5' , '6' , '7' ,
		'8' , '9' , 'a' , 'b' ,
		'c' , 'd' , 'e' , 'f'
	};
	
	char *str = new char[digit + 1];
	
	if (num < 0)
	{
		num *= -1;
		
		str[digit - 1] = '-';
		
		digit --;
	}
	
	for (short int i = (digit - 1); i > -1; i--)
	{
		str[i] = _temp_char[num % 16];
		
		num /= (unsigned long)16;
	}
	
	str[digit] = '\0';
	
	Serial.print(str);
	
	delete str;
}

/************************************************************************/

class ControllerCircit
{
private:
	
	PS3USB controller;
	MCP_CAN transfer;
	
	USB usb;
	
	union ControllerData
	{
		struct BitControllerData
		{
			//[0]
			byte stick_left_x	:4;
			byte stick_left_y	:4;
			
			//[1]
			byte stick_right_x	:4;
			byte stick_right_y	:4;
			
			//[2]
			byte button_L1	:1;
			byte button_L2	:1;
			byte button_L3	:1;
			byte button_R1	:1;
			byte button_R2	:1;
			byte button_R3	:1;
			
			byte button_start	:1;
			byte button_select	:1;
			
			//[3]
			byte button_circle		:1;
			byte button_triangle	:1;
			byte button_cross		:1;
			byte button_square		:1;
			
			byte cross_left		:1;
			byte cross_over		:1;
			byte cross_right	:1;
			byte cross_under	:1;
		}
		bit;
		
		byte array[SEND_DATA_LENGTH];
	}
	send_data;
	
	ControllerData before_data;
	
public:
	
	ControllerCircit();
	
	void Initialize();
	
	void Read();
	
	void Convert();
	
	void Write();
};

/************************************************************************/

//----------------------------------------------------------------------//

ControllerCircit::ControllerCircit()
	: controller(&usb)
	, transfer(SPI_CS_PIN)
{
	for (int i = 0; i < SEND_DATA_LENGTH; i ++)
	{
		before_data.array[i] = 0;
		send_data.array[i] = 0;
	}
}

//----------------------------------------------------------------------//

void ControllerCircit::Initialize()
{
	Serial.begin(115200);
	Serial.println("Hello!! Mr.Suzu! Activate this circuit. Wait a second.");
	
	Serial.println("Confirm connection with Dualshock 3...");
	
	if (usb.Init() == -1)
	{
		Serial.println("Failed connection with Dualshock 3.");
		Serial.println("Confirm connection with Dualshock 3 again...");
		delay(100);
	}
	
	Serial.println("Successful connection with Dualshock 3!");
	
	Serial.println("Initialize CAN BUS Shield...");
	
	while (transfer.begin(CAN_BPS) != CAN_OK)
	{
		Serial.println("CAN BUS Shield initialization failed");
		Serial.println("Initialize CAN BUS Shield again...");
		delay(100);
	}
	
	Serial.println("Successful initialization of CAN BUS Shield!");
	
	Serial.println("This circuit activated successfully.");
	Serial.println("Good luck");
}

//----------------------------------------------------------------------//

void ControllerCircit::Read()
{
	usb.Task();
}

//----------------------------------------------------------------------//

void ControllerCircit::Convert()
{
	for (int i = 0; i < 4; i++)
	{
		before_data.array[i] = send_data.array[i];
	}
	
	send_data.bit.stick_left_x = controller.getAnalogHat(LeftHatX) / 17;
	send_data.bit.stick_left_y = controller.getAnalogHat(LeftHatY) / 17;
	send_data.bit.stick_right_x = controller.getAnalogHat(RightHatX) / 17;
	send_data.bit.stick_right_y = controller.getAnalogHat(RightHatY) / 17;
	
	send_data.bit.button_L1 = controller.getButtonPress(L1) ? 1 : 0;
	send_data.bit.button_L2 = controller.getButtonPress(L2) ? 1 : 0;
	send_data.bit.button_L3 = controller.getButtonPress(L3) ? 1 : 0;
	
	send_data.bit.button_R1 = controller.getButtonPress(R1) ? 1 : 0;
	send_data.bit.button_R2 = controller.getButtonPress(R2) ? 1 : 0;
	send_data.bit.button_R3 = controller.getButtonPress(R3) ? 1 : 0;
	
	send_data.bit.button_circle		= controller.getButtonPress(CIRCLE) ? 1 : 0;
	send_data.bit.button_square		= controller.getButtonPress(SQUARE) ? 1 : 0;
	send_data.bit.button_cross		= controller.getButtonPress(CROSS) ? 1 : 0;
	send_data.bit.button_triangle	= controller.getButtonPress(TRIANGLE) ? 1 : 0;
	
	send_data.bit.button_select	= controller.getButtonPress(SELECT) ? 1 : 0;
	send_data.bit.button_start	= controller.getButtonPress(START) ? 1 : 0;
	
	send_data.bit.cross_left	= controller.getButtonPress(LEFT) ? 1 : 0;
	send_data.bit.cross_right	= controller.getButtonPress(RIGHT) ? 1 : 0;
	send_data.bit.cross_over	= controller.getButtonPress(UP) ? 1 : 0;
	send_data.bit.cross_under	= controller.getButtonPress(DOWN) ? 1 : 0;
}

//----------------------------------------------------------------------//

void ControllerCircit::Write()
{
	transfer.sendMsgBuf(0x00, 0, SEND_DATA_LENGTH, send_data.array);
	
	for (int i = 0; i < 4; i++)
	{
		if (before_data.array[i] != send_data.array[i])
		{
			for (int i = 0; i < 4; i++)
			{
				PrintHexadecimal(send_data.array[i], 2);
				Serial.print(F("\t"));
			}
			
			Serial.print(F("\n"));
			
			break;
		}
	}
}

//----------------------------------------------------------------------//

/************************************************************************/

ControllerCircit circit;

/************************************************************************/

//----------------------------------------------------------------------//

void setup()
{
	circit.Initialize();
}

//----------------------------------------------------------------------//

void loop()
{
	circit.Read();
	
	circit.Convert();
	
	circit.Write();
}

//----------------------------------------------------------------------//

/************************************************************************/
