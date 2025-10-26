#pragma once

typedef struct {
  int lineHeight;
  int base;
  int pages;
  int packed;
} BMFontCommon;

typedef struct {
  int id;
  char file[64];
} BMFontPage;

typedef struct {
  int id;
  int x;
  int y;
  int width;
  int height;
  int xoffset;
  int yoffset;
  int xadvance;
  int page;
} BMFontChar;

typedef struct {
  BMFontCommon common;
  BMFontPage* pages;
  int charCount;
  BMFontChar* chars;
} BMFont;

BMFont* parseBMFont(const char* text);

void freeBMFont(BMFont* font);
