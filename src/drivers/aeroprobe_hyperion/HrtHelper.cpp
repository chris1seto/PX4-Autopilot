#include "HrtHelper.hpp"

bool HrtHelper_IsExpired(const hrt_abstime time, const hrt_abstime period)
{
  if (hrt_absolute_time() - time < period)
  {
    return false;
  }

  return true;
}

bool HrtHelper_IsDue(hrt_abstime& time, const hrt_abstime period)
{
  hrt_abstime time_now = hrt_absolute_time();

  if (time_now - time < period)
  {
    return false;
  }

  time = time_now;
  return true;
}
