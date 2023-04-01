#include "Memory.h"
#include <iostream>
#include <thread>

namespace offsets {
    constexpr ::std::ptrdiff_t dwClientState = 0x59F19C;
    constexpr ::std::ptrdiff_t dwClientState_Map = 0x28C;
};

int main()
{
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

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        mapName.clear();
        const auto dwClientState_Map = mem.Read<std::uintptr_t>(client + offsets::dwClientState_Map);
        for (int i = 0; i < 64; i++)
        {
            const char c = mem.Read<char>(dwClientState + offsets::dwClientState_Map + i);
            if (c == '\0')
            {
                break;
            }
            mapName += c;
        }
        std::cout << mapName<< std::endl;
    }
};

