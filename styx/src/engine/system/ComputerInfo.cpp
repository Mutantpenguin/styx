#include "ComputerInfo.hpp"

#ifdef __linux__
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_cpuinfo.h>
#elif _WIN32
	#include <SDL.h>
	#include <SDL_cpuinfo.h>
#else
	#error "unsupported platform"
#endif


#ifdef WIN32
	#include <windows.h>
#else
	#include <limits>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <errno.h>
	#include <sys/sysinfo.h>
#endif

#include "src/ext/fmt/format.h"

#include "src/engine/logger/CLogger.hpp"

#include "src/engine/helper/String.hpp"

namespace ComputerInfo
{
	std::string OsName()
	{
		#ifdef WIN32
			std::string osversion = std::string( "unknown Windows" );

			OSVERSIONINFOEX	osinfo;
			ZeroMemory( &osinfo, sizeof( OSVERSIONINFOEX ) );
			osinfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );

			if( GetVersionEx( reinterpret_cast< OSVERSIONINFO* >( &osinfo ) ) )
			{
				SYSTEM_INFO sysinfo;
				GetSystemInfo( &sysinfo );

				switch( osinfo.dwPlatformId )
				{
				case VER_PLATFORM_WIN32_NT :
					if( 6 == osinfo.dwMajorVersion )
					{
						if( 0 == osinfo.dwMinorVersion )
						{
							if( VER_NT_WORKSTATION == osinfo.wProductType )
							{
								osversion = std::string( "Windows Vista " );
							}
							else
							{
								osversion = std::string( "Windows Server \"Longhorn\" " );
							}
						}
						else if( 1 == osinfo.dwMinorVersion )
						{
							osversion = std::string( "Windows 7 " );
						}
						else if( 2 == osinfo.dwMinorVersion )
						{
							osversion = std::string( "Windows 8 " );
						}
					}
					else if( 5 == osinfo.dwMajorVersion )
					{
						if( 0 == osinfo.dwMinorVersion )
						{
							osversion = std::string( "Microsoft Windows 2000 " );
						}
						else if( 1 == osinfo.dwMinorVersion )
						{
							osversion = std::string( "Microsoft Windows XP " );
						}
						else if( 2 == osinfo.dwMinorVersion )
						{
							if( GetSystemMetrics( 89 ) ) // stands for "SM_SERVERR2"
							{
								osversion = std::string( "Microsoft Windows Server 2003 \"R2\" ");
							}
							else if( ( VER_NT_WORKSTATION == osinfo.wProductType ) && ( PROCESSOR_ARCHITECTURE_AMD64 == sysinfo.wProcessorArchitecture ) )
							{
								osversion = std::string( "Microsoft Windows XP Professional x64 Edition ");
							}
							else
							{
								osversion = std::string( "Microsoft Windows Server 2003, " );
							}
						}
					}
					else if( osinfo.dwMajorVersion <= 4 )
					{
						osversion = std::string( "Microsoft Windows NT " );
					}

					if( ( VER_NT_WORKSTATION == osinfo.wProductType ) && ( PROCESSOR_ARCHITECTURE_AMD64 != sysinfo.wProcessorArchitecture ) )
					{
						if( 4 == osinfo.dwMajorVersion )
						{
							osversion += std::string( "Workstation 4.0 " );
						}
						else if( osinfo.wSuiteMask & VER_SUITE_PERSONAL )
						{
							osversion += std::string( "Home Edition " );
						}
						else
						{
							osversion += std::string( "Professional " );
						}
					}
					else if( ( VER_NT_SERVER == osinfo.wProductType )
							  ||
							  ( VER_NT_DOMAIN_CONTROLLER == osinfo.wProductType ) )
					{
						if( ( 5 == osinfo.dwMajorVersion ) && ( 2 == osinfo.dwMinorVersion ) )
						{
							if( PROCESSOR_ARCHITECTURE_IA64 == sysinfo.wProcessorArchitecture )
							{
								if( osinfo.wSuiteMask & VER_SUITE_DATACENTER )
								{
									osversion += std::string( "Datacenter Edition for Itanium-based Systems" );
								}
								else if( osinfo.wSuiteMask & VER_SUITE_ENTERPRISE )
								{
									osversion += std::string( "Enterprise Edition for Itanium-based Systems" );
								}
							}
							else if( PROCESSOR_ARCHITECTURE_AMD64 == sysinfo.wProcessorArchitecture )
							{
								if( osinfo.wSuiteMask & VER_SUITE_DATACENTER )
								{
									osversion += std::string( "Datacenter x64 Edition " );
								}
								else if( osinfo.wSuiteMask & VER_SUITE_ENTERPRISE )
								{
									osversion += std::string( "Enterprise x64 Edition " );
								}
								else
								{
									osversion += std::string( "Standard x64 Edition " );
								}
							}
							else
							{
								if( osinfo.wSuiteMask & VER_SUITE_DATACENTER )
								{
									osversion += std::string( "Datacenter Edition " );
								}
								else if( osinfo.wSuiteMask & VER_SUITE_ENTERPRISE )
								{
									osversion += std::string( "Enterprise Edition " );
								}
								else if( VER_SUITE_BLADE == osinfo.wSuiteMask )
								{
									osversion += std::string( "Web Edition " );
								}
								else
								{
									osversion += std::string( "Standard Edition " );
								}
							}
						}
						else if( ( 5 == osinfo.dwMajorVersion ) && ( 0 == osinfo.dwMinorVersion ) )
						{
							if( osinfo.wSuiteMask & VER_SUITE_DATACENTER )
							{
								osversion += std::string( "Datacenter Server " );
							}
							else if( osinfo.wSuiteMask & VER_SUITE_ENTERPRISE )
							{
								osversion += std::string( "Advanced Server " );
							}
							else
							{
								osversion += std::string( "Server " );
							}
						}
						else
						{
							if( osinfo.wSuiteMask & VER_SUITE_ENTERPRISE )
							{
								osversion += std::string("Server 4.0, Enterprise Edition " );
							}
							else
							{
								osversion += std::string( "Server 4.0 " );
							}
						}
					}
					else
					{
						HKEY hKey;
						TCHAR szProductType[ 80 ];
						DWORD dwBufLen = 80 * sizeof( TCHAR );
						LONG lRet;

						lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT( "SYSTEM\\CurrentControlSet\\Control\\ProductOptions" ), 0, KEY_QUERY_VALUE, &hKey );
						if( lRet != ERROR_SUCCESS )
						{
							return FALSE;
						}
						lRet = RegQueryValueEx( hKey, TEXT( "ProductType" ), nullptr, nullptr, reinterpret_cast< LPBYTE >( szProductType ), &dwBufLen);
						RegCloseKey( hKey );

						if( ( lRet != ERROR_SUCCESS )
							||
							( dwBufLen > 80 * sizeof( TCHAR ) ) )
						{
							return FALSE;
						}

						if( 0 == lstrcmpi( TEXT( "WINNT" ), szProductType ) )
						{
							osversion += std::string(  "Workstation " );
						}
						if( 0 == lstrcmpi( TEXT( "LANMANNT" ), szProductType ) )
						{
							osversion += std::string(  "Server " );
						}
						if( 0 == lstrcmpi( TEXT( "SERVERNT" ), szProductType ) )
						{
							osversion += std::string(  "Advanced Server " );
						}
						osversion += std::string(  "%d.%d ", osinfo.dwMajorVersion, osinfo.dwMinorVersion );
					}

					// Display service pack (if any) and build number.
					if( ( 4 == osinfo.dwMajorVersion ) && ( 0 == lstrcmpi( osinfo.szCSDVersion, TEXT( "Service Pack 6" ) ) ) )
					{
						HKEY hKey;

						// Test for SP6 versus SP6a.
						if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009" ), 0, KEY_QUERY_VALUE, &hKey ) )
						{
							osversion += std::string(  "Service Pack 6a (Build %d)\n", osinfo.dwBuildNumber & 0xFFFF );
						}
						else // Windows NT 4.0 prior to SP6a
						{
							std::stringstream strm;
							strm << ( osinfo.dwBuildNumber & 0xFFFF );
							osversion += std::string( osinfo.szCSDVersion ) + std::string( " (Build " ) + strm.str() + std::string( ")\n"  );
						}

						RegCloseKey( hKey );
					}
					else // not Windows NT 4.0
					{
						std::stringstream strm;
						strm << (osinfo.dwBuildNumber & 0xFFFF);
						osversion += std::string( osinfo.szCSDVersion ) + std::string( " (Build " ) + strm.str() + std::string( ")"  );
					}

					break;

				case VER_PLATFORM_WIN32_WINDOWS :
					if( 4 == osinfo.dwMajorVersion )
					{
						if( 0 == osinfo.dwMinorVersion )
						{
							osversion = "Microsoft Windows 95 ";
							if( ( 'B' == osinfo.szCSDVersion[ 1 ] )
								||
								( 'C' == osinfo.szCSDVersion[ 1 ] ) )
							{
								osversion += std::string( "OSR2 " );
							}
						}
						else if( 10 == osinfo.dwMinorVersion )
						{
							osversion = "Microsoft Windows 98 ";
							if( ( 'A' == osinfo.szCSDVersion[ 1 ] )
								||
								( 'B' == osinfo.szCSDVersion[ 1 ] ) )
							{
								osversion += std::string( "SE " );
							}
						}
						else if( 90 == osinfo.dwMinorVersion )
						{
							osversion = "Microsoft Windows Millenium Edition ";
						}
					}
					break;

				case VER_PLATFORM_WIN32s :
					osversion = "Microsoft Win32s";
					break;
				}
			}
		#else // linux
			std::string osversion = std::string( "unknown Linux" );

			const std::string lsbReleaseFilename { "/etc/lsb-release" };
			
			std::unique_ptr<FILE, decltype(&fclose)> file_lsb_release_exists( fopen( lsbReleaseFilename.c_str(), "r" ), fclose );
			
			if( file_lsb_release_exists )
			{
				const std::string lsbReleaseCommand { "lsb_release -drs" };
				
				std::unique_ptr<FILE, decltype(&pclose)> file_release( popen( lsbReleaseCommand.c_str(), "r" ), pclose );
				
				if( file_release )
				{
					std::array<char, 128>buffer_release;

					fgets( buffer_release.data(), buffer_release.size(), file_release.get() );

					buffer_release[ strlen( buffer_release.data() ) - 1 ] = 0;

					osversion = std::string( buffer_release.data() );

					const std::string kernelVersionCommand { "uname -rmo" };

					std::unique_ptr<FILE, decltype(&pclose)> file_kernel( popen( kernelVersionCommand.c_str(), "r" ), pclose );

					if( file_kernel )
					{
						std::array<char, 128>buffer_kernel;
						fgets( buffer_kernel.data(), buffer_kernel.size(), file_kernel.get() );

						buffer_kernel[ strlen( buffer_kernel.data() ) - 1 ] = 0;

						osversion += std::string( " Kernel " ) + std::string( buffer_kernel.data() );
					}
					else
					{
						logWARNING( "'{0}' couldn't be run", kernelVersionCommand );
					}
				}
				else
				{
					logWARNING( "'{0}' couldn't be run", lsbReleaseCommand );
				}
			}
			else
			{
				logWARNING( "'{0}' doesn't seem to exist", lsbReleaseFilename );
			}
		#endif

		return( osversion );
	}

	int SystemMemoryMiB()
	{
		return( SDL_GetSystemRAM() );
	}

	int ProcessorCount()
	{
		return( SDL_GetCPUCount() );
	}

	std::string ProcessorInfo()
	{
		// TODO with SDL 2.x we can use the following functions to get CPU-Information
		// SDL_GetCPUType();
		// SDL_GetCPUName();

		#ifdef WIN32
			char	value[ 1024 ];
			DWORD	valueSize;
			HKEY	hKey;

			std::string processor{ "unknown" };

			if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey ) )
			{
				// get processor name
				valueSize = 1024;
				if( ERROR_SUCCESS == RegQueryValueEx( hKey, TEXT( "ProcessorNameString" ), nullptr, nullptr, reinterpret_cast< LPBYTE >( &value ), &valueSize ) )
				{
					processor = value;
				}
				else
				{
					processor = "UnknownProc";
				}

				processor += " / ";

				// get processor identifier
				valueSize = 1024;
				if( ERROR_SUCCESS == RegQueryValueEx( hKey, TEXT( "Identifier" ), nullptr, nullptr, reinterpret_cast< LPBYTE >( &value ), &valueSize ) )
				{
					processor += value;
				}
				else
				{
					processor += "UnknownIdentifier";
				}

				processor += " / ";

				// get processor vendor
				valueSize = 1024;
				if( ERROR_SUCCESS == RegQueryValueEx( hKey, TEXT( "VendorIdentifier" ), nullptr, nullptr, reinterpret_cast< LPBYTE >( &value ), &valueSize ) )
				{
					processor += value;
				}
				else
				{
					processor += "UnknownVendor";
				}

				processor += " / ";

				// get processor frequence
				valueSize = 1024;
				if( ERROR_SUCCESS == RegQueryValueEx( hKey, TEXT( "~MHz" ), nullptr, nullptr, reinterpret_cast< LPBYTE >( &value ), &valueSize ) )
				{
					processor += std::to_string( reinterpret_cast< int >( value ) );
					processor += "MHz";

				}
				else
				{
					processor += "UnknownFreq";
				}
			}

			return( String::trim( processor ) );

			RegCloseKey( hKey );
		#else // linux
			std::string model_name;
			std::string cpufamily;
			std::string model;
			std::string stepping;
			std::string vendor_id;
			std::string mhz;

			std::unique_ptr<FILE, decltype(&pclose)> lscpu( popen( "LC_ALL=en_US.utf8 lscpu", "r" ), pclose );
			
			if( lscpu )
			{
				std::array<char, 128> buffer;

				while( fgets( buffer.data(), buffer.size(), lscpu.get() ) )
				{
					std::string std_buffer( buffer.data() );

					if( std_buffer.substr( 0, 11 ) == std::string( "Model name:" ) )
					{
						model_name = String::trim( std_buffer.substr( std_buffer.find( ":" ) + 1 ) );
					}
					else if( std_buffer.substr( 0, 10 ) == std::string( "Vendor ID:" ) )
					{
						vendor_id = String::trim( std_buffer.substr( std_buffer.find( ":" ) + 1 ) );
					}
					else if( std_buffer.substr( 0, 11 ) == std::string( "CPU family:" ) )
					{
						cpufamily = String::trim( std_buffer.substr( std_buffer.find( ":" ) + 1 ) );
					}
					else if( std_buffer.substr( 0, 6 ) == std::string( "Model:" ) )
					{
						model = String::trim( std_buffer.substr( std_buffer.find( ":" ) + 1 ) );
					}
					else if( std_buffer.substr( 0, 9 ) == std::string( "Stepping:" ) )
					{
						stepping = String::trim( std_buffer.substr( std_buffer.find( ":" ) + 1 ) );
					}
					else if( std_buffer.substr( 0, 12 ) == std::string( "CPU max MHz:" ) )
					{
						std::string mhz2 = std_buffer.substr( std_buffer.find( ":" ) + 1 );
						mhz = String::trim( mhz2.substr( 0, mhz2.find( "." ) ) );
					}
				}

				return( fmt::format( "{0} / Family {1} Model {2} Stepping {3} / {4} / {5} MHz", model_name, cpufamily, model, stepping, vendor_id, mhz ) );
			}

			return( "unknown" );
		#endif
	}

	std::string CPUFeatures()
	{
		std::string cpuFeatures;

		if( SDL_HasRDTSC() )
		{
			cpuFeatures += "RDTSC ";
		}

		if( SDL_HasAltiVec() )
		{
			cpuFeatures += "AltiVec ";
		}

		if( SDL_HasMMX() )
		{
			cpuFeatures += "MMX ";
		}

		if( SDL_Has3DNow() )
		{
			cpuFeatures += "3DNow ";
		}

		if( SDL_HasSSE() )
		{
			cpuFeatures += "SSE ";
		}

		if( SDL_HasSSE2() )
		{
			cpuFeatures += "SSE2 ";
		}

		if( SDL_HasSSE3() )
		{
			cpuFeatures += "SSE3 ";
		}

		if( SDL_HasSSE41() )
		{
			cpuFeatures += "SSE41 ";
		}

		if( SDL_HasSSE42() )
		{
			cpuFeatures += "SSE42 ";
		}

		if( SDL_HasAVX() )
		{
			cpuFeatures += "AVX";
		}

		return( cpuFeatures );
	}
}
