#pragma once

#include "AnalyticsTypes.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

class PortStatistics
{
public:
    void increment(Port port)
    {
        portCounts[port]++;
    }

    uint32_t getCount(Port port) const
    {
        auto it = portCounts.find(port);
        if (it != portCounts.end())
        {
            return it->second;
        }
        return 0;
    }

    std::vector<std::pair<Port, uint32_t>> getTopN(size_t n) const
    {
        std::vector<std::pair<Port, uint32_t>> sortedCounts(portCounts.begin(), portCounts.end());
        std::sort(sortedCounts.begin(), sortedCounts.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
        if (n < sortedCounts.size())
        {
            sortedCounts.resize(n);
        }
        return sortedCounts;
    }

    void clear()
    {
        portCounts.clear();
    }

private:
    std::unordered_map<Port, uint32_t> portCounts;
};
