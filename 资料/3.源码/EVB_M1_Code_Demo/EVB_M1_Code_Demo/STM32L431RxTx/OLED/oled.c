#include "oled.h"
#include "BC95_CluB.h"
extern msg_for_oled_show	msg_oled_show;
	//Œ¬
	const u8g_pgm_uint8_t Text_wen[32] = {
0x00,0x00,0x00,0x00,0x1F,0xFC,0x0A,0x54,0x0A,0x54,0x0A,0x56,0x0F,0xF4,0x08,0x14,
0x07,0xE8,0x04,0x28,0x04,0x2A,0x07,0xE8,0x04,0x30,0x07,0xEC,0x00,0x20,0x00,0x00
	};

	//∂»
	const u8g_pgm_uint8_t Text_du[32] = {	
0x00,0x00,0x00,0x80,0x18,0x84,0x07,0x7C,0x02,0x24,0x1F,0xFC,0x02,0x24,0x03,0xC4,
0x04,0x04,0x07,0xE4,0x02,0x42,0x01,0x82,0x01,0x82,0x1E,0x79,0x00,0x00,0x00,0x00
	};
	
	// ™
	const u8g_pgm_uint8_t Text_shi[] = {
  0xff, 0xff, 0xff, 0xff, 0x3f,   0xff, 0xff, 0xff, 0xff, 0x3f,   0xe0, 0xe0, 0xff, 0xff, 0x3f,
   0xe3, 0xe1, 0xff, 0xff, 0x3f,   0xf3, 0xf1, 0xff, 0xff, 0x3f,   0xf3, 0xf1, 0xfe, 0xbf, 0x37,
   0xf3, 0x11, 0x1c, 0x1f, 0x30,   0xf3, 0x01, 0x08, 0x8c, 0x20,   0xf3, 0x01, 0x00, 0xc0, 0x39,
   0xf3, 0x81, 0xc7, 0xc1, 0x39,   0xf3, 0xc1, 0xc7, 0xc9, 0x38,   0xf3, 0xc1, 0xc3, 0x19, 0x3c,
   0xe3, 0x89, 0x01, 0x98, 0x3f,   0xc7, 0x18, 0x00, 0x08, 0x3e,   0x0f, 0x3c, 0x70, 0x1c, 0x30,
   0x3f, 0xff, 0xfc, 0x87, 0x31,   0xff, 0xff, 0xbf, 0xc7, 0x23,   0x01, 0x00, 0x00, 0xc6, 0x23,
   0x03, 0x00, 0x00, 0x0e, 0x30,   0xff, 0xff, 0x3f, 0x1f, 0x3c,   0xff, 0xff, 0x3f, 0xff, 0x3f,
   0xff, 0xff, 0x3f, 0xff, 0x3f,   0xff, 0xff, 0xff, 0xff, 0x3f,   0xff, 0xff, 0xff, 0xff, 0x3f
		};
		
extern const u8g_fntpgm_uint8_t customfont[] U8G_FONT_SECTION("customfont");

	const u8g_fntpgm_uint8_t testfont[] U8G_FONT_SECTION("testfont") = {
0,16,16,0,254,0,0,0,0,0,0,15,0,14,254,0,
//Áâ©(\x00)
0,14,16,33,16,0,254,16,128,16,128,80,128,80,252,125,
84,82,84,144,84,16,148,28,148,241,36,82,36,16,68,16,
68,16,132,17,40,16,16,
//ËÅî(\x01)
0,15,16,33,16,0,254,0,136,252,72,72,80,72,0,121,
252,72,32,72,32,120,32,75,254,72,32,76,80,120,80,200,
136,8,136,9,4,10,2,
//ÁΩë(\x02)
0,13,15,31,16,1,254,255,248,128,8,128,8,132,40,165,
40,148,168,136,72,136,72,148,168,148,168,165,40,194,8,128,
8,128,40,128,16,
//‰ø±(\x03)
0,15,16,33,16,0,254,8,0,11,248,10,8,18,8,19,
248,50,8,51,248,82,8,147,248,18,8,18,8,31,254,16,
0,17,16,18,8,20,4,
//‰πê(\x04)
0,13,16,33,16,1,254,0,64,1,224,62,0,32,0,34,
0,66,0,66,0,127,248,2,0,18,64,18,32,34,16,66,
8,130,8,10,0,4,0,
//ÈÉ®(\x05)
0,15,16,33,16,0,254,16,0,8,62,127,162,0,36,33,
36,18,40,255,228,0,36,0,34,63,34,33,34,33,52,33,
40,63,32,33,32,0,32,
//F(\x06)
0,6,10,11,8,1,0,252,128,128,128,248,128,128,128,128,
128,
//1(\x07)
0,5,10,11,8,2,0,32,96,160,32,32,32,32,32,32,
248,
//2(\x08)
0,6,10,11,8,1,0,120,132,132,4,24,32,64,128,128,
252,
//3(\x09)
0,6,10,11,8,1,0,120,132,132,4,56,4,4,132,132,
120,
//4(\x0a)
0,6,10,11,8,1,0,8,24,40,72,136,136,252,8,8,
8,
//5(\x0b)
0,6,10,11,8,1,0,252,128,128,128,248,4,4,4,132,
120,
//6(\x0c)
0,6,10,11,8,1,0,56,64,128,128,248,132,132,132,132,
120,
//7(\x0d)
0,6,10,11,8,1,0,252,4,4,8,8,8,16,16,16,
16,
//8(\x0e)
0,6,10,11,8,1,0,120,132,132,132,120,132,132,132,132,
120,
//9(\x0f)
0,6,10,10,8,1,0,120,132,132,132,124,4,4,4,8,
112

};


