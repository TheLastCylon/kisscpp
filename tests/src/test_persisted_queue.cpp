#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include "../catch.hpp"
#include "../kisscpp/persisted_queue.hpp"

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

//using namespace kisscpp;

//--------------------------------------------------------------------------------
class StringBiCoder : public kisscpp::Base64BiCoder<std::string>
{
  public:
    StringBiCoder() {};
    ~StringBiCoder() {};

    //--------------------------------------------------------------------------------
    virtual boost::shared_ptr<std::string> encode(const boost::shared_ptr<std::string> obj2encode)
    {
      return encodeToBase64String(*obj2encode);
    }

    //--------------------------------------------------------------------------------
    virtual boost::shared_ptr<std::string> decode(const std::string& str2decode)
    {
      boost::shared_ptr<std::string> retval = decodeFromBase64(str2decode);
      return retval;
    }
};

typedef kisscpp::PersistedQueue<std::string, StringBiCoder> StringQ;
typedef boost::shared_ptr<StringQ>                          SharedStringQ;
typedef boost::scoped_ptr<StringQ>                          ScopedStringQ;
typedef boost::shared_ptr<std::string>                      SharedString;

SCENARIO("Queued data correctly persisted", "[persisted_queue]")
{
	GIVEN("A queue")
	{
    SharedStringQ test_queue;
    test_queue.reset(new StringQ("test_queue", "/tmp/", 10));

    //--------------------------------------------------------------------------------
    WHEN("We push an item to the queue") {
      SharedString new_string;
      new_string.reset(new std::string("test string 1"));

      test_queue->push_back(new_string);

      THEN("The size should be 1") {
        REQUIRE(test_queue->size() == 1);
        test_queue->pop_front();
      }
    }

    //--------------------------------------------------------------------------------
    WHEN("We push 10 items to the queue") {
      SharedString new_string;
      new_string.reset(new std::string("test string 1"));

      for(unsigned short i = 0; i < 10; ++i) {
        test_queue->push_back(new_string);
      }

      THEN("The size should be 10") {
        REQUIRE(test_queue->size() == 10);
        for(unsigned short i = 0; i < 10; ++i) {
          test_queue->pop_front();
        }
      }
    }
    
    //--------------------------------------------------------------------------------
    WHEN("We push enough to have multiple pages on disk") {
      SharedString new_string;
      new_string.reset(new std::string("test string 1"));

      for(unsigned short i = 0; i < 50; ++i) {
        test_queue->push_back(new_string);
      }

      THEN("The size should still be correctly calculated") {
        REQUIRE(test_queue->size() == 50);
        for(unsigned short i = 0; i < 50; ++i) {
          test_queue->pop_front();
        }
      }
    }
  }
}
