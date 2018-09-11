//Segmentron library
// By Christian Elzey
// Last modified 9-8-2018
// 
// Description: Intended to be run on an ESP8266 microcontroller powering a Segmentron 16-segment display. 
// See Github for more info. 

#include "Segmentron.h"
#include "Arduino.h"

uint8_t numberOfModules;
uint8_t numberOfLines;
uint8_t numberOfColumns;

String messageToPrint; 

uint8_t base_I2C = 0x70; // base I2C addr. 

char charBuffer[64]; // This is stores all 64 characters that were last displayed. 
uint16_t displaybuffer[64]; // Stores the actual pixel values that will go to the LEDs
  

  

static const uint8_t numbertable[] = {
	0x3F, /* 0 */
	0x06, /* 1 */
	0x5B, /* 2 */
	0x4F, /* 3 */
	0x66, /* 4 */
	0x6D, /* 5 */
	0x7D, /* 6 */
	0x07, /* 7 */
	0x7F, /* 8 */
	0x6F, /* 9 */
	0x77, /* a */
	0x7C, /* b */
	0x39, /* C */
	0x5E, /* d */
	0x79, /* E */
	0x71, /* F */
};


static const uint16_t asciiFontTable[] PROGMEM =  {

0b0000000000000001,
0b0000000000000010,
0b0000000000000100,
0b0000000000001000,
0b0000000000010000,
0b0000000000100000,
0b0000000001000000,
0b0000000010000000,
0b0000000100000000,
0b0000001000000000,
0b0000010000000000,
0b0000100000000000,
0b0001000000000000,
0b0010000000000000,
0b0100000000000000,
0b1000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0001001011001001,
0b0001010111000000,
0b0001001011111001,
0b0000000011100011,
0b0000010100110000,
0b0001001011001000,
0b0011101000000000,
0b0001011100000000,


	0b0000000000000000, /* (space) */
	0b0000000000001100, /* ! */
	0b0000001000000100, /* " */
	0b1010101000111100, /* # */
	0b1010101010111011, /* $ */
	0b1110111010011001, /* % */
	0b1001001101110001, /* & */
	0b0000001000000000, /* ' */
	0b0001010000000000, /* ( */
	0b0100000100000000, /* ) */
	0b1111111100000000, /* * */
	0b1010101000000000, /* + */
	0b0100000000000000, /* , */
	0b1000100000000000, /* - */
	0b0001000000000000, /* . */
	0b0100010000000000, /* / */
	0b0100010011111111, /* 0 */
	0b0000010000001100, /* 1 */
	0b1000100001110111, /* 2 */
	0b0000100000111111, /* 3 */
	0b1000100010001100, /* 4 */
	0b1001000010110011, /* 5 */
	0b1000100011111011, /* 6 */
	0b0000000000001111, /* 7 */
	0b1000100011111111, /* 8 */
	0b1000100010111111, /* 9 */
	0b0010001000000000, /* : */
	0b0100001000000000, /* ; */
	0b1001010000000000, /* < */
	0b1000100000110000, /* = */
	0b0100100100000000, /* > */
	0b0010100000000111, /* ? */
	0b0000101011110111, /* @ */
	0b1000100011001111, /* A */
	0b0010101000111111, /* B */
	0b0000000011110011, /* C */
	0b0010001000111111, /* D */
	0b1000000011110011, /* E */
	0b1000000011000011, /* F */
	0b0000100011111011, /* G */
	0b1000100011001100, /* H */
	0b0010001000110011, /* I */
	0b0000000001111100, /* J */
	0b1001010011000000, /* K */
	0b0000000011110000, /* L */
	0b0000010111001100, /* M */
	0b0001000111001100, /* N */
	0b0000000011111111, /* O */
	0b1000100011000111, /* P */
	0b0001000011111111, /* Q */
	0b1001100011000111, /* R */
	0b1000100010111011, /* S */
	0b0010001000000011, /* T */
	0b0000000011111100, /* U */
	0b0100010011000000, /* V */
	0b0101000011001100, /* W */
	0b0101010100000000, /* X */
	0b1000100010111100, /* Y */
	0b0100010000110011, /* Z */
	0b0010001000010010, /* [ */
	0b0001000100000000, /* \ */
	0b0010001000100001, /* ] */
	0b0101000000000000, /* ^ */
	0b0000000000110000, /* _ */
	0b0000000100000000, /* ` */
	0b1010000001110000, /* a */
	0b1010000011100000, /* b */
	0b1000000001100000, /* c */
	0b0010100000011100, /* d */
	0b1100000001100000, /* e */
	0b1010101000000010, /* f */
	0b1010001010100001, /* g */
	0b1010000011000000, /* h */
	0b0010000000000000, /* i */
	0b0010001001100000, /* j */
	0b0011011000000000, /* k */
	0b0000000011000000, /* l */
	0b1010100001001000, /* m */
	0b1010000001000000, /* n */
	0b1010000001100000, /* o */
	0b1000001011000001, /* p */
	0b1010001010000001, /* q */
	0b1000000001000000, /* r */
	0b1010000010100001, /* s */
	0b1000000011100000, /* t */
	0b0010000001100000, /* u */
	0b0100000001000000, /* v */
	0b0101000001001000, /* w */
	0b0101010100000000, /* x */
	0b0000101000011100, /* y */
	0b1100000000100000, /* z */
	0b1010001000010010, /* { */
	0b0010001000000000, /* | */
	0b0010101000100001, /* } */
	0b1100110000000000, /* ~ */
	0b0011111111111111,

};

