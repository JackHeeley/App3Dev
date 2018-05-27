#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utf8;

namespace UnitTestExtendedUniversalCppSupport
{

   ///<remarks> see: https://stackoverflow.com/questions/14807192/can-i-use-an-stdvector-as-a-facade-for-a-pre-allocated-raw-array </remarks>
   TEST_CLASS(UnitTestMemoryMappedFile)
   {
   public:

      TEST_METHOD(TestMemoryMappedFileConstructor)
      {
         try
         {
            // prepare for test...
            const std::string file_path("test.iso");
            const std::string buffer_name("test_buffer");
            uint64_t buffer_size(1024);
            // perform the operation under test (construct a memory mapped file with a constructor)...
            MemoryMappedFile mmf(file_path, buffer_name, buffer_size);

            // test succeeds if construction doesn't throw and...
            utf8::Assert::AreEqual(mmf.get_buffer_name(), buffer_name, "buffer_name retrieved doesn't match buffer_name supplied");
            utf8::Assert::AreEqual(mmf.get_file_path(), file_path, "file_path retrieved doesn't match file_path supplied");
            utf8::Assert::AreEqual(mmf.get_buffer_size(), buffer_size, "buffer_size retrieved doesn't match buffer_size supplied");
            try
            {
               auto buffer = mmf.get_buffer_address();
            }
            catch (...)
            {
               std::stringstream ss;
               SystemError lastError;
               ss << "get_buffer_address()  failed (" << lastError.get_error_code() << ") " << lastError.get_error_text();
               utf8::Assert::Fail(ss.str().c_str());
            }
         }
         catch (std::exception e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }      
      
      TEST_METHOD(TestMemoryMappedFileGetBuffer)
      {
         try
         {
            // prepare for test...
            const std::string file_path("test.iso");
            const std::string buffer_name("test_buffer");
            uint64_t buffer_size(1024);
            // perform the operation under test (construct a memory mapped file with a constructor)...
            MemoryMappedFile mmf(file_path, buffer_name, buffer_size);

            // test succeeds if construction doesn't throw and...
            utf8::Assert::AreEqual(mmf.get_buffer_name(), buffer_name, "buffer_name retrieved doesn't match buffer_name supplied");
            utf8::Assert::AreEqual(mmf.get_file_path(), file_path, "file_path retrieved doesn't match file_path supplied");
            utf8::Assert::AreEqual(mmf.get_buffer_size(), buffer_size, "buffer_size retrieved doesn't match buffer_size supplied");
            try
            {
               auto buffer = mmf.get_buffer();
            }
            catch (...)
            {
               std::stringstream ss;
               SystemError lastError;
               ss << "get_buffer()  failed (" << lastError.get_error_code() << ") " << lastError.get_error_text();
               utf8::Assert::Fail(ss.str().c_str());
            }
         }
         catch (std::exception e)
         {
            utf8::Assert::IsTrue(false, e.what()); // something went wrong
         }
      }

   };
}