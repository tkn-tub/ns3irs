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
#include "ns3/irs-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/simulator.h"
#include "ns3/test.h"

#include <cmath>
#include <string>

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
        double m_dt;     // distance tx -> irs
        double m_dx;     // distance irs -> rx
        double inAngle;  // radians
        double outAngle; // radians
        double size;
        std::string lookuptable;
    };

    /// Test vectors
    TestVectors<TestVector> m_testVectors;
};

IrsPropagationLossModelTestCase::IrsPropagationLossModelTestCase()
    : TestCase("Compare results of IrsPropagationLossModel to path loss formel specified by ETSI"),
      m_testVectors()
{
}

IrsPropagationLossModelTestCase::~IrsPropagationLossModelTestCase()
{
}

void
IrsPropagationLossModelTestCase::DoRun()
{
    double tolerance = 0;
    double frequency = 5.21e9;
    double txPowerDbm = 17;
    static const double c = 299792458.0;
    double lambda = c / frequency;

    TestVector tv;

    ns3::LogComponentEnable("IrsPropagationLossModel", ns3::LOG_LEVEL_ALL);
    tv.m_dt = 1.0;
    tv.m_dx = 1.0;
    tv.size = 400;
    tv.inAngle = 2.3562;
    tv.outAngle = 0.0349;
    tv.lookuptable =
        "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT88_FREQ5.21GHz_constructive.csv";
    m_testVectors.Add(tv);

    tv.m_dt = 1.0;
    tv.m_dx = 10.0;
    tv.size = 400;
    tv.inAngle = 2.3562;
    tv.outAngle = 0.0349;
    tv.lookuptable =
        "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT88_FREQ5.21GHz_constructive.csv";
    m_testVectors.Add(tv);

    tv.m_dt = 1.0;
    tv.m_dx = 30.0;
    tv.size = 400;
    tv.inAngle = 2.3562;
    tv.outAngle = 0.0349;
    tv.lookuptable =
        "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT88_FREQ5.21GHz_constructive.csv";
    m_testVectors.Add(tv);

    tv.m_dt = 60.0;
    tv.m_dx = 10.0;
    tv.size = 400;
    tv.inAngle = 2.3562;
    tv.outAngle = 0.0349;
    tv.lookuptable =
        "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT88_FREQ5.21GHz_constructive.csv";
    m_testVectors.Add(tv);

    tv.m_dt = 30.0;
    tv.m_dx = 30.0;
    tv.size = 400;
    tv.inAngle = 2.3562;
    tv.outAngle = 0.0349;
    tv.lookuptable =
        "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT88_FREQ5.21GHz_constructive.csv";
    m_testVectors.Add(tv);

    NodeContainer irsNode;
    irsNode.Create(1);

    IrsHelper irsHelper;
    irsHelper.SetDirection(Vector(0, 1, 0));

    Ptr<MobilityModel> a = CreateObject<ConstantPositionMobilityModel>();
    Ptr<MobilityModel> b = CreateObject<ConstantPositionMobilityModel>();
    Ptr<MobilityModel> irs = CreateObject<ConstantPositionMobilityModel>();
    irs->SetPosition(Vector(0, 0, 0));
    irsNode.Get(0)->AggregateObject(irs);

    Ptr<FriisPropagationLossModel> irsLossModel = CreateObject<FriisPropagationLossModel>();
    irsLossModel->SetFrequency(frequency);
    irsLossModel->SetSystemLoss(1);

    Ptr<IrsPropagationLossModel> lossModel = CreateObject<IrsPropagationLossModel>();
    lossModel->SetFrequency(frequency);
    lossModel->SetIrsNodes(&irsNode);
    lossModel->SetIrsPropagationModel(irsLossModel);

    /*
    The path loss for a IRS scenario in the far field case is given in:
    ETSI GR RIS 003 V1.1.1. "Reconfigurable Intelligent Surfaces (RIS); Communication Models,
    Channel Models, Channel Estimation and Evaluation Methodology."

    dt, dr: distance tx->irs, irs->rx
    size: irs size
    A = 1
    dx * dy = lambda^2 / (4 * pi) -- Size of an Isotropic Antenna (Björnson & Demir, 2024)
    */
    auto pl_etsi = [](double dt, double dr, double size, double lambda) {
        return std::pow((4 * M_PI * dt * dr) / (size * (std::pow(lambda, 2) / (4 * M_PI)) * 1), 2);
    };

    for (uint32_t i = 0; i < m_testVectors.GetN(); ++i)
    {
        tv = m_testVectors.Get(i);

        irsHelper.SetLookupTable(tv.lookuptable);
        irsHelper.Install(irsNode);

        a->SetPosition(Vector(tv.m_dt * std::cos(tv.inAngle), tv.m_dt * std::sin(tv.inAngle), 0));
        b->SetPosition(Vector(tv.m_dx * std::cos(tv.outAngle), tv.m_dx * std::sin(tv.outAngle), 0));
        double resultdBm = lossModel->CalcRxPower(txPowerDbm, a, b);
        double etsidBm =
            txPowerDbm + lossModel->DbmFromW(-pl_etsi(tv.m_dt, tv.m_dx, tv.size, lambda));

        NS_TEST_EXPECT_MSG_EQ_TOL(resultdBm, etsidBm, tolerance, "Got unexpected rcv power");
    }

    // out of range
    // irsHelper.SetLookupTable("contrib/irs/examples/lookuptables/IRS_400_IN135_OUT2_FREQ5.21GHz_constructive.csv");
    // irsHelper.Install(irsNode);
    //
    // a->SetPosition(Vector(1, 1, 0));
    // b->SetPosition(Vector(-1, 1, 0));
    // double resultdBm = lossModel->CalcRxPower(txPowerDbm, a, b);
    //
    // NS_TEST_ASSERT_MSG_EQ(std::isinf(resultdBm), true, "Should not be in LoS");
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
    int IrsPathsTest(const std::vector<Vector>& directions, const std::vector<Vector>& positions);
};