// THIS NEEDS TO BE UPDATED TO SUPPORT MULTIPLE MODULES
void Segmentron::setBrightness(uint8_t b) {
  if (b > 15) b = 15;
  Wire.beginTransmission(i2c_addr);
  Wire.write(HT16K33_CMD_BRIGHTNESS | b);
  Wire.endTransmission();  
}


// Sets designated module to blink at specified rate. 
void Segmentron::blinkRate(uint8_t b, uint8_t _module) {
  i2c_addr = base_I2C + (_module * 2);
  Wire.beginTransmission(i2c_addr);
  if (b > 3) b = 0; 
  
  Wire.write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1)); 
  Wire.endTransmission();
}

Segmentron::Segmentron(void) {
}

void Segmentron::begin(uint8_t _numberOfModules = 1, uint8_t _lines = 1, uint8_t _columns = 1) {
	numberOfLines = _lines;
	numberOfColumns = _columns;
    numberOfModules = _numberOfModules;

  i2c_addr = base_I2C; // This is the I2C address of the FIRST module. (Line 1 Col 1)
 // The layout of the modules will be arranged as follows. 
 // 0, 1, 2, 3           
 // 4, 5, 6, 7             
 // The actual address of each module is equal to its NUMBER (0-7) plus 0x70
 
  
  Wire.begin();
  for(int i = 0; i < numberOfModules; i++){
 	  Wire.beginTransmission(i2c_addr);
	  Wire.write(0x21);  // turn on oscillator
	  Wire.endTransmission();
 	  blinkRate(HT16K33_BLINK_OFF, i);
	  setBrightness(15); // 
	  i2c_addr = i2c_addr + i*2; // increment to the next module
  }
  
  
}


// Will write the entire 64-character buffer to the display
void Segmentron::writeDisplay() {
i2c_addr = base_I2C;

for (int i = 0; i < numberOfModules; i++){
  i2c_addr = i2c_addr + (i*2); // increment i2c address for each module
  
  Wire.beginTransmission(i2c_addr);
  Wire.write((uint8_t)0x00 ); // start at address $00

  for (uint8_t j=0; j<8; j++) {
  	int k = (i * 8) + j;
    Wire.write(displaybuffer[k] & 0xFF);    
    Wire.write(displaybuffer[k] >> 8);    
  }
  Wire.endTransmission();  
}
}// end Writedisplay


void Segmentron::clear(void) {
  for (uint8_t i=0; i<64; i++) {
    displaybuffer[i] = 0;
  }
}
  
  
  




void Segmentron::writeDigitRaw(uint8_t n, uint16_t bitmask) {
  displaybuffer[n] = bitmask;
}


void Segmentron::printString(String _text, uint8_t line, uint8_t n){
// Will print out the string on the specified line (indexed at 0 of course) starting 
// at the specified character. User needs to call writedisplay afterwards. 

	String Text = _text;
	
	uint8_t startLocation = ((line * numberOfColumns)*8) + n;
	uint8_t endLocation = ((line + 1) * numberOfColumns * 8);
	
	int x = 0;
	int len = Text.length(); 
	for(int i = startLocation; i < endLocation; i++){
		displaybuffer[i] = Text.charAt(x);
		x++;
		if(x >= len){ break;};// stops it from overflowing string
	}
	
}


void Segmentron::writeDigitAscii(uint8_t n, uint8_t line, uint8_t a) {

	// fixing swapped pins on the PCB. 
  uint16_t font = pgm_read_word(asciiFontTable+a);
  uint16_t segmentB = font & (0b0000000000000010);
  uint16_t segmentK = font & (0b0000000100000000);
  font = font & 0b1111111011111101;
  if(segmentB != 0){
	font = font | 0b00000000100000000;
  }
 if(segmentK != 0){
	font = font | 0b00000000000000010;
  }
  
  uint8_t pos;
  pos = (line * numberOfColumns * 8) + n;
  displaybuffer[pos] = font;

 

}
