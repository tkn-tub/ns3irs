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
#include "ns3/irs-lookup-helper.h"
#include "ns3/irs-propagation-loss-model.h"
#include "ns3/irs-spectrum-model.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/test.h"
#include "ns3/tuple.h"
#include "ns3/uinteger.h"

#include <cmath>
#include <iostream>
#include <optional>
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
 * \brief IrsPropagationLossModel Test. Results for different lookuptables are compared against pl
 * formular from ETSI report.
 *
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

    // Test vectors
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

    IrsLookupHelper irsHelper;
    irsHelper.SetDirection({0, 1, 0});

    Ptr<MobilityModel> a = CreateObject<ConstantPositionMobilityModel>();
    Ptr<MobilityModel> b = CreateObject<ConstantPositionMobilityModel>();
    Ptr<MobilityModel> irs = CreateObject<ConstantPositionMobilityModel>();
    irs->SetPosition({0, 0, 0});
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
}

/**
 * \ingroup irs-tests
 *
 * \brief IrsPropagationLossModel Test for helper functions
 */
class IrsPropagationLossModelHelperFunctionsTestCase : public TestCase
{
  public:
    IrsPropagationLossModelHelperFunctionsTestCase();
    ~IrsPropagationLossModelHelperFunctionsTestCase() override;

  private:
    void DoRun() override;
    void TestAngleCalculation();
    void TestAngle3DCalculation();
    void TestPathCalculation();
    int IrsPathsTest(const std::vector<Vector>& directions,
                     const std::vector<Vector>& positions,
                     const std::vector<double>& in_angles,
                     const std::vector<double>& out_angles);
};

IrsPropagationLossModelHelperFunctionsTestCase::IrsPropagationLossModelHelperFunctionsTestCase()
    : TestCase("Check if IrsPropagationLossModel helper functions are correct.")
{
}

IrsPropagationLossModelHelperFunctionsTestCase::~IrsPropagationLossModelHelperFunctionsTestCase()
{
}