extern u8g_t u8g;  
static void OLED_Show_Temp(float temp, float hum)
{

	
}


void OLED_Show_Page1(void)
{
	//u8g_DrawXBMP(&u8g, 0, 0, 16, 16, Text_du);
	//u8g_SetFont(&u8g,testfont);//set current font  
	//u8g_DrawStr(&u8g, 5, 32, "\x00\x01\x02\x03\x04");
	//u8g_DrawStr(&u8g, 15, 16, "\xa1\xa2\xa3\xa4\xa5\xa6");
	u8g_SetFont(&u8g,u8g_font_profont10);//set current font  
	u8g_DrawStr(&u8g, 25, 10, "www.iot-club.cn");	
}
extern int i;
void OLED_Show_Page2(void)
{
	u8g_SetFont(&u8g,customfont);//set current font  
	u8g_SetFontPosTop(&u8g);
	
	//u8g_DrawStr(&u8g, 15, 16, "\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8");
//	u8g_DrawStr(&u8g, 15, 16, "\x06\x07\x10\x11\x12\x13\x14");
//	u8g_DrawStr(&u8g, 15, 30, "\x06\x08\x10\x15\x16\x17");
//	u8g_DrawStr(&u8g, 15, 44, "\x06\x08\x10\x18\x19\x1a\x1b");

	
	
	u8g_DrawStr(&u8g, 15, i, "\x06\x07\x10\x11\x12\x13\x14");
	
	u8g_DrawStr(&u8g, 15, i+18, "\x06\x08\x10\x15\x16\x17");
	
	u8g_DrawStr(&u8g, 15, i+36, "\x06\x09\x10\x18\x19\x1a\x1b");	
	
	
	
	//u8g_DrawStr(&u8g, 15, 16, "F1");
}
void OLED_Show_a_Page(int show_i)
{
	u8g_SetFont(&u8g,u8g_font_profont10);
	u8g_SetFontPosTop(&u8g);
	switch(show_i)
	{
		//case 1:u8g_DrawStr(&u8g, 25, 28, "www.iot-club.cn");
		case 1:{
			//Flag 1
			
			
			u8g_DrawStr(&u8g, 40, 0, "CSQ and Time");
			u8g_DrawStr(&u8g, 0, 10, "CSQ:");
			u8g_DrawStr(&u8g, 28, 10, bc95_net_data.net_csq);
			u8g_DrawStr(&u8g, 0, 20, "Time:");
			u8g_DrawStr(&u8g, 26, 20, bc95_net_data.net_time);
			break;
		}
		case 2:{
			u8g_DrawStr(&u8g, 0, 0, "Temperature and humidity:");
			u8g_DrawStr(&u8g, 0, 10, "Temp:");
			u8g_DrawStr(&u8g, 25, 10, msg_oled_show.TEMP_for_show);
			u8g_DrawStr(&u8g, 0, 20, "Humi:");
			u8g_DrawStr(&u8g, 25, 20, msg_oled_show.HUMI_for_show);
			break;
		}
		case 3:{
			u8g_DrawStr(&u8g, 0, 0, "Receive UDP data:");
			u8g_DrawStr(&u8g, 20, 10, bc95_net_data.net_nsorf_str);
			u8g_DrawStr(&u8g, 0, 20, "Time:");
			u8g_DrawStr(&u8g, 26, 20, bc95_net_data.net_time);
			break;
		}
	}	
}
