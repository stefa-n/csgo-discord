#include "Memory.h"
#include "discord/discord.h"
#include <iostream>
#include <thread>

namespace offsets {
    constexpr ::std::ptrdiff_t dwClientState = 0x59F19C;
    constexpr ::std::ptrdiff_t dwClientState_Map = 0x28C;
};

int main()
{
    discord::Core* core{};
    auto res = discord::Core::Create(1091068583351681056, DiscordCreateFlags_Default, &core);
    discord::Activity activity{};
    activity.GetAssets().SetLargeImage("csgo");

    const auto mem = Memory("csgo.exe");
    const auto client = mem.GetModuleAddress("engine.dll");
    if (!client)
    {
        std::cout << "Run the program while CS:GO is running";
        return 0;
    }
    std::cout << "client.dll -> " << "0x" << std::hex << client << std::dec << std::endl;
    const auto dwClientState = mem.Read<std::uintptr_t>(client + offsets::dwClientState);
    std::cout << "dwClientState -> " << "0x" << std::hex << dwClientState << std::dec << std::endl;

    std::string mapName;
    std::string o_mapName;

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        mapName.clear();
        const auto dwClientState_Map = mem.Read<std::uintptr_t>(dwClientState + offsets::dwClientState_Map);
        for (int i = 0; i < 128; i++)
        {
            const char c = mem.Read<char>(dwClientState + offsets::dwClientState_Map + i);
            if (c == '\0')
            {
                break;
            }
            mapName += c;
        }
        if (mapName == "")
        {
            activity.SetState("In main menu");
            activity.SetDetails("");
        }
        else
        {
            activity.SetState(mapName.c_str());
            activity.SetDetails("In a match");
        }

        if (mapName != o_mapName)
        {
            std::cout << mapName << " " << o_mapName << std::endl;
            activity.GetTimestamps().SetStart(std::time(0));
            o_mapName = mapName;
        }

        core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
        core->RunCallbacks();
    }
};

