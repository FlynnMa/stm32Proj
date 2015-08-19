#include "shell.h"
#include "trace.h"

Serial serialDevice(USBTX, USBRX);

Shell *m_shellSingleton;

void listCommands(unsigned int argc,char * argv [ ],int * result)
{
    int i;

    Trace("following commands are supported:");

    i = 0;
    while(m_shellSingleton->m_defaultCmd[i].name != NULL)
    {
        Trace("%s\t##\t%s",
                m_shellSingleton->m_defaultCmd[i].name,
                m_shellSingleton->m_defaultCmd[i].help);
        i++;
    }

    if (NULL == m_shellSingleton->m_custCmd)
    {
        return;
    }

    Trace("\r\n");
    i = 0;
    while(m_shellSingleton->m_custCmd[i].name != NULL)
    {
        Trace("%s\t##\t%s",
                m_shellSingleton->m_custCmd[i].name,
                m_shellSingleton->m_custCmd[i].help);
        i++;
    }
}

void cmdHelp(unsigned int argc,char * argv [ ],int * result)
{
    Trace("Hi, what can I do for you?");
    TraceLine();
    listCommands(argc, argv, result);
    TraceLine();

    *result = true;

}

void cmdExit(unsigned int argc,char * argv [ ],int * result)
{
    Trace("Byebye!");
    *result = true;
    m_shellSingleton->m_exit = true;
}

void cmdArgTest(unsigned int argc,char * argv [ ],int * result)
{
    for(int i = 0; i < argc; i ++)
    {
        Trace("arg %d \t- %s", i, argv[i]);
    }
}

void cmdEcho(unsigned int argc,char * argv [ ],int * result)
{
    if (argc == 0)
    {
        m_shellSingleton->m_echo = !m_shellSingleton->m_echo;
    }
    else if (strncasecmp(argv[0], "on", 2) == 0)
    {
        m_shellSingleton->m_echo = true;
    }
    else if (strncasecmp(argv[0], "off", 3) == 0)
    {
        m_shellSingleton->m_echo = false;
    }
}

void Shell::rxInterrupt(void)
{
    uint32_t ch;

    while (serialDevice.readable()) {
        ch = (uint32_t)serialDevice.getc();
        m_serialQ.put((uint32_t*)ch);
    }
}


Shell* Shell::instance(void)
{
    if (m_shellSingleton != NULL)
    {
        return m_shellSingleton;
    }

    m_shellSingleton = new Shell;
    return m_shellSingleton;
}

const Command defaultCommands[] =
{
    {"help", "show help", cmdHelp},
    {"exit", "exit shell program", cmdExit},
    {"echo", "echo on or off", cmdEcho},
    {"arg",  "test arguments", cmdArgTest},
    {NULL, NULL, NULL} /* must end with empty command */
};


Shell::Shell():m_custCmd(NULL)
{
    m_exit = false;
    m_echo = true;
    m_defaultCmd = defaultCommands;
    serialDevice.baud(115200); //921600, 460800£¬ 225000, 115200
    serialDevice.attach(this, &Shell::rxInterrupt,Serial::RxIrq);

}

void Shell::setCustCommand(const Command * pCustCmd)
{
    m_custCmd = pCustCmd;
}

const Command* Shell::findCommand(const char *pCommand)
{
    int i;

    i = 0;
    while(m_defaultCmd[i].name != NULL)
    {
        if (strncasecmp(pCommand, m_defaultCmd[i].name, strlen(m_defaultCmd[i].name)) == 0)
        {
            return &m_defaultCmd[i];
        }
        i++;
    }

    if (m_custCmd == NULL)
        return NULL;

    i = 0;
    while(m_custCmd[i].name != NULL)
    {
        if (strncasecmp(pCommand, m_custCmd[i].name, strlen(m_custCmd[i].name)) == 0)
        {
            return &m_custCmd[i];
        }
        i++;
    }

    return NULL;
}

/**
 * @brief   Reads a whole line from the input channel.
 *
 * @param[in] chp       pointer to a @p BaseSequentialStream object
 * @param[in] line      pointer to the line buffer
 * @param[in] size      buffer maximum length
 * @return              The operation status.
 * @retval TRUE         the channel was reset or CTRL-D pressed.
 * @retval FALSE        operation successful.
 *
 * @api
 */
static bool getLine(char ch, char *line, unsigned size)
{
    char *p = line;
    const char *printStr = NULL;
    char buf[10];
    bool ret = false;

    memset(buf, 0, 4);
    while(*p != 0)
    {
        p++;
    }/* find the end */

    switch(ch)
    {
      case 4: //end of transmition
        printStr = "^D";
        memset(line, 0, size);
        ret = true;
        break;

      case 8:  //back space
        if (p > line)
        {
          snprintf(buf, 10, "%c%c%c", ch, 0x20, ch);
          printStr = buf;
          p--;
          *p = 0;
        }
        break;

      case '\r':
        printStr = "\r\n";
        *p = 0;
        ret = true;
        break;

      case 127: // delete
        if (p > line)
        {
//          TracePrint("%c", ch);
           snprintf(buf, 10, "%c", ch);
           printStr = buf;
           p--;
          *p = 0;
        }
        break;

      default:
        if ((ch >= 0x20) && (p < line + size - 1))
        {
//          TracePrint("%c", ch);
          *p = (char)ch;
          buf[0] = ch;
          printStr = buf;
        }
        break;
    }

    if (m_shellSingleton->m_echo == true)
    {
        TracePrint("%s", printStr);
    }

  return ret;
}

static char *_strtok(char *str, const char *delim, char **saveptr) {
  char *token;
  if (str)
    *saveptr = str;
  token = *saveptr;

  if (!token)
    return NULL;

  token += strspn(token, delim);
  *saveptr = strpbrk(token, delim);
  if (*saveptr)
    *(*saveptr)++ = '\0';

  return *token ? token : NULL;
}

void Shell::process(void const *argument)
{
    char ch;
    static char line[SHELL_MAX_LINE_LEN];
    bool ret;
    char *cmd, *argPos, *tok;
    int i;
    static char *args[SHELL_MAX_ARGUMENTS + 1];
    const Command *exec;
    int result;

    TraceLine();
    Trace("welcome to gBike");
    TraceLine();

    m_pthread = (Thread*)argument;
    memset(line, 0, SHELL_MAX_LINE_LEN);
    while(m_exit == false)
    {
        osEvent evt = m_serialQ.get();
        if (evt.status != osEventMessage)
        {
            Trace("queue->get() return %02x status", evt.status);
            continue;
        }
        ch = (char)evt.value.v;

        ret = getLine(ch, line, SHELL_MAX_LINE_LEN);
        if (false == ret)
            continue;

        cmd = _strtok(line, " \t", &tok);
        i = 0;
        while((argPos = _strtok(NULL," \t", &tok)) != NULL)
        {
            if (i >= SHELL_MAX_ARGUMENTS)
            {
                Trace("too many arguments!");
                cmd = NULL;
                break;
            }
            args[i++] = argPos;
        }
        args[i] = NULL;
        if (cmd == NULL)
        {
            goto loop;
        }
        exec = findCommand(cmd);
        if (exec)
        {
            exec->function(i, args, &result);
        }
        else
        {
            Trace("unknow command:%s", cmd);
        }

    loop:
        memset(line, 0, SHELL_MAX_LINE_LEN);
    }
}

