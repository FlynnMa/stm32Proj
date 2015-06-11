/*
    Copyright (c) 2015 Mayunfei

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software
    and associated documentation files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge, publish, distribute,
    sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or
    substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
    BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef shell_h
#define shell_h

#include "mbed.h"
#include "rtos.h"

#define SHELL_MAX_ARGUMENTS    4

#define SHELL_MAX_LINE_LEN     20
/** Shell command interface implementation \n
    It link commands to functions. \n
    On each execution of a command (char array), it will parse the array and send all the parameters to the functions. \n
    Here is an quick example class that to show the purpose of this class.
    @code
    #include "shell.h"

    Serial pc(USBTX, USBRX);

    void echoCommand(unsigned int argc, char* argv[], char* result);

    int main() {
        CommandDispatcher disp = CommandDispatcher();
        char buffer[50];
        char result[50];
        int i=0;

        disp.addCommand("echo", echoCommand);
        pc.printf("Example Command Dispatcher\n\n");

        while(true) {
            buffer[i++] = pc.getc();
            if (buffer[i-1]=='\n') {
                buffer[i-1]='\0';
                i=0;

                if (disp.executeCommand(buffer, result)) {
                    pc.printf("%s\n", result);
                } else {
                    pc.printf("Command not found.\n");
                }
            }
        }
    }

    void echoCommand(unsigned int argc, char* argv[], char* result) {
        int i;
        sprintf(result, "");
        for (i=1; i<argc; i++) {
            sprintf(result, "%s %s", result, argv[i]);
        }
        sprintf(result, "%s\n", result);
    }
    @endcode
*/
struct Command {
    const char *name;
    const char *help;
    void(*function)(unsigned int argc, char* argv[], int *result);

};


class Shell {
public:
    /** Constructor of the Command Dispatcher
    */
    Shell();

    static Shell* instance(void);
    /** Set custom commands
    */
    void setCustCommand(const Command *pCustCmd);

//    void addCommand(char* commandName, void(*commandFunction)(unsigned int argc, char* argv[], char* result));

    /** Execute a command. It will check each registered command with its name.
        @param command the command with the arguments in a single char array.
        @param result a char array alocated by the user and sent to the function for feedback use.
        @return true if a registered command was found, else false.
    */
    bool executeCommand(char* command, char* result);

    const Command* findCommand(const char *pCommand);

    void process(void const *argument);

    bool m_exit;

    bool m_echo;

    const Command *m_custCmd;
    const Command *m_defaultCmd;

private:

//        const Command defaultCommands[];
        char     m_tmpCh;
        Thread  *m_pthread;
        Queue<uint32_t, 20> m_serialQ;

        int parse_buffer(char* buffer, char*** argv);

    /** A Utility function to transform a char array to lower case (useful for argument checks). \n
            WARNING : This function modifies the word.
        @param word the word to modify.
    */
    static void toLowerCase(char* word);

    void rxInterrupt(void);

//    void cmdHelp(unsigned int argc,char * argv [ ],int * result);

//    void cmdExit(unsigned int argc,char * argv [ ],int * result);


//    void listCommands(unsigned int argc,char * argv [ ],int * result);

};

#endif

