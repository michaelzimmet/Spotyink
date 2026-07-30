#pragma once
#include <Arduino.h>
#include "../dataObjs/cover/Cover.h"

bool fetchCover(const String& url, int nativeSize, int targetSize, Cover& outCover);
