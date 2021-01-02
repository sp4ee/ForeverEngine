#include <src/Logic/cycle_ix.h>
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{
	TEST_CLASS(Test_cycle_ix)
	{
	public:
		TEST_METHOD(test_cmp_cycles)
		{
			Assert::IsTrue(0 == cmp_cycles(42, 42));
			Assert::IsTrue(-1 == cmp_cycles(41, 42));
			Assert::IsTrue(1 == cmp_cycles(43, 42));
			Assert::IsTrue(1 == cmp_cycles(1, INT16_MAX));
			Assert::IsTrue(-1 == cmp_cycles(INT16_MAX, 1));
		}

		TEST_METHOD(test_add_cycles)
		{
			Assert::IsTrue(3 == add_cycles(1, 2));
			Assert::IsTrue(0 == add_cycles(INT16_MAX, 1));
			Assert::IsTrue(1 == add_cycles(INT16_MAX, 2));
		}
	};
}
