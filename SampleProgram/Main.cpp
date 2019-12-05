//
// main.cpp : Defines the entry point for the console application.
//
// The program rips the content of the CD/DVD in the zero'th
// enumerated system device drive, and writes it to a (disk) file 
// with a fixed name.
//
// Copyright (c) 2017-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
#include "stdafx.h"

#ifdef UNICODE 
#define CP_UTF8 65001
extern "C" __declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int wCodePageID);
#endif

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <chrono>
#include <future>

using namespace std::chrono_literals;

#pragma warning(disable:26426)

///<summary> declare the (single) file logger in use.</summary>
file_logger Logger("ripper.log", LogFilter::Full);

///<summary> filename for ripped image</summary>
const static std::string fileName("cdrom_image.iso");

#pragma warning(default:26426)

///<summary> number of times to test for an absent device.</summary>
constexpr static int  MAX_RETRIES = 3;

///<summary> program entrypoint.</summary>
///<param name = "argc"> number of command line parameters (expected 1).</param>
///<param name = "argv"> array of supplied command line parameters (expect only argv[0] i.e. program path).</param>
///<returns> exit code 0 if no errors were encountered, or exit code -1 if an error occurred.</returns>
///<remarks> uses system pause and stdout to interact with user.</remarks>
int main(int argc, char* argv[])
{

#ifdef UNICODE
   SetConsoleOutputCP(CP_UTF8);
   setvbuf(stdout, nullptr, _IOFBF, 1000);
   std::cout << u8"καλή τύχη! Εύσχημα!" << std::endl;
#endif

   try
   {
      LOG_INFO("Sample test program starting.");
      for(int i=0;i<MAX_RETRIES;i++)
      {
         if (!(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get().empty())) break;

         std::cout << "Please attach an appropriate CDROM device to the system" << std::endl;
         std::system("pause");
      }

      const DeviceDiscoverer cdromDevices(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);

      if (cdromDevices.device_path_map.get().empty())
      {
         LOG_ERROR("CDROM media not present or device not attached (program terminating).");
         std::cout << "CDROM media not present or device not attached (program terminating)" << std::endl;
         exit(1);
      }

      LOG_INFO("Ripping cd image from medium.");
      std::cout << "Ripping cd image from medium. Please wait..." << std::endl;

      {
         auto deviceName = cdromDevices.device_path_map.get()[0];
         std::atomic<int> progress = 0;
         bool signal_done = false;

         auto show = [&progress, &signal_done]
         {

            int percent = 0;
            while (!signal_done && percent < 100)
            {
               std::this_thread::sleep_for(1s);
               percent = progress;
               const std::string u8FullBlock(u8"█");

               std::string bar("");
               for (int i = 0; i < percent/5; i++) bar.append(u8FullBlock);
               std::cout << "\r" << "[" << bar << std::string(gsl::narrow_cast<size_t>(100/5 - percent/5), ' ') << "]";
               
               std::cout << percent << "%";
               std::cout.flush();
            }
            std::cout << std::endl;
            std::cout.flush();
         };

         thread_RAII r(std::thread(show), thread_RAII::DtorAction::join);

         Ripper(deviceName, fileName, progress)();
         signal_done = true;
      }

      std::cout << "Ripping completed successfully. It is now safe to remove the CDROM device" << std::endl;
      LOG_INFO("Ripping completed successfully.");
      exit(0);
   }
   catch (const error::context& f)
   {
      std::string error_text = "Unhandled Error/Exception: "; error_text.append(f.full_what()); // fancy what
      //std::string error_text = "Unhandled Error/Exception: "; error_text.append(f.what());    // or simple what if you prefer
      std::cout << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
   catch (const std::exception& e)
   {
      LOG_WARNING("A std::exception was thrown. (design intent is to always use error::context).");
      std::string error_text = "Unhandled Std Exception: "; error_text.append(e.what());   // simple what
      std::cout << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
   catch (...)
   {
      LOG_WARNING("Something odd was thrown. (design intent is to always use error::context).");
      std::string error_text = "Unhandled exception, "; error_text.append(SystemError().get_error_text()); // best effort
      std::cout << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
}

