#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmfont.h"

static char *trim(char *str) {
  char *end;

  while (isspace((unsigned char) *str)) {
    ++str;
  }

  if (*str == 0) {
    return NULL;
  }

  end = str + strlen(str) - 1;

  while (end > str && isspace((unsigned char) *end)) {
    --end;
  }

  end[1] = 0;

  return str;
}

static void parseAttributes(char *attr, char *key, char *value) {
  char *eq = strchr(attr, '=');

  if (eq) {
    *eq = 0;
    strcpy(key, attr);
    strcpy(value, eq + 1);

    if (value[0] == '\"' && value[strlen(value) - 1] == '\"') {
      memmove(value, value + 1, strlen(value) - 1);
      value[strlen(value) - 2] = 0;
    }
  }
}

BMFont *parseBMFont(const char *text) {
  BMFont *font = calloc(1, sizeof(BMFont));

  char line[1024];
  char key[64];
  char value[64];

  char *buffer = strdup(text);
  const char *linePtr = buffer;

  int currentPage = -1;
  int currentChar = -1;

  while (linePtr) {
    char *nl = strchr(linePtr, '\n');

    if (nl != NULL) {
      strncpy(line, linePtr, nl - linePtr);
      linePtr = nl + 1;
    } else {
      strcpy(line, linePtr);
      linePtr = NULL;
    }

    char *trimmedLine = trim(line);

    if (!trimmedLine) {
      // ignore empty lines
      continue;
    }

    char *token = strtok(line, " ");

    if (token) {
      if (strcmp(token, "common") == 0) {
        while ((token = strtok(NULL, " "))) {
          parseAttributes(token, key, value);
          if (strcmp(key, "lineHeight") == 0) {
            font->common.lineHeight = atoi(value);
          } else if (strcmp(key, "base") == 0) {
            font->common.base = atoi(value);
          } else if (strcmp(key, "pages") == 0) {
            font->common.pages = atoi(value);
            font->pages = calloc(font->common.pages, sizeof(BMFontPage));
          } else if (strcmp(key, "packed") == 0) {
            font->common.packed = atoi(value);
          }
        }
      } else if (strcmp(token, "page") == 0) {
        BMFontPage *page = &font->pages[++currentPage];
        while ((token = strtok(NULL, " "))) {
          parseAttributes(token, key, value);
          if (strcmp(key, "id") == 0) {
            page->id = atoi(value);
          } else if (strcmp(key, "file") == 0) {
            strcpy(page->file, value);
          }
        }
      } else if (strcmp(token, "chars") == 0) {
        while ((token = strtok(NULL, " "))) {
          parseAttributes(token, key, value);
          if (strcmp(key, "count") == 0) {
            font->charCount = atoi(value);
            font->chars = calloc(font->charCount, sizeof(BMFontChar));
          }
        }
      } else if (strcmp(token, "char") == 0) {
        BMFontChar *ch = &font->chars[++currentChar];
        while ((token = strtok(NULL, " "))) {
          parseAttributes(token, key, value);

          if (strcmp(key, "id") == 0) {
            ch->id = atoi(value);
          } else if (strcmp(key, "x") == 0) {
            ch->x = atoi(value);
          } else if (strcmp(key, "y") == 0) {
            ch->y = atoi(value);
          } else if (strcmp(key, "width") == 0) {
            ch->width = atoi(value);
          } else if (strcmp(key, "height") == 0) {
            ch->height = atoi(value);
          } else if (strcmp(key, "xoffset") == 0) {
            ch->xoffset = atoi(value);
          } else if (strcmp(key, "yoffset") == 0) {
            ch->yoffset = atoi(value);
          } else if (strcmp(key, "xadvance") == 0) {
            ch->xadvance = atoi(value);
          } else if (strcmp(key, "page") == 0) {
            ch->page = atoi(value);
          }
        }
      }
    }
  }

  free(buffer);

  return font;
}

void freeBMFont(BMFont *font) {
  if (font->pages) {
    free(font->pages);
  }

  if (font->chars) {
    free(font->chars);
  }

  free(font);
}
