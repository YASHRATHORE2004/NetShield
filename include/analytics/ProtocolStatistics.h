#pragma once

#include "AnalyticsTypes.h"
#include <unordered_map>
#include <vector>

class ProtocolStatistics
{
public:
    void increment(ProtocolType protocol)
    {
        protocolCounts[protocol]++;
    }

    void decrement(ProtocolType protocol)
    {
        auto it = protocolCounts.find(protocol);
        if (it != protocolCounts.end() && it->second > 0)
        {
            it->second--;
        }
    }

    void reset()
    {
        protocolCounts.clear();
    }

    uint32_t getCount(ProtocolType protocol) const
    {
        auto it = protocolCounts.find(protocol);
        if (it != protocolCounts.end())
        {
            return it->second;
        }
        return 0;
    }

    std::unordered_map<ProtocolType, uint32_t> getAllStatistics() const
    {
        return protocolCounts;
    }

private:
    std::unordered_map<ProtocolType, uint32_t> protocolCounts;
};
