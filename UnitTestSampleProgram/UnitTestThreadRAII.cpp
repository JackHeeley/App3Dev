#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;
using namespace utf8;

namespace UnitTestExtendedUniversalCppSupport
{
   TEST_CLASS(UnitTestThreadRAII)
   {
   public:

      ///<summary>confirms the design without stressing it</summary>
      TEST_METHOD(TestThreadRAII)
      {
         try
         {
            {
               std::atomic<int> progress = 0;
               bool signal_done = false;

               auto show = [&progress, &signal_done]
               {
                  int percent = 0;
                  while (!signal_done && percent < 100)
                  {
                     std::this_thread::sleep_for(1s);
                     percent = progress;
                     std::cout << "\r" << "[" << std::string(percent / 5, '\xfe') << std::string(gsl::narrow_cast<size_t>(100 / 5 - percent / 5), ' ') << "]";
                     std::cout << percent << "%";
                     std::cout.flush();
                  }
                  std::cout << std::endl;
                  std::cout.flush();
               };

               thread_RAII r(std::thread(show), thread_RAII::DtorAction::join);

               while (progress < 100)
               {
                  std::this_thread::sleep_for(10ms);
                  progress++;
               }
               signal_done = true;
            }
         }
         catch (const std::exception & e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }
    };
}