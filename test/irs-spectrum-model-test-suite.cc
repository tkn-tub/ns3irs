/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/abort.h"
#include "ns3/config.h"
#include "ns3/core-module.h"
#include "ns3/irs-lookup-model.h"
#include "ns3/irs-lookup-table.h"
#include "ns3/irs-spectrum-model.h"
#include "ns3/log.h"
#include "ns3/node-container.h"
#include "ns3/test.h"
#include "ns3/tuple.h"
#include "ns3/uinteger.h"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

using namespace ns3;
using namespace std;
using namespace std::chrono;

NS_LOG_COMPONENT_DEFINE("IrsSpectrumModelTest");

class IrsSpectrumModelTestCase : public TestCase
{
  public:
    IrsSpectrumModelTestCase()
        : TestCase("Compare results of IrsSpectrumModel to lookup table generated in Matlab.")
    {
    }

  private:
    void DoRun() override;

    /// Test vector
    struct TestVector
    {
        double freq;
        double lambda;
        tuple<uint16_t, uint16_t> N;
        tuple<double, double> d;
        string lookuptable;
        uint8_t in_angle;
        uint8_t out_angle;
        double delta;
    };

    /// Test vectors
    TestVectors<TestVector> m_testVectors;
};

Ptr<IrsLookupTable>
SetLookupTable(string filename)
{
    // Load Lookup Table from csv file
    ifstream file(filename);
    NS_ABORT_MSG_IF(!file.is_open(), "IRS Lookup Table file not found.");

    // Create the lookup table
    Ptr<IrsLookupTable> irsLookupTable = CreateObject<IrsLookupTable>();

    string line;
    // Skip the header
    getline(file, line);

    // Read the data
    while (getline(file, line))
    {
        stringstream ss(line);
        string item;
        uint8_t in_angle, out_angle;
        double gain, phase_shift;

        // Read each value separated by comma
        getline(ss, item, ',');
        in_angle = stoi(item);
        getline(ss, item, ',');
        out_angle = stoi(item);
        getline(ss, item, ',');
        gain = stod(item);
        getline(ss, item, ',');
        phase_shift = stod(item);

        // Insert into the map
        irsLookupTable->Insert(in_angle, out_angle, gain, phase_shift);
    }

    file.close();
    return irsLookupTable;
}

void
IrsSpectrumModelTestCase::DoRun()
{
    Eigen::initParallel();
    TestVector tv;

    tv.freq = 5.21e9;
    tv.lambda = 0.057541738579655;
    tv.N = {20, 20};
    tv.d = {0.028770869289827, 0.028770869289827};
    tv.in_angle = 110;
    tv.out_angle = 69;
    tv.delta = M_PI;
    tv.lookuptable =
        "contrib/irs/examples/lookuptables/IRS_400_IN110_OUT69_FREQ5.21GHz_destructive.csv";
    m_testVectors.Add(tv);

    Ptr<IrsLookupModel> irsNormal = CreateObject<IrsLookupModel>();

    for (uint32_t i = 0; i < m_testVectors.GetN(); ++i)
    {
        tv = m_testVectors.Get(i);

        Ptr<IrsSpectrumModel> irs = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue({0, 1, 0}),
            "N",
            TupleValue<UintegerValue, UintegerValue>({get<0>(tv.N), get<1>(tv.N)}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({get<0>(tv.d), get<1>(tv.d)}),
            "Samples",
            UintegerValue(100),
            "Frequency",
            DoubleValue(tv.freq));
        irs->CalcRCoeffs(40,
                         42.7989,
                         Angles(DegreesToRadians(tv.in_angle), DegreesToRadians(0)),
                         Angles(DegreesToRadians(tv.out_angle), DegreesToRadians(0)),
                         tv.delta);

        irsNormal->SetDirection(Vector(0, 1, 0));
        irsNormal->SetLookupTable(SetLookupTable(tv.lookuptable));

        for (int i = 1; i < 180; ++i)
        {
            for (int j = 1; j < 180; ++j)
            {
                IrsEntry newEntry;
                IrsEntry oldEntry;
                newEntry = irs->GetIrsEntry(Angles(DegreesToRadians(i), DegreesToRadians(0)),
                                            Angles(DegreesToRadians(j), DegreesToRadians(0)),
                                            tv.freq);
                oldEntry = irsNormal->GetIrsEntry(i, j);

                NS_TEST_EXPECT_MSG_EQ_TOL(newEntry.gain, oldEntry.gain, 0.3, "Unexpected gain");
                NS_TEST_EXPECT_MSG_EQ_TOL(newEntry.phase_shift,
                                          oldEntry.phase_shift,
                                          0.05,
                                          "Unexpected gain");
            }
        }
    }
}

class IrsSpectrumModelTestCaching : public TestCase
{
  public:
    IrsSpectrumModelTestCaching()
        : TestCase("Test the performance of the caching of the IrsSpectrumModel")
    {
    }

    void DoRun() override
    {
        Ptr<IrsSpectrumModel> irs = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue({0, 1, 0}),
            "N",
            TupleValue<UintegerValue, UintegerValue>({20, 20}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({0.028770869289827, 0.028770869289827}),
            "Samples",
            UintegerValue(100),
            "Frequency",
            DoubleValue(5.21e9));
        irs->CalcRCoeffs(Angles(DegreesToRadians(135), DegreesToRadians(0)),
                         Angles(DegreesToRadians(45), DegreesToRadians(0)));

        double azimuth = 135.0;
        double elevation = 45.0;
        // Measure time for the first call (non-cached)
        auto start = high_resolution_clock::now();
        irs->GetIrsEntry(azimuth, elevation);
        auto stop = high_resolution_clock::now();
        auto uncachedDuration = duration_cast<nanoseconds>(stop - start).count();

        // Measure time for subsequent calls (cached)
        std::vector<long long> cachedDurations;
        for (int i = 0; i < 20; ++i)
        {
            start = high_resolution_clock::now();
            irs->GetIrsEntry(azimuth, elevation);
            stop = high_resolution_clock::now();
            cachedDurations.push_back(duration_cast<nanoseconds>(stop - start).count());
        }

        // Analyze results
        long long avgCachedDuration = 0;
        for (long long duration : cachedDurations)
        {
            avgCachedDuration += duration;
        }
        avgCachedDuration /= cachedDurations.size();

        // Print results
        NS_LOG_UNCOND("Time for first (non-cached) call: " << uncachedDuration << " ns");
        NS_LOG_UNCOND("Average time for cached calls: " << avgCachedDuration << " ns");

        // Test assertion to ensure caching is significantly faster
        NS_TEST_ASSERT_MSG_LT(avgCachedDuration,
                              uncachedDuration * 0.5,
                              "Cached calls should be significantly faster than uncached calls");
    }
};

class IrsSpectrumModelTestSuite : public TestSuite
{
  public:
    IrsSpectrumModelTestSuite();
};

IrsSpectrumModelTestSuite::IrsSpectrumModelTestSuite()
    : TestSuite("irs-spectrum-model", Type::UNIT)
{
    AddTestCase(new IrsSpectrumModelTestCase, TestCase::Duration::EXTENSIVE);
    AddTestCase(new IrsSpectrumModelTestCaching, TestCase::Duration::QUICK);
}

/// Static variable for test initialization
static IrsSpectrumModelTestSuite g_irsSpectrumModelTestSuite;
