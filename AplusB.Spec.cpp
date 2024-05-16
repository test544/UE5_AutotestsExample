#include "AutotestingExample/AplusB/AplusB.h"

//
//Doc: https://docs.unrealengine.com/5.2/en-US/automation-spec-in-unreal-engine/
//

//> We have added a new type of automation test to our existing automation testing framework. This 
//> new type is known as a Spec. "Spec" is a term for a test that is built following the 
//> Behavior Driven Design (BDD, https://en.wikipedia.org/wiki/Behavior-driven_development) 
//> methodology. It is a very common methodology used in web development testing, which we
//> adapted to our C++ framework. 

//> Specs should be defined in a file with the .spec.cpp extension and not have the word "Test" in the name. 

//> There are two methods for defining the header for your spec, and both are similar to the
//> existing method we use to define test types. 

//> The easiest method is to use the DEFINE_SPEC macro, which takes the exact same parameters as all the rest of the test define macros.

//Note that specs cannot be smoke tests
DEFINE_SPEC(FAplusBSpecTestShort, "AutotestingExampleProject.AplusB.SpecTest.Short", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

void FAplusBSpecTestShort::Define()
{
	Describe("Sum testing", [this]()
	{
			It("Should correctly sum 0 with 0", [this]() {
				AplusB FeatureToTest;
				TestEqual("sum of two zeros", FeatureToTest.Sum(0, 0), 0 + 0);
			});

			It("Should correctly sum -100 with 100", [this]() {
				AplusB FeatureToTest;
				TestEqual("sum of two zeros", FeatureToTest.Sum(-100,100), -100 + 100);
			});

			It("Should correctly sum class members a&b", [this]() {
				AplusB FeatureToTest;
				TestEqual("sum of two zeros", FeatureToTest.Sum(FeatureToTest.a, FeatureToTest.b), FeatureToTest.a + FeatureToTest.b);
			});
	});
}

//> The only other alternative is to use the BEGIN_DEFINE_SPEC and END_DEFINE_SPEC macros. 
//> These macros allow you to define your own members as part of the test. As you will see 
//> in the next section, there is value to have things relative to the this pointer. 
BEGIN_DEFINE_SPEC(FAplusBSpecTestExpanded, "AutotestingExampleProject.AplusB.SpecTest.Expanded", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	TSharedPtr<AplusB> ObjectPtr;
	int c;
END_DEFINE_SPEC(FAplusBSpecTestExpanded)

void FAplusBSpecTestExpanded::Define()
{
	Describe("Sum testing", [this]()
	{
		BeforeEach([this]() {
			ObjectPtr = MakeShared<AplusB>();
			c = 13;
		});

		It("Should correctly sum 0 with 999", [this]() {
			if (!TestNotNull("AplusB object created in BeforeEach", ObjectPtr.Get()))
				return;

			TestEqual("Summing 0 with 999", ObjectPtr->Sum(0, 999), 0 + 999);
		});

		It("Should be overflow while trying to sum two numbers", [this]() {
			if (!TestNotNull("AplusB object created in BeforeEach", ObjectPtr.Get()))
				return;

			TestNotEqual("Summing maxint16 with 2", static_cast<int>(ObjectPtr->Sum(0x0000'7FFF, 0x0000'0001)), (0x0000'7FFF + 0x0000'0001));
		});

		xIt("Should not be called at all. x*() functions are a way to disable blocks of code in .Spec testing", [this]() {
			FString x = "Some test that not working properly yet";
			TestTrue("Test should not fail due it turn off", x.IsEmpty());
		});

		Describe("Nested testing", [this]()
		{
			//outer before\\after-each does affect nested blocks
			//inner before\\after-each does not affect outer tests
			BeforeEach([this]() {
				c = c + 10;
			});

			It("Checking c is 23 in nested describe block", [this]() {
				TestEqual("c is 23", c, 23);
			});
		});

		It("Class member still should be 13", [this]()
		{
			TestEqual("c is still 13", c, 13);
		});

		AfterEach([this]() {
			ObjectPtr = nullptr;
		});
	});
}

//> Latent Completion
//> Sometimes, you need to write a test needing to perform an action that takes multiple frames, such as when 
//> performing a query.In these scenarios, you can use the overloaded LatentBeforeEach(), LatentIt(), and 
//> LatentAfterEach() members.Each of these members are identical to the non - latent variations, except their 
//> lambdas take a simple delegate called Done.
//
//> When using the latent variations, the Spec test type will not continue execution to the next code block in 
//> the test sequence until the actively running latent code block invokes the Done delegate.
DEFINE_SPEC(FAplusBSpecTestLatent, "AutotestingExampleProject.AplusB.SpecTest.LatentExample", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

void FAplusBSpecTestLatent::Define()
{
	Describe("Latent(multiframe) test", [this]()
	{
		LatentIt("Should check sum of 1 & 1, but only a two seconds later", [this](const FDoneDelegate& Done) {
			TestNotNull("GEngine", GEngine);

			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Done](float _)->bool {
				Done.Execute();
				return false;
			}), 2.0f);

			AplusB FeatureToTest;
			TestEqual("sum of two ones", FeatureToTest.Sum(1, 1), 2);
		});
	});
}