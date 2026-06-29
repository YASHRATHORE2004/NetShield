#pragma once

#include "AnalyticsTypes.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype> // For std::tolower

class DomainStatistics
{
public:
    void increment(const DomainName& domain)
    {
        if (domain.empty())
        {
            return;
        }
        std::string normalizedDomain = domain;
        std::transform(normalizedDomain.begin(), normalizedDomain.end(), normalizedDomain.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        domainCounts[normalizedDomain]++;
    }

    uint32_t getCount(const DomainName& domain) const
    {
        auto it = domainCounts.find(domain);
        if (it != domainCounts.end())
        {
            return it->second;
        }
        return 0;
    }

    std::vector<std::pair<DomainName, uint32_t>> getTopN(size_t n) const
    {
        std::vector<std::pair<DomainName, uint32_t>> sortedCounts(domainCounts.begin(), domainCounts.end());
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
        domainCounts.clear();
    }

private:
    std::unordered_map<DomainName, uint32_t> domainCounts;
};
