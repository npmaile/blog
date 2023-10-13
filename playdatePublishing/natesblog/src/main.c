//
//  main.c
//  Extension
//
//  Created by Dave Hayden on 7/30/14.
//  Copyright (c) 2014 Panic, Inc. All rights reserved.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pd_api.h"
#include "pd_api/pd_api_gfx.h"
#include "pd_api/pd_api_sys.h"

static int update(void *userdata);
static void splitLines();
// static void angleDrawLine(void *userdata, float);
LCDFont *font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif
    int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg) {
  (void)arg; // arg is currently only used for event = kEventKeyPressed

  if (event == kEventInit) {
    const char *err;
    pd->system->setUpdateCallback(update, pd);
    splitLines();
  }
  return 0;
}

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 400

#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

static char **lines;
static int numlines;

static int update(void *userdata) {
  PlaydateAPI *pd = userdata;
  pd->graphics->clear(kColorWhite);
  pd->graphics->setFont(font);
  for (int i = 0; i < numlines; i++) {
    pd->graphics->drawText(lines[i], strlen(lines[i]), kUTF8Encoding, 0,
                           i * TEXT_HEIGHT);
  }
  // pd->graphics->drawText(JUNK_TEXT, strlen(junktext), kUTF8Encoding, 0, 0);

  return 1;
}

static char *junktext =
    "sjdlkfjdslkfjlksjdfjlkdsjflkslkfjlkdsjflksjlkfjdlkfjslkfjlkdsjlkfdslkjfdsj"
    "lkfjdslkjflksjflkjslkfjkljdslkfjslkfjlkjdslkfsjlkfjlkfds";

static void splitLines() {
  int charsPerLine = SCREEN_WIDTH / TEXT_WIDTH;
  // split junk text into lines of width charsPerLine
  int len = strlen(junktext);
  int numlines = len / charsPerLine;
  lines = malloc(sizeof(char *) * numlines);
  for (int i = 0; i < len; i++) {
    char *line = malloc(sizeof(char) * charsPerLine);
    int j = 0;
    for (; (j < i * charsPerLine + 1) && line[i * charsPerLine + 1] != '\0';
         j++) {
      line[j] = i * charsPerLine + j;
    }
    line[j] = '\0';
    lines[i] = line;
  }
}

/*
static float linelength = 2.0f;
static int lineWidth = 1;

void angleDrawLine(void *userdata, float angle) {
  PlaydateAPI *pd = userdata;
  PDButtons current;
  PDButtons pushed;
  PDButtons released;
  pd->system->getButtonState(&current, &pushed, &released);
  if (pushed & kButtonUp) {
    linelength += 1.0f;
  }
  if (pushed & kButtonDown) {
    linelength -= 1.0f;
  }
  if (pushed & kButtonLeft) {
    lineWidth -= 1;
  }
  if (pushed & kButtonRight) {
    lineWidth += 1;
  }

  float angleRad = angle * 3.14f / 180.0f;
  float x0 = SCREEN_WIDTH * .5f;
  float x1 = x0 + linelength * cosf(angleRad);
  float y0 = SCREEN_HEIGHT * .5f;
  float y1 = y0 + linelength * sinf(angleRad);

  pd->graphics->drawLine(x0, y0, x1, y1, lineWidth, kColorXOR);
}
*/
