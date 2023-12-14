#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include "include/LOG.h"

auto getTopApp() -> std::string;
auto printCurrentTime() -> std::string;
auto readProfile(const char *profile, std::vector<std::string> &apps)
{
    std::ifstream file(profile);
    std::string buf;
    while (std::getline(file, buf)) {
        if (buf[0] == '#' || buf.empty()) {
            continue;
        }
        apps.push_back(buf);
    }

    file.close();
}
// 记录同一app的使用时间 单位 秒
static int count = 0;
// 单位;秒
#define 超时保护时间 300
auto run(std::vector<std::string> &apps, std::string &now_package) -> bool
{
    std::string const TopApp = getTopApp();
    if (TopApp == now_package) {
        count++;
        if (count > 超时保护时间) {
            for (const auto &app : apps) {
                std::system(
                    ("nohup pm disable " + app + " >/dev/null 2>&1 &").c_str());
                LOG("已经保护: ", app);
                count = 0;
            }
        }
        return true;
    }
    LOG("时间: ", printCurrentTime());
    LOG("打开了软件: ", TopApp);
    now_package = TopApp;
    count = 0;

    return true;
}
auto runStart(std::vector<std::string> &apps, std::string &now_package) -> bool
{
    pthread_setname_np(pthread_self(), "HeavyThread");
    while (true) {
        run(apps, now_package);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
auto main(int argc, char **argv) -> int
{
    pthread_setname_np(pthread_self(), "MainThread");
    if (argv[1] == nullptr) {
        LOG("命令行参数没写");
        return 1;
    }

    std::vector<std::string> apps;
    readProfile(argv[1], apps);
    for (const auto &app : apps) {
        LOG("已经添加保护的app: ", app);
    }
    std::string now_package = "";
    std::thread HeavyThread(runStart, std::ref(apps), std::ref(now_package));
    HeavyThread.join();
    // runStart(apps, now_package);
}
