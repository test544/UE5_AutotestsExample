
#include "AutotestingExample/AplusB/AplusB.h"
#include "Misc/AutomationTest.h"

//this is a useful utils for testings
#if WITH_EDITOR
#include "Tests/AutomationEditorCommon.h"
#endif

//
//Doc: https://docs.unrealengine.com/5.2/en-US/automation-technical-guide/
//

//> Automation Testing is the lowest level of automated testing, and is best suited for low-level 
//>  tests of the core functionality of the Engine. This system exists outside of the UObject 
//> ecosystem, so it is not visible to Blueprints or the Engine's Reflection System. 

//> Simple Tests
//> Simple Tests are used to describe single atomic tests, and are useful as unit or feature tests

//Simple test that always succeds
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAplusBSimpleTest, "AutotestingExampleProject.AplusB.SimpleTest.AplusB", EAutomationTestFlags::SmokeFilter | EAutomationTestFlags::ApplicationContextMask)

bool FAplusBSimpleTest::RunTest(const FString& Parameters)
{
	AplusB FeatureToTest;
	//There are plenty of Test*() functions, check them in AutomationTest.h
	return TestEqual("Sum() of our feature", FeatureToTest.Sum(11, 22), 11 + 22);
}

//Simple test that always fails
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAplusBSimpleTest_failed, "AutotestingExampleProject.AplusB.SimpleTest.AplusB failed", EAutomationTestFlags::SmokeFilter | EAutomationTestFlags::ApplicationContextMask)

bool FAplusBSimpleTest_failed::RunTest(const FString& Parameters)
{
	return TestNotEqual<int16>(TEXT("Intentionally failed Sum() check"), AplusB().Sum(11, 22), (11 + 11));
}

//> Complex Tests
//> Complex Tests are used to run the same code on a number of inputs. These are generally content
//> stress tests.For instance, loading all maps or compiling all Blueprints would be good fits for
//> complex automation tests.

//Complex test that checks A+B on several inputs
//Note: complex tests cannot be 'smoke test'
IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAplusBComplexTest, "AutotestingExampleProject.AplusB.ComplexTest.AplusB", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

void FAplusBComplexTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			OutBeautifiedNames.Add("Sum of " + FString::FromInt(i) + " and " + FString::FromInt(j));

			//Can probably toss stringified json in instead of custom formatted params
			OutTestCommands.Add(FString::FromInt(i) + "," + FString::FromInt(j) + "," + FString::FromInt(i + j));
		}
	}
}

bool FAplusBComplexTest::RunTest(const FString& Parameters)
{
	TArray<FString> List;
	FString Delimiter = ",";
	auto Len = Parameters.ParseIntoArray(List, *Delimiter, true);

	if (!TestEqual("Num of input params", Len, 3))
		return false;

	if (!TestTrue("A is numeric", List[0].IsNumeric()))
		return false;

	int A = FCString::Atoi(*List[0]);

	if (!TestTrue("B is numeric", List[1].IsNumeric()))
		return false;

	int B = FCString::Atoi(*List[1]);

	if (!TestTrue("Expected result is numeric", List[2].IsNumeric()))
		return false;

	int ExpectedResult = FCString::Atoi(*List[2]);

	return TestEqual("Test sum equal expected result", A + B, ExpectedResult);
}

//> Latent Commands
//> Latent Commands can be queued up during RunTest, causing sections of the code to run across multiple frames.
//
// Won't provide example on this, as A+B have no multiframe logic. If you really need it - check the doc above.
// Also recommend to check .Spec testing instead.
