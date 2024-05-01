
// Include a header file from your module to test.
#include "ns3/irs.h"

// An essential include is test.h
#include "ns3/test.h"

// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;

// Add a doxygen group for tests.
// If you have more than one test, this should be in only one of them.
/**
 * \defgroup irs-tests Tests for irs
 * \ingroup irs
 * \ingroup tests
 */

// This is an example TestCase.
/**
 * \ingroup irs-tests
 * Test case for feature 1
 */
class IrsTestCase1 : public TestCase
{
  public:
    IrsTestCase1();
    virtual ~IrsTestCase1();

  private:
    void DoRun() override;
};

// Add some help text to this case to describe what it is intended to test
IrsTestCase1::IrsTestCase1()
    : TestCase("Irs test case (does nothing)")
{
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
IrsTestCase1::~IrsTestCase1()
{
}

//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
IrsTestCase1::DoRun()
{
    // A wide variety of test macros are available in src/core/test.h
    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    // Use this one for floating point comparisons
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

// The TestSuite class names the TestSuite, identifies what type of TestSuite,
// and enables the TestCases to be run.  Typically, only the constructor for
// this class must be defined

/**
 * \ingroup irs-tests
 * TestSuite for module irs
 */
class IrsTestSuite : public TestSuite
{
  public:
    IrsTestSuite();
};

IrsTestSuite::IrsTestSuite()
    : TestSuite("irs", Type::UNIT)
{
    // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
    AddTestCase(new IrsTestCase1, Duration::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
/**
 * \ingroup irs-tests
 * Static variable for test initialization
 */
static IrsTestSuite sirsTestSuite;
