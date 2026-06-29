#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <algorithm>

#include "../include/analytics/AnalyticsTypes.h"
#include "../include/analytics/ProtocolStatistics.h"
#include "../include/analytics/IPStatistics.h"
#include "../include/analytics/PortStatistics.h"
#include "../include/analytics/DomainStatistics.h"

void testProtocolStatistics()
{
    ProtocolStatistics stats;

    stats.increment(ProtocolType::TCP);
    stats.increment(ProtocolType::TCP);
    stats.increment(ProtocolType::UDP);

    assert(stats.getCount(ProtocolType::TCP) == 2);
    assert(stats.getCount(ProtocolType::UDP) == 1);
    assert(stats.getCount(ProtocolType::ICMP) == 0);

    stats.decrement(ProtocolType::TCP);
    assert(stats.getCount(ProtocolType::TCP) == 1);

    stats.reset();
    assert(stats.getCount(ProtocolType::TCP) == 0);
    assert(stats.getAllStatistics().empty());

    std::cout << "ProtocolStatistics tests passed." << std::endl;
}

void testIPStatistics()
{
    IPStatistics stats;

    stats.increment("192.168.1.1");
    stats.increment("192.168.1.1");
    stats.increment("192.168.1.2");
    stats.increment("192.168.1.3");
    stats.increment("192.168.1.3");
    stats.increment("192.168.1.3");

    assert(stats.getCount("192.168.1.1") == 2);
    assert(stats.getCount("192.168.1.2") == 1);
    assert(stats.getCount("192.168.1.3") == 3);
    assert(stats.getCount("192.168.1.4") == 0);

    auto top2 = stats.getTopN(2);
    assert(top2.size() == 2);
    assert(top2[0].first == "192.168.1.3" && top2[0].second == 3);
    assert(top2[1].first == "192.168.1.1" && top2[1].second == 2);

    stats.clear();
    assert(stats.getCount("192.168.1.1") == 0);
    assert(stats.getTopN(1).empty());

    std::cout << "IPStatistics tests passed." << std::endl;
}

void testPortStatistics()
{
    PortStatistics stats;

    stats.increment(80);
    stats.increment(80);
    stats.increment(443);
    stats.increment(22);
    stats.increment(22);
    stats.increment(22);

    assert(stats.getCount(80) == 2);
    assert(stats.getCount(443) == 1);
    assert(stats.getCount(22) == 3);
    assert(stats.getCount(8080) == 0);

    auto top2 = stats.getTopN(2);
    assert(top2.size() == 2);
    assert(top2[0].first == 22 && top2[0].second == 3);
    assert(top2[1].first == 80 && top2[1].second == 2);

    std::cout << "PortStatistics tests passed." << std::endl;
}

void testDomainStatistics()
{
    DomainStatistics stats;

    stats.increment("google.com");
    stats.increment("Google.com"); // Should be normalized to lowercase
    stats.increment("example.org");
    stats.increment("example.org");
    stats.increment(""); // Should be ignored

    assert(stats.getCount("google.com") == 2);
    assert(stats.getCount("GOOGLE.COM") == 2);
    assert(stats.getCount("example.org") == 2);
    assert(stats.getCount("empty.com") == 0);

    auto top2 = stats.getTopN(2);
    assert(top2.size() == 2);
    assert(top2[0].second == 2);
    assert(top2[1].second == 2);
    // Order of google.com and example.org might vary based on map iteration, check counts
    if (top2[0].first == "google.com")
    {
        assert(top2[1].first == "example.org");
    }
    else
    {
        assert(top2[0].first == "example.org");
        assert(top2[1].first == "google.com");
    }

    stats.clear();
    assert(stats.getCount("google.com") == 0);
    assert(stats.getTopN(1).empty());

    std::cout << "DomainStatistics tests passed." << std::endl;
}

int main()
{
    testProtocolStatistics();
    testIPStatistics();
    testPortStatistics();
    testDomainStatistics();

    std::cout << "All Analytics Statistics tests completed." << std::endl;

    return 0;
}
