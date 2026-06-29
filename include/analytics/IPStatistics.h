#pragma once

#include "AnalyticsTypes.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

class IPStatistics
{
public:
    void increment(const IPAddress& ip)
    {
        ipCounts[ip]++;
    }

    uint32_t getCount(const IPAddress& ip) const
    {
        auto it = ipCounts.find(ip);
        if (it != ipCounts.end())
        {
            return it->second;
        }
        return 0;
    }

    std::vector<std::pair<IPAddress, uint32_t>> getTopN(size_t n) const
    {
        std::vector<std::pair<IPAddress, uint32_t>> sortedCounts(ipCounts.begin(), ipCounts.end());
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
        ipCounts.clear();
    }

private:
    std::unordered_map<IPAddress, uint32_t> ipCounts;
};
