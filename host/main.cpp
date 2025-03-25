#include "Host.h"
#include "Logger.h"

int main(int argc, char* argv[])
{
    Logger::instance()->init("nativeMCP");

    Host host;
    host.init();
    return host.run();
}
