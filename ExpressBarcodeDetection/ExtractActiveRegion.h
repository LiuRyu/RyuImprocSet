#ifndef EXTRACT_ACTIVE_REGION_H
#define EXTRACT_ACTIVE_REGION_H

#include "RyuCore.h"

int  ExtractActiveRegion_init();

void ExtractActiveRegion_release();

int ExtractActiveRegion(unsigned char * img, int wid, int hei);

RyuPoint * getExtractActiveRegion();

#endif