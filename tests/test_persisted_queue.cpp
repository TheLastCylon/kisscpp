#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <vector>
#include <chrono>
#include "catch.hpp"

//#include <iostream>

//void function()
//{
//    long long number = 0;
//
//    for( long long i = 0; i != 2000000; ++i )
//    {
//       number += 5;
//    }
//}

//int main()
//{
//	auto t1 = std::chrono::high_resolution_clock::now();
//	function();
//	auto t2 = std::chrono::high_resolution_clock::now();
//
//	auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
//
//	std::cout << duration;
//	return 0;
//}

SCENARIO("Queued data correctly persisted", "[persisted_queue]")
{
	GIVEN("A queue")
	{
		std::vector<int> v(5);

		REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 5);

        WHEN("the size is increased") {
			v.resize(10);

			THEN("the size and capacity change") {
				REQUIRE(v.size() == 10);
				REQUIRE(v.capacity() >= 10);
			}
		}

        WHEN("the size is reduced") {
			v.resize( 0 );

			THEN("the size changes but not capacity") {
				REQUIRE( v.size() == 0 );
				REQUIRE( v.capacity() >= 5 );
			}
		}

        WHEN("more capacity is reserved") {
			v.reserve(10);

			THEN("the capacity changes but not the size") {
				REQUIRE(v.size() == 5);
				REQUIRE(v.capacity() >= 10);
			}
		}

        WHEN("less capacity is reserved") {
			v.reserve(0);
			THEN("neither size nor capacity are changed") {
				REQUIRE(v.size() == 5);
				REQUIRE(v.capacity() >= 5);
			}
		}
	}
}
