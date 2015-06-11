#include "mbed.h"
#include "shell.h"
#include "trace.h"
#include "sensors.h"

static void cmdJoke(unsigned int argc, char *argv[], int *result)
{
  const char aJok[] = "A successful marriage is based on give & take:\r\n"
      "Where husband gives money, gifts, dresses & wife takes it;\r\n"
      "And whereas wife gives advices, lectures, tensions & husband takes it!\r\n";

  TraceLine();
  Trace(aJok);
  TraceLine();
}

void cmdLogOrientationOn(unsigned int argc, char *argv[], int *result)
{

}

void cmdAccOn(unsigned int argc, char *argv[], int *result)
{
    if (argc == 0)
    {
        sensorsAccEnable(TOGGLE);
    }
    else if (strncasecmp(argv[0], "on", 2) == 0)
    {
        sensorsAccEnable(true);
    }
    else if (strncasecmp(argv[0], "off", 3) == 0)
    {
        sensorsAccEnable(false);
    }
}

void cmdAccLog(unsigned int argc, char *argv[], int *result)
{
    if (argc == 0)
    {
        sensorsAccLogEnable(TOGGLE);
    }
    else if (strncasecmp(argv[0], "on", 2) == 0)
    {
        sensorsAccLogEnable(true);
    }
    else if (strncasecmp(argv[0], "off", 3) == 0)
    {
        sensorsAccLogEnable(false);
    }
}


void cmdGyroOn(unsigned int argc, char *argv[], int *result)
{

}

void cmdOrienLog(unsigned int argc, char *argv[], int *result)
{
    if (argc == 0)
    {
        sensorsOrientationLogEnable(TOGGLE);
    }
    else if (strncasecmp(argv[0], "on", 2) == 0)
    {
        sensorsOrientationLogEnable(true);
    }
    else if (strncasecmp(argv[0], "off", 3) == 0)
    {
        sensorsOrientationLogEnable(false);
    }
}

void cmdOrientationOn(unsigned int argc, char *argv[], int *result)
{
    if (argc == 0)
    {
        sensorsOrientationEnable(TOGGLE);
    }
    else if (strncasecmp(argv[0], "on", 2) == 0)
    {
        sensorsOrientationEnable(true);
    }
    else if (strncasecmp(argv[0], "off", 3) == 0)
    {
        sensorsOrientationEnable(false);
    }
}

void cmdLogGyro(unsigned int argc, char *argv[], int *result)
{
}


void cmdLogOrientationString(unsigned int argc, char *argv[], int *result)
{
}

void cmdLogBinary(unsigned int argc, char *argv[], int *result)
{
    if (argc == 0)
    {
        sensorsLogBinary(TOGGLE);
    }
    else if (strncasecmp(argv[0], "on", 2) == 0)
    {
        sensorsLogBinary(true);
    }
    else if (strncasecmp(argv[0], "off", 3) == 0)
    {
        sensorsLogBinary(false);
    }
}

void cmdNonProcess(unsigned int argc, char *argv[], int *result)
{

}

void cmdRequestSyncToken(unsigned int argc, char *argv[], int *result)
{
    Trace("#SYNCH%s", argv[0]);
}


extern const Command custCommands[] =
{
    {"jok",         "tell me a joke",                           cmdJoke},
    {"orion",       "enable orientation log",                   cmdLogOrientationOn},
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

