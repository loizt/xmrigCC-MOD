#include <stdlib.h>
#include <string>

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN  /* avoid including junk */
    #include <windows.h>
    #include <signal.h>
	#include <chrono>
	#include <thread>
#else
    #include <sys/wait.h>
    #include <errno.h>
#endif

int main(int argc, char **argv) {

    std::string ownPath(argv[0]);

#if defined(_WIN32) || defined(WIN32)
    int pos = ownPath.rfind('\\');
    std::string xmrigMiner("xmrigMiner.exe");
#else
    int pos = ownPath.rfind('/');
    std::string xmrigMiner("xmrigMiner");
#endif

    std::string xmrigMinerPath = ownPath.substr(0, pos+1) + xmrigMiner;

#if defined(_WIN32) || defined(WIN32)
    xmrigMinerPath = "\"" + xmrigMinerPath + "\"";
#endif

    for (int i=1; i < argc; i++){
        xmrigMinerPath += " ";
        xmrigMinerPath += argv[i];
    }

    xmrigMinerPath += " --daemonized";

    int status = 0;

    do {
        status = system(xmrigMinerPath.c_str());
#if defined(_WIN32) || defined(WIN32)
    } while (status == EINTR);

	if (status == EINVAL) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
#else
    } while (WEXITSTATUS(status) == EINTR);
#endif
}

