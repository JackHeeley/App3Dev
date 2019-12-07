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
constexpr auto CP_UTF8 = 65001;
extern "C" __declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int wCodePageID) ;
#endif

#include <atomic>
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

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
   // on windows, switch platform console support to use the utf8 codepage 
   SetConsoleOutputCP(CP_UTF8);
   //std::cout << u8"καλή τύχη! Εύσχημα!" << std::endl;
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
         ///<summary>name of the device containing the media to be copied.</summary>
         auto deviceName = cdromDevices.device_path_map.get()[0];
         
         ///<summary>atomic int used to track progress.</summary>
         std::atomic<int> progress = 0;

         auto show_progress = [&progress] 
         {
            std::function<std::string(int)> progress_bar = [](int percent)
            {
               std::stringstream bar;
               bar << "\r" << "[";
               for (int i = 0; i < 20; i++)
               {
                  bar << ((i < percent / 5) ? u8"█" : " ");
               }
               bar << "]" << std::setw(3) << percent << "%";
               return bar.str();
            };

            while (progress<100)
            {
               std::this_thread::sleep_for(100ms);
               std::cout << progress_bar(progress);
            }
            std::cout << progress_bar(progress) << std::endl;
         };

         LOG_INFO("Launch the progress bar in a separate thread.");
         thread_RAII separate_thread(std::thread(show_progress), thread_RAII::DtorAction::detach);

         LOG_INFO("Do the ripping from the main thread.");
         Ripper(deviceName, fileName, progress)();

         // in the case where Ripper returns without an exception being thrown, if the progress indicator 
         // hasn't reached 100, then this is a programming error. The simplest handling for this scenario
         // is to allow the program to hang on the 'join' below, and find and fix the root cause.  

         LOG_INFO("Block until progress bar is finished.");
         separate_thread.get().join(); 
      }

      std::cout << "Ripping completed successfully. It is now safe to remove the CDROM device" << std::endl;
      LOG_INFO("Ripping completed successfully.");
      exit(0);
   }
   catch (const error::context& f)
   {
      std::string error_text = "Unhandled Error/Exception: "; error_text.append(f.full_what()); // fancy what
      //std::string error_text = "Unhandled Error/Exception: "; error_text.append(f.what());    // or simple what if you prefer
      std::cout << std::endl << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
   catch (const std::exception& e)
   {
      LOG_WARNING("A std::exception was thrown. (design intent is to always use error::context).");
      std::string error_text = "Unhandled Std Exception: "; error_text.append(e.what());   // simple what
      std::cout << std::endl << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
   catch (...)
   {
      LOG_WARNING("Something odd was thrown. (design intent is to always use error::context).");
      std::string error_text = "Unhandled exception, "; error_text.append(SystemError().get_error_text()); // best effort
      std::cout << std::endl << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
}