IrsPropagationLossModelHelperFunctionsTestCase::IrsPropagationLossModelHelperFunctionsTestCase()
    : TestCase("Check if IrsPropagationLossModel helper functions are correct.")
{
}

IrsPropagationLossModelHelperFunctionsTestCase::~IrsPropagationLossModelHelperFunctionsTestCase()
{
}

int
IrsPropagationLossModelHelperFunctionsTestCase::IrsPathsTest(const std::vector<Vector>& directions,
                                                             const std::vector<Vector>& positions)
{
    NS_ASSERT_MSG(directions.size() == positions.size(),
                  "The number of directions must match the number of positions");
    NodeContainer irsNodes;
    irsNodes.Create(directions.size());

    // Create IRS helper and mobility helper
    IrsHelper irsHelper;
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    // Install IRS nodes based on the directions and positions
    for (size_t i = 0; i < directions.size(); ++i)
    {
        irsHelper.SetDirection(directions[i]);
        irsHelper.SetLookupTable(
            "contrib/irs/examples/lookuptables/IRS_400_IN135_OUT88_FREQ5.21GHz_constructive.csv");
        irsHelper.Install(irsNodes.Get(i));

        positionAlloc->Add(positions[i]);
    }

    // Configure mobility
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(irsNodes);

    // Create and configure the propagation loss model
    Ptr<IrsPropagationLossModel> lossModel = CreateObject<IrsPropagationLossModel>();
    lossModel->SetIrsNodes(&irsNodes);

    return lossModel->m_irsPaths.size();
}

void
IrsPropagationLossModelHelperFunctionsTestCase::DoRun()
{
    Ptr<IrsPropagationLossModel> model = CreateObject<IrsPropagationLossModel>();

    // Angle Calculation - Test if wrong positions are returnt as -1,-1
    std::pair<double, double> angles =
        model->CalcAngles(Vector{0, 0, 0}, Vector{10, 10, 0}, Vector{0, 5, 0}, Vector{0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.first, -1, "A and B are on opposite sides of the IRS");

    angles =
        model->CalcAngles(Vector{-1, -1, 0}, Vector{1, -1, 0}, Vector{0, 0, 0}, Vector{0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.first, -1, "A and B are on wrong side of the IRS");

    angles =
        model->CalcAngles(Vector{-1, -1, 0}, Vector{1, 1, 0}, Vector{0, 0, 0}, Vector{0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.first, -1, "A is on wrong side of the IRS");

    angles =
        model->CalcAngles(Vector{-1, 1, 0}, Vector{1, -1, 0}, Vector{0, 0, 0}, Vector{0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.first, -1, "B is on wrong side of the IRS");
    angles =
        model->CalcAngles(Vector{-1, 1, 0}, Vector{1, 1, 0}, Vector{0, 0, 0}, Vector{0, -1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.first, -1, "A and B are on wrong side of the IRS");

    angles =
        model->CalcAngles(Vector{-1, 1, 0}, Vector{1, -1, 0}, Vector{0, 0, 0}, Vector{0, -1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.first, -1, "A is on wrong side of the IRS");

    angles =
        model->CalcAngles(Vector{-1, -1, 0}, Vector{1, 1, 0}, Vector{0, 0, 0}, Vector{0, -1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.first, -1, "B is on wrong side of the IRS");

    // Test the Calculation of irs paths
    NS_TEST_EXPECT_MSG_EQ(IrsPathsTest({Vector(0, 1, 0), Vector(0, -1, 0)},
                                       {Vector(0.0, 0.0, 0.0), Vector(2.0, 2.0, 0.0)}),
                          4,
                          "Two IRS nodes facing each other");

    NS_TEST_EXPECT_MSG_EQ(IrsPathsTest({Vector(0, 1, 0), Vector(0, 1, 0)},
                                       {Vector(0.0, 0.0, 0.0), Vector(2.0, 2.0, 0.0)}),
                          2,
                          "Two IRS nodes not facing each other");

    NS_TEST_EXPECT_MSG_EQ(
        IrsPathsTest({Vector(0, 1, 0), Vector(0, -1, 0), Vector(0, -1, 0)},
                     {Vector(0.0, 0.0, 0.0), Vector(2.0, 2.0, 0.0), Vector(4.0, 2.0, 0.0)}),
        9,
        "Three IRS nodes two facing in same direction on in opposite");

    NS_TEST_EXPECT_MSG_EQ(
        IrsPathsTest({Vector(1, 1, 0), Vector(1, -1, 0), Vector(-1, -1, 0)},
                     {Vector(0.0, 0.0, 0.0), Vector(0.0, 2.0, 0.0), Vector(2.0, 2.0, 0.0)}),
        15,
        "Three IRS nodes two facing in same direction on in opposite");

    NS_TEST_EXPECT_MSG_EQ(
        IrsPathsTest({Vector(1, 1, 0), Vector(1, -1, 0), Vector(-1, -1, 0), Vector(-1, 11, 0)},
                     {Vector(0.0, 0.0, 0.0),
                      Vector(0.0, 2.0, 0.0),
                      Vector(2.0, 2.0, 0.0),
                      Vector(2.0, 0.0, 0.0)}),
        64,
        "Three IRS nodes two facing in same direction on in opposite");
}

/**
 * \ingroup propagation-tests
 *
 * \brief Propagation models TestSuite
 *
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
