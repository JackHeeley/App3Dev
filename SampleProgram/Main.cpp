//
// main.cpp : Defines the entry point for the console application.
//
// The program rips the content of the CD/DVD in the zero'th
// enumerated system device drive, and writes it to a (disk) file 
// with a fixed name.
//
// THE OBJECTIVE IS TO DEMONSTRATE ROBUST CODING PRACTICES WITH:
//
// #C++17 #STANDARDTEMPLATELIBRARY #GUIDLINESUPPORTLIBRARY #UTF8EVERYWHERE
// #SMARTLOGGING #STRUCTUREDEXCEPTIONHANDLING #UNITTESTING #VISUALSTUDIO
//
// Copyright (c) 2017-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/App3Dev
//
//    This program is free software : you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see < http://www.gnu.org/licenses/ >.
//
#include "stdafx.h"

#include <atomic>
#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <signal.h>


///<summary>forward reference to signal handler.</summary> 
void signal_handler(int signum);

///<summary> *** PROGRAM ENTRYPOINT ***.</summary>
///<param name = "argc"> number of command line parameters (expected 1).</param>
///<param name = "argv"> array of supplied command line parameters (expect only argv[0] i.e. program path).</param>
///<returns> exit code 0 if no errors were encountered, or exit code -1 if an error occurred.</returns>
///<remarks> uses system pause and stdout to interact with user.</remarks>
int main(int argc, char* argv[])
{

   ///<summary> create a file logger available to all components.</summary>
   CREATE_LOGGER(logger_factory::logger_type::file_logger, "ripper.log", DEFAULT_LOG_FILTER);

   ///<summary> filename for ripped image</summary>
   const static std::string fileName("cdrom_image.iso");

   ///<summary> number of times to test for an absent device.</summary>
   constexpr static int  MAX_RETRIES = 3;

   ///<summary>atomic int used to track progress.</summary>
   static std::atomic<int> progress = 0;
   
   ///<summary> a testable initialization value device name.</summary>
   const std::string NO_DEVICE("device not found");

   // Register handler for signals
   signal(SIGINT, signal_handler);     // CTRL-C
   signal(SIGBREAK, signal_handler);   // CTRL-BREAK & TERMINATE

   try
   {
      LOG_INFO(u8"Γειά σας Κόσμε! On Windows, switch platform console support to use the utf8 codepage");
      utf8::console::configure_codepage();
      std::cout << u8"Γειά σας Κόσμε!\n" << std::endl; // try it.

      std::cout << "SampleProgram.exe Copyright(c) 2019-2020 Jack Heeley.\n";
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

      LOG_INFO("Build a ripper, used to acquire the data");
      Ripper rip(deviceName);

      LOG_INFO("Build a tracker, for reporting progress");
      progress_tracker tracker(progress);

      LOG_INFO("Ripping image from optical disk.");
      std::cout << "Ripping image from optical disk. Please wait..." << std::endl;

      LOG_INFO("Launch the tracker as a task (shows progress bar on stdout).");
      auto future = std::async(std::launch::async, tracker);
         
      LOG_INFO("Do the ripping from the main thread.");
      rip(fileName, progress);

      Expects(progress == 100);   // if not, earlier versions of program would deadlock at join (still good to check)

      LOG_INFO("Block until progress tracker returns."); 
      if (!future.get()) 
      {
         LOG_WARNING("Progress thread stalled (did not finish normally).");
      }
      
      std::cout << "Ripping completed successfully. You may now remove the optical disk." << std::endl;
      LOG_INFO("Ripping completed successfully.");
      std::system("pause");
      exit(0);
   }

   // In addition to (obvious) main thread exceptions, exceptions thrown from the tracker task also propagate and are caught below.
   // This is (usually) exactly what we've always wanted for SEH, but was hithertofore difficult to arrange. With std::async it's built in.
   catch (const error::context& f) 
   {
      std::string error_text = "Unhandled Error/Exception: "; error_text.append(f.full_what()); // fancy what (root cause and locus of error)
      std::cout << std::endl << error_text << std::endl;
      std::system("pause");
      LOG_ERROR(error_text);
      exit(-1);
   }

   // Our design intent is to always use error::context. External libraries can't comply with all of our design requirements though, but will often 
   // follow best practice and raise std::exceptions. We therefore try to catch and handle these, and other error indications, at point of failure 
   // when calling platform library functions. If recovery isn't practical, we construct an error::context from the std::exception and throw it.
   // Here we we provide a last-ditch defence, to handle the possibility that this approach is not perfectly applied everywhere in the code...
   catch (const std::exception& e)
   {
      LOG_WARNING("A std::exception was thrown.");
      std::string error_text = "Unhandled Std Exception: "; error_text.append(e.what());   // simple what (a comment)
      std::cout << std::endl << error_text << std::endl;
      std::system("pause");
      LOG_ERROR(error_text);
      exit(-1);
   }

   // ...and this is the foxhole, behind the last-ditch, ensuring some kind of controlled program exit (if possible) no matter what was thrown.
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
