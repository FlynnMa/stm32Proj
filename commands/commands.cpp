#include "mbed.h"
#include "shell.h"
#include "trace.h"
#include "sensors.h"
#include "electronicVehicleCustom.h"


extern electronicVehicleCustomDataType evCustomData;
static void cmdJoke(unsigned int argc, char *argv[], int *result)
{
  const char aJok[] = "A successful marriage is based on give & take:\r\n"
      "Where husband gives money, gifts, dresses & wife takes it;\r\n";
  const char aJok2[] =
      "And whereas wife gives advices, lectures, tensions & husband takes it!\r\n";

  TraceLine();
  Trace(aJok);
  Trace(aJok2);
  TraceLine();
}

void cmdSetBatteryVoltage(unsigned int argc, char *argv[], int *result)
{
    evCustomData.voltage = atof(argv[0]);

}

void cmdAccOn(unsigned int argc, char *argv[], int *result)
{
}

void cmdAccLog(unsigned int argc, char *argv[], int *result)
{
}


void cmdGyroOn(unsigned int argc, char *argv[], int *result)
{

}

void cmdOrienLog(unsigned int argc, char *argv[], int *result)
{
}

void cmdOrientationOn(unsigned int argc, char *argv[], int *result)
{
}

void cmdLogGyro(unsigned int argc, char *argv[], int *result)
{
}


void cmdLogOrientationString(unsigned int argc, char *argv[], int *result)
{
}

void cmdLogBinary(unsigned int argc, char *argv[], int *result)
{
}

void cmdNonProcess(unsigned int argc, char *argv[], int *result)
{

}

void cmdRequestSyncToken(unsigned int argc, char *argv[], int *result)
{
}


extern const Command custCommands[] =
{
    {"jok",         "tell me a joke",                           cmdJoke},
    {"battery",     "set battery voltage",                      cmdSetBatteryVoltage},
    {"accon",       "enable acc sensor",                        cmdAccOn},
    {"acclog",      "enable acc data log",                      cmdAccLog},
    {"gyroon",      "enable orientation log",                   cmdGyroOn},
    {"binary",      "log orientaion data as binary",            cmdLogBinary},
    {"orien",       "enable orientation",                       cmdOrientationOn},
    {"orienlog",    "enable orientation data log",              cmdOrienLog},
    {"#oe0",        "disable out put errors",                   cmdNonProcess},
    {"#sync",       "request sync token",                       cmdRequestSyncToken},
    {NULL, NULL, NULL}
};

