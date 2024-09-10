#pragma once

#include <drivers/drv_hrt.h>

bool HrtHelper_IsExpired(const hrt_abstime time, const hrt_abstime period);
bool HrtHelper_IsDue(hrt_abstime& time, const hrt_abstime period);