void
IrsPropagationLossModelHelperFunctionsTestCase::TestAngleCalculation()
{
    // Angle Calculation - Test if wrong positions are returnt as -1,-1
    Ptr<IrsPropagationLossModel> model = CreateObject<IrsPropagationLossModel>();

    auto angles = model->CalcAngles({0, 0, 0}, {10, 10, 0}, {0, 5, 0}, {0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.has_value(), false, "A and B are on opposite sides of the IRS");

    angles = model->CalcAngles({-1, -1, 0}, {1, -1, 0}, {0, 0, 0}, {0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.has_value(), false, "A and B are on wrong side of the IRS");

    angles = model->CalcAngles({-1, -1, 0}, {1, 1, 0}, {0, 0, 0}, {0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.has_value(), false, "A is on wrong side of the IRS");

    angles = model->CalcAngles({-1, 1, 0}, {1, -1, 0}, {0, 0, 0}, {0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.has_value(), false, "B is on wrong side of the IRS");
    angles = model->CalcAngles({-1, 1, 0}, {1, 1, 0}, {0, 0, 0}, {0, -1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.has_value(), false, "A and B are on wrong side of the IRS");

    angles = model->CalcAngles({-1, 1, 0}, {1, -1, 0}, {0, 0, 0}, {0, -1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.has_value(), false, "A is on wrong side of the IRS");

    angles = model->CalcAngles({-1, -1, 0}, {1, 1, 0}, {0, 0, 0}, {0, -1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.has_value(), false, "B is on wrong side of the IRS");
}

void
IrsPropagationLossModelHelperFunctionsTestCase::TestAngle3DCalculation()
{
    Ptr<IrsPropagationLossModel> model = CreateObject<IrsPropagationLossModel>();

    auto angles = model->CalcAngles3D({-1, -1, 0}, {1, 1, 0}, {0, 1, 0});
    NS_TEST_EXPECT_MSG_EQ(angles.has_value(), false, "Node is on the wrong side of the IRS");
}

void
IrsPropagationLossModelHelperFunctionsTestCase::TestPathCalculation()
{
    // Test the Calculation of irs paths
    NS_TEST_EXPECT_MSG_EQ(IrsPathsTest({Vector(0, 1, 0), Vector(0, -1, 0)},
                                       {Vector(0.0, 0.0, 0.0), Vector(2.0, 2.0, 0.0)},
                                       {45, 45},
                                       {45, 45}),
                          4,
                          "Two IRS nodes facing each other");

    NS_TEST_EXPECT_MSG_EQ(IrsPathsTest({Vector(0, 1, 0), Vector(0, 1, 0)},
                                       {Vector(0.0, 0.0, 0.0), Vector(2.0, 2.0, 0.0)},
                                       {45, 45},
                                       {45, 45}),
                          2,
                          "Two IRS nodes not facing each other");

    NS_TEST_EXPECT_MSG_EQ(
        IrsPathsTest({Vector(0, 1, 0), Vector(0, -1, 0), Vector(0, -1, 0)},
                     {Vector(0.0, 0.0, 0.0), Vector(2.0, 2.0, 0.0), Vector(4.0, 2.0, 0.0)},
                     {135, 135, 116},
                     {45, 80, 76}),
        9,
        "Three IRS nodes two facing in same direction one in opposite");

    NS_TEST_EXPECT_MSG_EQ(
        IrsPathsTest({Vector(1, 1, 0), Vector(1, -1, 0), Vector(-1, -1, 0)},
                     {Vector(0.0, 0.0, 0.0), Vector(0.0, 2.0, 0.0), Vector(2.0, 2.0, 0.0)},
                     {135, 180, 135},
                     {0, 45, 18}),
        15,
        "Three IRS nodes facing each other");

    NS_TEST_EXPECT_MSG_EQ(
        IrsPathsTest({Vector(1, 1, 0), Vector(1, -1, 0), Vector(-1, -1, 0), Vector(-1, 1, 0)},
                     {Vector(0.0, 0.0, 0.0),
                      Vector(0.0, 2.0, 0.0),
                      Vector(2.0, 2.0, 0.0),
                      Vector(2.0, 0.0, 0.0)},
                     {135, 180, 135, 180},
                     {0, 45, 0, 45}),
        64,
        "Four IRS nodes facing each other");
}

int
IrsPropagationLossModelHelperFunctionsTestCase::IrsPathsTest(const std::vector<Vector>& directions,
                                                             const std::vector<Vector>& positions,
                                                             const std::vector<double>& in_angles,
                                                             const std::vector<double>& out_angles)

{
    NS_ASSERT_MSG(directions.size() == positions.size(),
                  "The number of directions must match the number of positions");
    NodeContainer irsNodes;
    irsNodes.Create(directions.size());

    Ptr<IrsPropagationLossModel> model = CreateObject<IrsPropagationLossModel>();
    // Create IRS helper and mobility helper
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    // Install IRS nodes based on the directions and positions
    for (size_t i = 0; i < directions.size(); ++i)
    {
        Ptr<IrsSpectrumModel> irs = CreateObjectWithAttributes<IrsSpectrumModel>(
            "Direction",
            VectorValue(directions[i]),
            "N",
            TupleValue<UintegerValue, UintegerValue>({50, 50}),
            "Spacing",
            TupleValue<DoubleValue, DoubleValue>({0.028770869289827, 0.028770869289827}),
            "Samples",
            UintegerValue(100),
            "Frequency",
            DoubleValue(5.21e9));
        irs->CalcRCoeffs(Angles(DegreesToRadians(in_angles[i]), DegreesToRadians(0)),
                         Angles(DegreesToRadians(out_angles[i]), DegreesToRadians(0)));
        irsNodes.Get(i)->AggregateObject(irs);

        positionAlloc->Add(positions[i]);
    }

    // Configure mobility
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(irsNodes);

    Ptr<PropagationLossModel> irsLossModel =
        CreateObjectWithAttributes<FriisPropagationLossModel>();
    Ptr<IrsPropagationLossModel> lossModel =
        CreateObjectWithAttributes<IrsPropagationLossModel>("IrsNodes",
                                                            PointerValue(&irsNodes),
                                                            "IrsLossModel",
                                                            PointerValue(irsLossModel));

    return lossModel->m_irsPaths.size();
}

void
IrsPropagationLossModelHelperFunctionsTestCase::DoRun()
{
    TestAngleCalculation();
    TestPathCalculation();
    TestAngle3DCalculation();
}

/**
 * \ingroup irs-tests
 *
 * \brief IrsPropagationLossModel TestSuite
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
    // AddTestCase(new IrsPropagationLossModelTestCase, TestCase::Duration::QUICK);
    AddTestCase(new IrsPropagationLossModelHelperFunctionsTestCase, TestCase::Duration::QUICK);
}

/// Static variable for test initialization
static IrsPropagationLossModelsTestSuite g_irsPropagationLossModelsTestSuite;
