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
#include "ns3/irs-helper.h"
#include "ns3/irs-spectrum-model.h"
#include "ns3/log.h"
#include "ns3/node-container.h"
#include "ns3/test.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("IrsSpectrumModelTest");

class IrsSpectrumModelTestCase : public TestCase
{
  public:
    IrsSpectrumModelTestCase();
    ~IrsSpectrumModelTestCase() override;

  private:
    void DoRun() override;

    /// Test vector
    struct TestVector
    {
        double freq;
        double lambda;
        std::tuple<uint16_t, uint16_t> N;
        std::tuple<double, double> d;
        std::string lookuptable;
        Angles in_angle;
        Angles out_angle;
        double delta;
    };

    /// Test vectors
    TestVectors<TestVector> m_testVectors;
};

IrsSpectrumModelTestCase::IrsSpectrumModelTestCase()
    : TestCase("Compare results of IrsPropagationLossModel to path loss formel specified by ETSI"),
      m_testVectors()
{
}

IrsSpectrumModelTestCase::~IrsSpectrumModelTestCase()
{
}

Ptr<IrsLookupTable>
SetLookupTable(std::string filename)
{
    // Load Lookup Table from csv file
    std::ifstream file(filename);
    NS_ABORT_MSG_IF(!file.is_open(), "IRS Lookup Table file not found.");

    // Create the lookup table
    Ptr<IrsLookupTable> irsLookupTable = CreateObject<IrsLookupTable>();

    std::string line;
    // Skip the header
    std::getline(file, line);

    // Read the data
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string item;
        uint8_t in_angle, out_angle;
        double gain, phase_shift;

        // Read each value separated by comma
        std::getline(ss, item, ',');
        in_angle = std::stoi(item);
        std::getline(ss, item, ',');
        out_angle = std::stoi(item);
        std::getline(ss, item, ',');
        gain = std::stod(item);
        std::getline(ss, item, ',');
        phase_shift = std::stod(item);

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
    tv.in_angle = Angles(DegreesToRadians(110), DegreesToRadians(0));
    tv.out_angle = Angles(DegreesToRadians(69), DegreesToRadians(0));
    tv.delta = M_PI;
    tv.lookuptable =
        "contrib/irs/examples/lookuptables/IRS_400_IN110_OUT69_FREQ5.21GHz_destructive.csv";
    m_testVectors.Add(tv);

    Ptr<IrsSpectrumModel> irs = CreateObject<IrsSpectrumModel>();
    Ptr<Irs> irsNormal = CreateObject<Irs>();

    for (uint32_t i = 0; i < m_testVectors.GetN(); ++i)
    {
        tv = m_testVectors.Get(i);

        irs->SetDirection(Vector(0, 1, 0));
        irs->SetSamples(100);
        irs->SetN(tv.N);
        irs->SetSpacing(tv.d);
        irs->SetFrequency(tv.freq);
        irs->SetIngoingAngles(tv.in_angle);
        irs->SetOutgoingAngles(tv.out_angle);
        irs->CalcRCoeffs(40, 42.7989, tv.delta);

        irsNormal->SetDirection(Vector(0, 1, 0));
        irsNormal->SetLookupTable(SetLookupTable(tv.lookuptable));

        for (int i = 1; i < 180; i += 5)
        {
            for (int j = 1; j < 180; j += 5)
            {
                IrsEntry newEntry;
                IrsEntry oldEntry;
                if ((i * j) % 333 == 0) // Sample every 20th run
                {
                    auto start = std::chrono::high_resolution_clock::now();

                    // Call the new function
                    newEntry = irs->GetIrsEntry(Angles(DegreesToRadians(i), DegreesToRadians(0)),
                                                Angles(DegreesToRadians(j), DegreesToRadians(0)),
                                                tv.freq);

                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> new_duration = end - start;

                    start = std::chrono::high_resolution_clock::now();

                    // Call the old function
                    oldEntry = irsNormal->GetIrsEntry(i, j);

                    end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> old_duration = end - start;

                    std::cout << "Sampled Angles (" << i << ", " << j << "): "
                              << "Matrix time: " << new_duration.count() << "s | "
                              << "Lookuptable time: " << old_duration.count() << "s" << std::endl;
                }
                else
                {
                    // Perform the comparison
                    newEntry = irs->GetIrsEntry(Angles(DegreesToRadians(i), DegreesToRadians(0)),
                                                Angles(DegreesToRadians(j), DegreesToRadians(0)),
                                                tv.freq);
                    oldEntry = irsNormal->GetIrsEntry(i, j);
                }

                NS_TEST_EXPECT_MSG_EQ_TOL(newEntry.gain, oldEntry.gain, 0.1, "Got unexpected gain");
                NS_TEST_EXPECT_MSG_EQ_TOL(newEntry.phase_shift,
                                          oldEntry.phase_shift,
                                          0.1,
                                          "Got unexpected gain");
                // std::cout << "Angles (" << i << " , " << j << "): " << newEntry.gain << " | "
                //           << newEntry.phase_shift << " -- " << oldEntry.gain << " | "
                //           << oldEntry.phase_shift << std::endl;
            }
        }
    }
}

class IrsSpectrumModelTestSuite : public TestSuite
{
  public:
    IrsSpectrumModelTestSuite();
};

IrsSpectrumModelTestSuite::IrsSpectrumModelTestSuite()
    : TestSuite("irs-spectrum-model", Type::UNIT)
{
    AddTestCase(new IrsSpectrumModelTestCase, TestCase::Duration::QUICK);
}

/// Static variable for test initialization
static IrsSpectrumModelTestSuite g_irsSpectrumModelTestSuite;
