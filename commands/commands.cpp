#include "mbed.h"
#include "shell.h"
#include "trace.h"
#include "sensors.h"
#include "electronicVehicleCustom.h"
#include "electronicVehicle.h"
#include "protocalApi.h"


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
    protocalApiSetFloat(CMD_ID_BATTERY_VOLTAGE, evCustomData.voltage);
    Trace("\tset battery voltage :%f success!", evCustomData.voltage);

}

void cmdSetSpeed(unsigned int argc, char *argv[], int *result)
{
    evCustomData.speed = atof(argv[0]);
    protocalApiSetFloat(CMD_ID_SPEED, evCustomData.speed);
    Trace("\tset speed :%f success!", evCustomData.speed);
}

void cmdSetMile(unsigned int argc, char *argv[], int *result)
{
    evCustomData.mile = atoi(argv[0]);
    protocalApiSetU32(CMD_ID_MILE, evCustomData.mile);
    Trace("\tset mile :%d success!", evCustomData.mile);
}

void cmdGetMile(unsigned int argc, char *argv[], int *result)
{
    protocalApiQueryWait(CMD_ID_MILE, &evCustomData.mile, sizeof(evCustomData.mile));
    Trace("\tGet mile :%d success!", evCustomData.mile);
}

extern const Command custCommands[] =
{
    {"jok",         "tell me a joke",                           cmdJoke},
    {"battery",     "set battery voltage",                      cmdSetBatteryVoltage},
    {"speed",       "set speed",                                cmdSetSpeed},
    {"mile",        "set mile",                                 cmdSetMile},
    {"getmile",     "Get mile",                                 cmdGetMile},
    {NULL, NULL, NULL}
};

