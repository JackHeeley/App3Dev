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
// FULL DOCUMENTATION AT https://github.com/JackHeeley/App3Dev/wiki
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

#include <future>

#include <signal.h>

///<summary>forward reference to signal handler.</summary> 
void signal_handler(int signum);

///<summary> *** PROGRAM ENTRYPOINT ***.</summary>
///<param name = "argc"> number of command line parameters (expected 1).</param>
///<param name = "argv"> array of supplied command line parameters (expect only argv[0] i.e. program path).</param>
///<returns> exit code EXIT_SUCCESS if no errors were encountered, or exit code EXIT_FAILURE if an error occurred.</returns>
///<remarks> uses system pause and stdout to interact with user.</remarks>
int main(int argc, char* argv[])
{
   ///<summary> create a file logger (available everywhere, including dll code).</summary>
   CREATE_LOGGER(logger_factory::logger_type::file_logger, "ripper.log", DEFAULT_LOG_FILTER);

   ///<summary>atomic int used to track progress.</summary>
   static std::atomic<int> progress = 0;
   
   // Register handler for signals
   signal(SIGINT, signal_handler);     // CTRL-C
   signal(SIGBREAK, signal_handler);   // CTRL-BREAK & TERMINATE

   try
   {
      LOG_INFO(U8("Γειά σας Κόσμε! On Windows, switch platform console support to use the utf8 codepage"));
      utf8::console::configure_codepage();
      std::cout << U8("Γειά σας Κόσμε!") << std::endl; // try it.
      std::cout << std::endl;
      std::cout << "SampleProgram.exe Copyright(c) 2019-2020 Jack Heeley." << std::endl;
      std::cout << "This program comes with ABSOLUTELY NO WARRANTY; for details refer to GPL 3.0." << std::endl;
      std::cout << "This is free software, and you are welcome to redistribute it" << std::endl;
      std::cout << "under certain conditions; refer to GPL 3.0 for details." << std::endl;
      std::cout << std::endl;
      std::cout << "You should have received a copy of the GNU General Public License (GPL 3.0)" << std::endl;
      std::cout << "along with this program. If not, see < http://www.gnu.org/licenses/" << " >." << std::endl;
      std::cout << std::endl;

      LOG_INFO("Sample test program starting.");
      
      LOG_INFO("Check if at least one optical reader is available (built-in, or currently attached to the system).");
      while (DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get().empty())
      {
         std::cout << "Please attach a suitable (e.g. usb) optical disk reader to the system. Press CTRL-BREAK to abort." << std::endl;
         std::system("pause");
      }

      LOG_INFO("Select the first such device, and check if an optical disk is loaded.");
      while (!CdromDevice::check_for_media((DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0])))
      {
         std::cout << "Please insert an optical disk into the (first) drive.  Press CTRL-BREAK to abort." << std::endl;
         std::system("pause");
      }

      LOG_INFO("A viable optical disk is now confirmed present in the (first) attached optical drive...");
      const std::string deviceName = DeviceDiscoverer(DeviceTypeDirectory::DeviceType::CDROM_DEVICES).device_path_map.get()[0];

      ///<summary>choose filename for ripped image</summary>
      const std::string fileName("cdrom_image.iso");

      // pre-empt starting a race on std::cout between main thread and progress tracker 
      std::cout << "Ripping image from optical disk. Please wait..." << std::endl;
      
      LOG_INFO("Build a ripper, used to acquire the data");
      Ripper rip(deviceName);

      LOG_INFO("Build a tracker, for reporting progress");
      progress_tracker tracker(progress);

      LOG_INFO("Launch the tracker as a task (shows progress bar on stdout).");
      auto future = std::async(std::launch::async, tracker);

      LOG_INFO("Ripping image from optical disk (done from main thread).");
      rip(fileName, progress);

      Expects(progress == 100);   // assert the expectation (in case code changes break design assumptions)

      LOG_INFO("Block until progress tracker returns.");
      if (!future.get())
      {
         LOG_WARNING("Progress thread stalled (did not finish normally).");
      }

      std::cout << "Ripping completed successfully. You may now remove the optical disk." << std::endl;
      LOG_INFO("Ripping completed successfully.");
      std::system("pause");
      exit(EXIT_SUCCESS);
   }

   // In addition to (obvious) main thread exceptions, exceptions thrown from the tracker task also propagate and are caught below.
   // With std::async this behaviour is built-in, because future.get() catches thread exceptions, and then rethrows them in the current thread context.
   catch (const error::context& f) 
   {
      std::string error_text = "Unhandled Error/Exception: "; error_text.append(f.full_what()); // fancy what (root cause and locus of error)
      std::cout << std::endl << error_text << std::endl;
      std::system("pause");
      LOG_ERROR(error_text);
      exit(EXIT_FAILURE);
   }

   // Our design intent is to always use error::context as the thrown object type during structured exception handling (SEH). But we can't impose that
   // requirement on the external libraries we choose to use. These libraries could do anything when stressed, including but not limited to best practice 
   // (i.e. raising std::exceptions). Our code aspires to manage all recoverable reported errors at the locus of the call/return, but will typically not 
   // anticipate or handle exceptions or other objects thrown from such libraries (noting particularly that aberrant behaviour might be undocumented). 
   // We make good effort to catch and handle these and other error indications, according to library documentation, at points of failure wherever we call
   // platform library functions. If recovery isn't practical, we aim to construct an error::context from any std::exception and rethrow it (with context). 
   // That would be caught above at the preceding catch statement. Here however, we provide a further defence, to handle the possibility that somewhere in
   // some untested path (now or in future) we fail to perfectly apply this treatment, and a std::exception slips through...
   catch (const std::exception& e)
   {
      LOG_WARNING("A std::exception was thrown. Our design intent (to use error::context) was compromised.");
      std::string error_text = "Unhandled Std Exception: "; error_text.append(e.what());   // simple what (a comment)
      std::cout << std::endl << error_text << std::endl;
      std::system("pause");
      LOG_ERROR(error_text);
      exit(EXIT_FAILURE);
   }

   // ...and extending this argument, this final catch ensures that, wherever possible, some kind of controlled program exit is made no matter object might
   // have been thrown. Note that we have no insight or control over what external libraries do, and they might not honour their contract.
   catch (...)
   {
      LOG_WARNING("Some unanticipated object was thrown. Our design intent (to use error::context) was compromised.");
      std::string error_text = "Unhandled exception, "; error_text.append(SystemError().get_error_text()); // best effort
      std::cout << std::endl << error_text << std::endl;
      std::system("pause");
      LOG_ERROR(error_text);
      exit(EXIT_FAILURE);
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
      std::cout << std::endl << error_text << std::endl << "If your drive supports locking you may need to reboot to retrieve the disk." << std::endl;
      LOG_ERROR(error_text);
   }
   exit(signum);
}
