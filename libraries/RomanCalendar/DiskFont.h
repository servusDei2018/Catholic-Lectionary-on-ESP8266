#pragma once

#ifndef _FONT_H
#define _FONT_H

#include "Arduino.h"
#include "SD.h"
#define FS_NO_GLOBALS
#include "FS.h"
#include <math.h>
#include <epd2in7b.h>
#include <epdpaint.h>
#include <utf8string.h>
#include "I2CSerialPort.h"
#include "I18n.h"

extern "C" {
#include "user_interface.h"
}
extern const int COLORED;
extern const int UNCOLORED;

extern const int PANEL_SIZE_X;
extern const int PANEL_SIZE_Y;

const unsigned long FONT_HEADER_OFFSET_START = 8;
const unsigned long FONT_HEADER_OFFSET_CHARHEIGHT = 8; // word
const unsigned long FONT_HEADER_OFFSET_STARTCHAR = 10; // dword
const unsigned long FONT_HEADER_OFFSET_ENDCHAR = 14; // dword
const unsigned long FONT_HEADER_OFFSET_NUMLOOKUPENTRIES = 18; // word
const unsigned long FONT_HEADER_OFFSET_SPACECHARWIDTH = 20; // byte
const unsigned long FONT_HEADER_OFFSET_END = 21;
const unsigned long BLOCKTABLE_OFFSET_BEGIN = FONT_HEADER_OFFSET_END;

typedef struct {
	uint32_t startchar;
	uint32_t endchar;
	uint32_t fileoffset_startchar;
} DiskFont_BlocktableEntry;

typedef struct {
	//uint8_t rtlflag;
	uint16_t widthbits;
	uint16_t heightbits;
	uint32_t bitmapfileoffset;
	double advanceWidth;
	double advanceHeight;
} DiskFont_FontCharInfo;

const int DiskFont_FontCharInfo_RecSize = 2+2+4+8+8; //bytes

class DiskFont {
public:
	//I18n* _I18n;
	String _fontfilename;
	
	File _file_sd;
	fs::File _file_spiffs;
	
	double 	_font_tuning_percent = 50.0;
	bool	_font_use_fixed_spacing = false;
	bool	_font_use_fixed_spacecharwidth = false;
	int		_font_fixed_spacing = 1;
	int		_font_fixed_spacecharwidth = 2;

	bool available = false;

	struct {
		uint16_t charheight;
		uint32_t startchar;
		uint32_t endchar;
		uint16_t numlookupblocks;
		uint8_t spacecharwidth;
		double ascent;
		double descent;
		double linespacing;
	} _FontHeader;
	
	DiskFont_BlocktableEntry* _Font_BlocktablePtr = NULL;
	
	#define CHARBUFF_SIZE 32
	uint8_t _char_buffer[CHARBUFF_SIZE];
	
	DiskFont();
	~DiskFont();
	
	bool begin(String fontfilename);
	bool begin(String fontfilename, double font_tuning_percent);
	bool begin(ConfigParams c);
	void end(void);

	bool OpenFontFile();
	bool CloseFontFile();
	bool Seek(uint64_t offset);
	size_t Position();
	size_t Size();
	bool Read(uint32_t* var);
	bool Read(uint16_t* var);
	bool Read(uint8_t* var);
	bool Read(double* var);
	bool Read(void* array, uint32_t bytecount);
	
	int DrawCharAt(int x, int y, char ascii_char, Paint* paint, int colored, uint16_t* blockToCheckFirst);
	int DrawCharAt(int x, int y, uint32_t codepoint, Paint* paint, int colored, uint16_t* blockToCheckFirst);
	int DrawCharAt(int x, int y, double& advanceWidth, uint32_t codepoint, Paint* paint, int colored, uint16_t* blockToCheckFirst);	

	void DrawStringAt(int x, int y, const char* text, Paint* paint, int colored, bool right_to_left, bool reverse_string);
	void DrawStringAt(int x, int y, String text, Paint* paint, int colored, bool right_to_left, bool reverse_string);
	void DrawStringAt(int x, int y, String text, Paint* paint, int colored, bool right_to_left);
	
	void DrawStringAtA(int x, int y, String text, Paint* paint, int colored, bool right_to_left, bool reverse_string);
	void DrawStringAtA(int x, int y, String text, Paint* paint, int colored, bool right_to_left);
	
	//bool doRtlStrings(String* s, bool right_to_left); // will process input string so that substrings are in correct rtl or ltr order, depending on characterset
	
	void GetTextWidth(String text, int& width, double& advanceWidth);
	int GetTextWidth(const char* text);
	int GetTextWidth(String text);
	double GetTextWidthA(String text);
	double GetTextWidthA(const char* text);
	
	double GetAdvanceWidth(uint16_t bitmapwidth, double advanceWidth, uint32_t codepoint, uint16_t& char_width);
//	double GetAdvanceWidth(int bitmapwidth, double advanceWidth);
	
	bool getCharInfo(String ch, DiskFont_FontCharInfo* fci);
	bool getCharInfo(int codepoint, uint16_t* blockToCheckFirst, DiskFont_FontCharInfo* fci);
	bool readFontCharInfoEntry(DiskFont_FontCharInfo* fci);
};

class FontBlockTable {
public:
	FontBlockTable();
	~FontBlockTable();
};

#endif
