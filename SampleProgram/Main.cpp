//
// main.cpp : Defines the entry point for the console application.
//
// The program rips the content of the CD/DVD in the zero'th
// enumerated system device drive, and writes it to a (disk) file 
// with a fixed name.
//
// Copyright (c) 2017 Jack Heeley, all rights reserved
//
#include "stdafx.h"

#include "cd_rom_device.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

///<summary> the file logger in use.</summary>
static file_logger Logger("ripper.log", LogFilter::Full);

///<summary> number of times to test for an absent device.</summary>
const static int  MAX_RETRIES = 3;

///<summary> filename for ripped image</summary>
const static std::string fileName("cdrom_image.iso");

///<summary> RAII object used to lock the physical media tray door on CDROM.</summary>
class TrayDoorLock
{
private:
   ///<summary> the referenced cdrom.</summary>
   CdromDevice& m_cdr;

public:
   ///<summary> lock tray door at construction time.</summary>
   ///<param name='cdr'> a reference to the cdrom to lock.</param>
   TrayDoorLock(CdromDevice& cdr) : m_cdr(cdr) 
   { 
      m_cdr.lock(); 
   }

   ///<summary>unlock tray door in all return paths</summary>
   ~TrayDoorLock() noexcept
   {
      try
      {
         m_cdr.unlock();
      }
      catch (...)
      {
         LOG_ERROR("Exception in ~TrayDoorLock.");
      }
   }
};

///<summary> functor class to rip image.</summary>
class Ripper
{
private:
   ///<summary> the cdrom to be ripped.</summary>
   CdromDevice m_cdr;
   
   ///<summary> the (memory mapped) file to receive the (iso 9660) image.</summary>
   MemoryMappedFile m_mmf;

public:
   ///<summary> construct a ripper.</summary>
   ///<param name='devicePath'> the utf8 name of a raw system cdrom device containing media.</param>
   ///<param name='filePath'> the utf8 name of a file to receive the (iso 9660) image.</param>
   Ripper(std::string devicePath, std::string filePath) :
      m_cdr(devicePath), 
      m_mmf(filePath, "", m_cdr.get_image_size())
   { 
      LOG_INFO(std::string("Ripper Device ").append(devicePath));
      LOG_INFO(std::string("Ripping to ").append(filePath));
   }
   
   ///<summary> functor to perform the rip operation.</summary>
   void operator()()
   {
      TrayDoorLock lock(m_cdr);

#ifdef _WIN64
      LOG_INFO("64-bit. mmf_vector can accommodate all supported media (using mmf_vector).");
      m_cdr.get_image(m_mmf.get_buffer());
#else
      LOG_INFO("32-bit. mmf_vector size can't accommodate all media sizes (using raw pointer instead).");
      m_cdr.get_image(m_mmf.get_buffer_address(), m_cdr.get_image_size());
#endif

   }

};

///<summary> program entrypoint.</summary>
///<param name = "argc"> number of command line parameters (expected 1).</param>
///<param name = "argv"> array of supplied command line parameters (expect only argv[0] i.e. program path).</param>
///<returns> exit code 0 if no errors were encountered, or exit code -1 if an error occurred.</returns>
///<remarks> uses system pause and stdout to interact with user.</remarks>
int main(int argc, char* argv[])
{

   ////HINT for non-ascii programmers...
   //SetConsoleOutputCP(CP_UTF8);  // TODO: platform specific code (function is windows)
   //setvbuf(stdout, nullptr, _IOFBF, 1000);
   //std::cout << "\xce\xba\xce\xb1\xce\xbb\xce\xae\x20\xcf\x84\xcf\x8d\xcf\x87\xce\xb7\x21" << std::endl;

   try
   {
      LOG_INFO("Sample test program starting.");
      for(int i=0;i<MAX_RETRIES;i++)
      {
         if (!(DeviceDiscoverer(GUID_DEVINTERFACE_CDROM).device_path_map.get().empty())) break;

         std::cout << "Please attach an appropriate CDROM device to the system" << std::endl;
         std::system("pause");
      }

      DeviceDiscoverer cdromDevices(GUID_DEVINTERFACE_CDROM);  // TODO: platform specific code (GUID is windows)

      if (cdromDevices.device_path_map.get().empty())
      {
         LOG_ERROR("CDROM device is not detected/attached (program terminating).");
         std::cout << "CDROM device is not detected/attached (program terminating)" << std::endl;
         exit(1);
      }

      LOG_INFO("Ripping cd image from medium.");
      std::cout << "Ripping cd image from medium. Please wait..." << std::endl;

      auto deviceName = cdromDevices.device_path_map.get()[0];

      Ripper rip(deviceName, fileName);

      rip();

      std::cout << "Ripping completed successfully. It is now safe to remove the CDROM device" << std::endl;
      LOG_INFO("Ripping completed successfully.");
      exit(0);
   }
   catch (fox::exception& f)
   {
      std::string error_text = "Unhandled Fox Exception: "; error_text.append(f.what());   // fancy what
      std::cout << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
   catch (std::exception& e)
   {
      LOG_WARNING("A std::exception was thrown. (design intent is to always use fox::exception).");
      std::string error_text = "Unhandled Std Exception: "; error_text.append(e.what());   // simple what
      std::cout << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
   catch (...)
   {
      LOG_WARNING("Something odd was thrown. (design intent is to always use fox::exception).");
      std::string error_text = "Unhandled exception, "; error_text.append(SystemError().get_error_text()); // best effort
      std::cout << error_text << std::endl;
      LOG_ERROR(error_text);
      exit(-1);
   }
}
