#include <TimeLib.h>
#include <time.h>

/**
 * Input time in epoch format and return tm time format
 * by Renzo Mischianti <www.mischianti.org> 
 */
static tm GetDateTimeByParams(long TimeParam) {
    const time_t Time = TimeParam;
    struct tm *NewTime = localtime(&Time);
    return *NewTime;
}
/**
 * Input tm time format and return String with format pattern
 * by Renzo Mischianti <www.mischianti.org>
 */
static String GetDateTimeStringByParams(tm *NewTime, char* Pattern = (char *)"%d/%m/%Y %H:%M:%S") {
    char Buffer[30];
    strftime(Buffer, 30, Pattern, NewTime);
    return Buffer;
}
 
/**
 * Input time in epoch format format and return String with format pattern
 * by Renzo Mischianti <www.mischianti.org> 
 */
static String GetEpochStringByParams(long Time, char* Pattern = (char *)"%d/%m/%Y %H:%M:%S") {
    tm NewTime = GetDateTimeByParams(Time);
    return GetDateTimeStringByParams(&NewTime, Pattern);
}
