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
#include "ns3/constant-position-mobility-model.h"
#include "ns3/double.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/test.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("IrsPropagationLossModelsTest");

/**
 * \ingroup irs
 * \defgroup irs-test irsmodule tests
 */

/**
 * \ingroup irs-tests
 *
 * \brief FriisPropagationLossModel Test
 */
class IrsPropagationLossModelTestCase : public TestCase
{
  public:
    IrsPropagationLossModelTestCase();
    ~IrsPropagationLossModelTestCase() override;

  private:
    void DoRun() override;

    /// Test vector
    struct TestVector
    {
        Vector m_position;  //!< Test node position
        double m_pt;        //!< Tx power [dBm]
        double m_pr;        //!< Rx power [W]
        double m_tolerance; //!< Tolerance
    };

    /// Test vectors
    TestVectors<TestVector> m_testVectors;
};

IrsPropagationLossModelTestCase::IrsPropagationLossModelTestCase()
    : TestCase("Check to see that the ns-3 Friis propagation loss model provides correct received "
               "power"),
      m_testVectors()
{
}

IrsPropagationLossModelTestCase::~IrsPropagationLossModelTestCase()
{
}

void
IrsPropagationLossModelTestCase::DoRun()
{
    // The ns-3 testing manual gives more background on the values selected
    // for this test.  First, set a few defaults.

    // the test vectors have been determined for a wavelength of 0.125 m
    // which corresponds to a frequency of 2398339664.0 Hz in the vacuum
    // Config::SetDefault("ns3::FriisPropagationLossModel::Frequency", DoubleValue(2398339664.0));
    // Config::SetDefault("ns3::FriisPropagationLossModel::SystemLoss", DoubleValue(1.0));
    //
    // // Select a reference transmit power
    // // Pt = 10^(17.0206/10)/10^3 = .05035702 W
    // double txPowerW = 0.05035702;
    // double txPowerdBm = 10 * std::log10(txPowerW) + 30;
    //
    // //
    // // We want to test the propagation loss model calculations at a few chosen
    // // distances and compare the results to those we have manually calculated
    // // according to the model documentation.  The model reference specifies,
    // // for instance, that the received power at 100m according to the provided
    // // input power will be 4.98265e-10 W.  Since this value specifies the power
    // // to 1e-15 significance, we test the ns-3 calculated value for agreement
    // // within 5e-16.
    // //
    // TestVector testVector;
    //
    // testVector.m_position = Vector(100, 0, 0);
    // testVector.m_pt = txPowerdBm;
    // testVector.m_pr = 4.98265e-10;
    // testVector.m_tolerance = 5e-16;
    // m_testVectors.Add(testVector);
    //
    // testVector.m_position = Vector(500, 0, 0);
    // testVector.m_pt = txPowerdBm;
    // testVector.m_pr = 1.99306e-11;
    // testVector.m_tolerance = 5e-17;
    // m_testVectors.Add(testVector);
    //
    // testVector.m_position = Vector(1000, 0, 0);
    // testVector.m_pt = txPowerdBm;
    // testVector.m_pr = 4.98265e-12;
    // testVector.m_tolerance = 5e-18;
    // m_testVectors.Add(testVector);
    //
    // testVector.m_position = Vector(2000, 0, 0);
    // testVector.m_pt = txPowerdBm;
    // testVector.m_pr = 1.24566e-12;
    // testVector.m_tolerance = 5e-18;
    // m_testVectors.Add(testVector);
    //
    // // Now, check that the received power values are expected
    //
    // Ptr<MobilityModel> a = CreateObject<ConstantPositionMobilityModel>();
    // a->SetPosition(Vector(0, 0, 0));
    // Ptr<MobilityModel> b = CreateObject<ConstantPositionMobilityModel>();
    //
    // Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel>();
    // for (uint32_t i = 0; i < m_testVectors.GetN(); ++i)
    // {
    //     testVector = m_testVectors.Get(i);
    //     b->SetPosition(testVector.m_position);
    //     double resultdBm = lossModel->CalcRxPower(testVector.m_pt, a, b);
    //     double resultW = std::pow(10.0, resultdBm / 10.0) / 1000;
    //     NS_TEST_EXPECT_MSG_EQ_TOL(resultW,
    //                               testVector.m_pr,
    //                               testVector.m_tolerance,
    //                               "Got unexpected rcv power");
    // }
}

