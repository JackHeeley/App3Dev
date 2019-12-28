//
// main.cpp : Defines the entry point for the console application.
//
// The program rips the content of the CD/DVD in the zero'th
// enumerated system device drive, and writes it to a (disk) file 
// with a fixed name.
//
// Copyright (c) 2017-2019 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
//    This program is free software : you can redistribute itand /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see < http://www.gnu.org/licenses/>.
//
#include "stdafx.h"

// _UNICODE may be a proxy indicator that we are compiling for Windows (WIN32 isn't any more)
#ifdef _UNICODE 
#define WINDOWS 
#endif

#ifdef WINDOWS
// don't #include <windows.h> when compiling with /std:c++17 
constexpr auto CP_UTF8 = 65001;
extern "C" __declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int wCodePageID) ;
#endif

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <signal.h>

using namespace std::chrono_literals;

///<summary>forward reference to signal handler.</summary> 
void signal_handler(int signum);

///<summary> *** PROGRAM ENTRYPOINT ***.</summary>
///<param name = "argc"> number of command line parameters (expected 1).</param>
///<param name = "argv"> array of supplied command line parameters (expect only argv[0] i.e. program path).</param>
///<returns> exit code 0 if no errors were encountered, or exit code -1 if an error occurred.</returns>
///<remarks> uses system pause and stdout to interact with user.</remarks>
int main(int argc, char* argv[])
{

#ifdef _DEBUG
   ///<summary> create a file logger available to all components. Configure for ALL log entry types.</summary>
   CREATE_LOGGER(logger_factory::type::file_logger, "ripper.log", LogFilter::Full);
#else
   ///<summary> create a file logger available to all components. Configure only for errors and warnings.</summary>
   CREATE_LOGGER(logger_factory::type::file_logger, "ripper.log", LogFilter::Normal);
#endif

   ///<summary> filename for ripped image</summary>
   const static std::string fileName("cdrom_image.iso");

   ///<summary> number of times to test for an absent device.</summary>
   constexpr static int  MAX_RETRIES = 3;

   ///<summary> a testable initialization value device name.</summary>
   const std::string NO_DEVICE("device not found");

   // Register handler for signals (avoid a permanently locked optical drive)
   signal(SIGINT, signal_handler);     // CTRL-C
   signal(SIGBREAK, signal_handler);   // CTRL-BREAK & TERMINATE

   try
   {
#ifdef WINDOWS
      LOG_INFO(u8"Γειά σου Κόσμε! On Windows, switch platform console support to use the utf8 codepage");
      SetConsoleOutputCP(CP_UTF8);
      // std::cout << u8"Γειά σου Κόσμε!\n" << std::endl; // try it.
#endif

      std::cout << "SampleProgram.exe Copyright(c) 2019 Jack Heeley.\n";
      std::cout << "This program comes with ABSOLUTELY NO WARRANTY; for details refer to GPL 3.0.\n";
      std::cout << "This is free software, and you are welcome to redistribute it\n";
      std::cout << "under certain conditions; refer to GPL 3.0 for details.\n\n";

      std::cout << "You should have received a copy of the GNU General Public License (GPL 3.0)\n";
      std::cout << "along with this program. If not, see < http://www.gnu.org/licenses/" << " >.\n" << std::endl;

      LOG_INFO("Sample test program starting.");
      
      ///<summary>name of the device containing the media to be copied.</summary>
      std::string deviceName = NO_DEVICE;

      for(int i=0;i<MAX_RETRIES;i++)
      {
         const DeviceDiscoverer all_cdrom_readers(DeviceTypeDirectory::DeviceType::CDROM_DEVICES);

         LOG_INFO("Check if at least one reader is attached...");
         if (all_cdrom_readers.device_path_map.get().empty())
         {
            std::cout << "Please attach a suitable (e.g. usb) optical disk reader to the system" << std::endl;
            std::system("pause");
            continue;
         }

         LOG_INFO("Select the first device, and check if an optical disk is loaded...");
         if (!(CdromDevice::check_for_media(all_cdrom_readers.device_path_map.get()[0])))
         {
            std::cout << "Please insert an optical disk into the drive (and wait for it to spin up)." << std::endl;
            std::system("pause");
            continue;
         }

         LOG_INFO("A viable optical disk is present in the (first) attached drive...");
         deviceName = all_cdrom_readers.device_path_map.get()[0];
         break;
      }

      LOG_INFO("Check that we are all ready now.");
      if (!deviceName.compare(NO_DEVICE))
      {
         LOG_ERROR("Optical disk not loaded or drive not attached (program terminating).");
         std::cout << "Optical disk not loaded or drive not attached (program terminating)" << std::endl;
         exit(1);
      }

      LOG_INFO("Ripping image from optical disk.");
      std::cout << "Ripping image from optical disk. Please wait..." << std::endl;
      {
         
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
         RAII_thread separate_thread(std::thread(show_progress), RAII_thread::DtorAction::detach);

         ///<summary>Ripper used to acquire the data</summary>
         Ripper rip(deviceName);

         LOG_INFO("Do the ripping from the main thread.");
         rip(fileName, progress);

         Expects(progress == 100);   // if not, program would deadlock at join

         LOG_INFO("Block until progress bar is finished.");
         separate_thread.get().join();
      }

      std::cout << "Ripping completed successfully. You may now remove the optical disk." << std::endl;
      LOG_INFO("Ripping completed successfully.");
      std::system("pause");
      exit(0);
   }
   catch (const error::context& f)
   {
      std::string error_text = "Unhandled Error/Exception: "; error_text.append(f.full_what()); // fancy what
      //std::string error_text = "Unhandled Error/Exception: "; error_text.append(f.what());    // or simple what if you prefer
      std::cout << std::endl << error_text << std::endl;
      std::system("pause");
      LOG_ERROR(error_text);
      exit(-1);
   }
   catch (const std::exception& e)
   {
      LOG_WARNING("A std::exception was thrown. (design intent is to always use error::context).");
      std::string error_text = "Unhandled Std Exception: "; error_text.append(e.what());   // simple what
      std::cout << std::endl << error_text << std::endl;
      std::system("pause");
      LOG_ERROR(error_text);
      exit(-1);
   }
   catch (...)
   {
      LOG_WARNING("Something odd was thrown. (design intent is to always use error::context).");
      std::string error_text = "Unhandled exception, "; error_text.append(SystemError().get_error_text()); // best effort
      std::cout << std::endl << error_text << std::endl;
      std::system("pause");
      LOG_ERROR(error_text);
      exit(-1);
   }
}

///<summary>signal handler to manage the locked state of an optical drive.</summary> 
///<remarks>User initiated program aborts (CTRL-C CTRL-BREAK and TERMINATE signals) risk
/// the optical drive door being left in a permanently locked state.</remarks>
void signal_handler(int signum)
{
   try
   {
      std::string reason("Program was interrupted (by user action)! Code ");
      reason.append(std::to_string(signum));
      LOG_WARNING(reason);
      CdromDevice(DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0]).unlock();
   }
   catch (...)
   {
      std::string error_text = "Couldn't unlock the optical drive.";
      std::cout << std::endl << error_text << "\nIf your drive supports locking you may need to reboot to retrieve the disk." << std::endl;
      LOG_ERROR(error_text);
   }
   exit(signum);
}
