#pragma once
struct PSF1_HEADER{
	unsigned char Magic[2];
	unsigned char Mode;
	unsigned char CharSize;
} ;

struct PSF1_FONT{
	PSF1_HEADER* PSF1Header;
	void* GlyphBuffer;
} ;