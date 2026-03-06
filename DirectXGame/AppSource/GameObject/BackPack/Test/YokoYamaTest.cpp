#include "YokoYamaTest.h"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

std::unordered_map<std::string, nlohmann::json> YokoYamaTest::dataMap;


bool YokoYamaTest::Load(const std::string& path)
{
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
        return false;
    }

    nlohmann::json j;
    ifs >> j;

    dataMap[path] = j;
    return true;
}

void YokoYamaTest::LoadAll(const std::string& directoryPath)
{
    if (!std::filesystem::exists(directoryPath))
    {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.path().extension() == ".json")
        {
            Load(entry.path().string());
        }
    }
}


bool YokoYamaTest::Save(const std::string& path)
{
    auto it = dataMap.find(path);
    if (it == dataMap.end())
    {
        return false;
    }

    std::filesystem::path p(path);
    std::filesystem::create_directories(p.parent_path());

    std::ofstream ofs(path);
    if (!ofs.is_open())
    {
        return false;
    }

    ofs << it->second.dump(2);
    return true;
}

void YokoYamaTest::SaveAll()
{
    for (auto& [path, _] : dataMap)
    {
        Save(path);
    }
}