/**
 * \ingroup irs
 * \defgroup irs-test irsmodule tests
 */

/**
 * \ingroup irs-tests
 *
 * \brief FriisPropagationLossModel Test
 */
class IrsPropagationLossModelHelperFunctionsTestCase : public TestCase
{
  public:
    IrsPropagationLossModelHelperFunctionsTestCase();
    ~IrsPropagationLossModelHelperFunctionsTestCase() override;

  private:
    void DoRun() override;

    /// Test vector
    struct TestVector
    {
        Vector m_position;  //!< Test node position
        double m_pt;        //!< Tx power [dBm]
        double m_pr;        //!< Rx power [W]
        double m_tolerance; //!< Tolerance
    };

    /// Test vectors
    TestVectors<TestVector> m_testVectors;
};

IrsPropagationLossModelHelperFunctionsTestCase::IrsPropagationLossModelHelperFunctionsTestCase()
    : TestCase("Check if IrsPropagationLossModel helper functions are correct."),
      m_testVectors()
{
}

IrsPropagationLossModelHelperFunctionsTestCase::~IrsPropagationLossModelHelperFunctionsTestCase()
{
}

void
IrsPropagationLossModelHelperFunctionsTestCase::DoRun()
{
    Ptr<IrsPropagationLossModel> model = CreateObject<IrsPropagationLossModel>();

    std::pair<double, double> angles = model->CalcAngles(Vector{0, 0, 0}, Vector{0, 0, 0}, Vector{1, 0, 0}, Vector{0,1,0});
    NS_TEST_EXPECT_MSG_EQ_TOL(angles.first, angles.second, 0.1, "Angles not matching: " << angles.first << " | " << angles.second);

    angles = model->CalcAngles(Vector{0, 0, 0}, Vector{0, 0, 0}, Vector{0, 1, 0}, Vector{1,0,0});
    NS_TEST_EXPECT_MSG_EQ_TOL(angles.first, angles.second, 0.1, "Angles not matching: " << angles.first << " | " << angles.second);

    // opposite sides
    angles = model->CalcAngles(Vector{0, 0, 0}, Vector{0, 10, 0}, Vector{0, 5, 0}, Vector{1,0,0});
    NS_TEST_EXPECT_MSG_EQ_TOL(angles.first, -1, 0.1, "Angle should be -1: " << angles.first << " | " << angles.second);
    NS_TEST_EXPECT_MSG_EQ_TOL(angles.second, -1, 0.1, "Angle should be -1: " << angles.first << " | " << angles.second);

    // opposite sides
    angles = model->CalcAngles(Vector{0, 0, 0}, Vector{10, 10, 0}, Vector{0, 5, 0}, Vector{1,0,0});
    NS_TEST_EXPECT_MSG_EQ_TOL(angles.first, -1, 0.1, "Angle should be -1: " << angles.first << " | " << angles.second);
    NS_TEST_EXPECT_MSG_EQ_TOL(angles.second, -1, 0.1, "Angle should be -1: " << angles.first << " | " << angles.second);

    angles = model->CalcAngles(Vector{-9.3969, 3.4202, 0}, Vector{8.6603, 5.0000, 0}, Vector{0, 0, 0}, Vector{1,0,0});
    NS_TEST_EXPECT_MSG_EQ_TOL(angles.first, 160, 0.1, "Angle should be 160: " << angles.first << " | " << angles.second);
    NS_TEST_EXPECT_MSG_EQ_TOL(angles.second, 30, 0.1, "Angle should be 30: " << angles.first << " | " << angles.second);
}

/**
 * \ingroup propagation-tests
 *
 * \brief Propagation models TestSuite
 *
 * This TestSuite tests the following models:
 *   - FriisPropagationLossModel
 */
class IrsPropagationLossModelsTestSuite : public TestSuite
{
  public:
    IrsPropagationLossModelsTestSuite();
};

IrsPropagationLossModelsTestSuite::IrsPropagationLossModelsTestSuite()
    : TestSuite("irs-propagation-loss-model", Type::UNIT)
{
    AddTestCase(new IrsPropagationLossModelTestCase, TestCase::Duration::QUICK);
    AddTestCase(new IrsPropagationLossModelHelperFunctionsTestCase, TestCase::Duration::QUICK);
}

/// Static variable for test initialization
static IrsPropagationLossModelsTestSuite g_irsPropagationLossModelsTestSuite;
