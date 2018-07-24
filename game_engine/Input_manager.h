#pragma once

#ifndef _INPUT_MANAGER
#define _INPUT_MANAGER


#include <stdio.h>
#include <utility> 
#include <list>
#include <map>
#include <string>
#include <vector>
#include "boost/container/flat_map.hpp"

#include <windows.h>

// hid devices helper
#include <hidsdi.h>
//#include <hidpddi.h>
//#include <hidpi.h>
#pragma comment (lib, "hid.lib")


#include "Base_Exception.h"

//required for registering for raw input
#include "Window Procedure.h"


#include "Input_Device.h"
#include "Tokenizer.h"



//	#define    DEVICE_MOUSE               0x01
//	#define    DEVICE_KEYBOARD            0x02
//	#define    DEVICE_GAMEPAD             0x04


namespace GameEngine
{

	class InputManager;

	class InputDeviceInfo
	{
	protected:

		typedef		std::map< std::string, std::pair< std::string, unsigned int > >					DeviceIdsMap;
		typedef		std::map< std::string, std::pair< std::string, unsigned int > >::value_type		DeviceIdsMapValue;

		std::string str_deviceName;
		HANDLE h_handle;

		DWORD dw_type;

		USHORT us_usagePage;
		USHORT us_usage;


		std::string str_readableName;


		std::string str_interfaceType;

		std::string str_definition;
		DeviceIdsMap m_definitionIds;

		std::string str_localSystemId;

		std::string str_UID;


		InputDeviceInfo* idi_nextDeviceSegment;

//		std::string str_readableName
//		std::string str_systemName
//		RID_DEVICE_INFO devInfo

	public:

		InputDeviceInfo( std::string _str_deviceName, HANDLE _h_handle, RID_DEVICE_INFO* _rdi_deviceInfo ) : str_deviceName( _str_deviceName ), h_handle( _h_handle ), dw_type( _rdi_deviceInfo->dwType ), idi_nextDeviceSegment( NULL )
		{
			if ( _rdi_deviceInfo->dwType == RIM_TYPEMOUSE )
			{
				us_usagePage = HID_MOUSE_USAGE_PAGE;
				us_usage = HID_MOUSE_USAGE;
			}
			else if ( _rdi_deviceInfo->dwType == RIM_TYPEKEYBOARD )
			{
				us_usagePage = HID_KEYBOARD_USAGE_PAGE;
				us_usage = HID_KEYBOARD_USAGE;
			}
			else
			{
				us_usagePage = _rdi_deviceInfo->hid.usUsagePage;
				us_usage = _rdi_deviceInfo->hid.usUsage;
			}


			{
				std::string str_deviceStringTmp = str_deviceName.substr( 0, 4 ).compare( "\\\\?\\" ) ? str_deviceName : str_deviceName.substr( 4 );
				std::vector< std::string > v_deviceTokens;

				if ( tokenize( str_deviceStringTmp, '#', v_deviceTokens ) == 4 )
				{
					str_interfaceType = v_deviceTokens[ 0 ];
					str_definition = v_deviceTokens[ 1 ];
					str_localSystemId = v_deviceTokens[ 2 ];
					str_UID = v_deviceTokens[ 3 ];

					std::vector< std::string > v_definitionTokens;

					if ( tokenize( str_definition, '&', v_definitionTokens ) )
					{
						std::vector< std::string > v_tokenizedIds;
						for ( unsigned int i = 0; i < v_definitionTokens.size(); ++i )
						{
							v_tokenizedIds.clear();
							if ( tokenize( v_definitionTokens[ i ], '_', v_tokenizedIds ) == 2 )
							{
								DeviceIdsMapValue p_tmp;
								p_tmp.first = v_tokenizedIds[ 0 ];
								p_tmp.second.first = v_tokenizedIds[ 1 ];
								p_tmp.second.second = std::strtoul( v_tokenizedIds[ 1 ].c_str(), v_tokenizedIds[ 1 ].c_str() + v_tokenizedIds[ 1 ].size(), 10 );

								m_definitionIds.insert( p_tmp );
							}
						}
					}


					char c_lszValue[1000] = {0};
					DWORD dw_length=1000;
					HKEY hk_hKey;
					std::string str_registryKey( "System\\CurrentControlSet\\Enum\\");
					str_registryKey.append( str_interfaceType ).append( "\\" ).append( str_definition ).append( "\\" ).append( str_localSystemId );

					if ( RegOpenKeyExA( HKEY_LOCAL_MACHINE, str_registryKey.c_str(), NULL, KEY_READ , &hKey) == ERROR_SUCCESS && RegQueryValueExA( hKey, "DeviceDesc", NULL, NULL, (unsigned char*)c_lszValue, &dw_length ) == ERROR_SUCCESS )
					{
						str_readableName = str_readableName.substr( str_readableName.find_last_of( ";" ) + 1 );
					}

					RegCloseKey( hk_hKey );
				}
			}
		};

		HANDLE getHandle()				{ return h_handle; };
		DWORD getType()					{ return dw_type; };
		USHORT getUsagePage()			{ return us_usagePage; };
		USHORT getUsage()				{ return us_usage; };
		std::string getReadableName()	{ return str_readableName; };

		void setDeviceSegment( InputDeviceInfo* _idi_nextDeviceSegment )
		{
			idi_nextDeviceSegment = _idi_nextDeviceSegment;
		};

		InputDeviceInfo* getDeviceSegment()
		{
			return idi_nextDeviceSegment;
		};

		void addDeviceSegment( InputDeviceInfo* _idi_nextDeviceSegment )
		{
			InputDeviceInfo* idi_tmpDev = this;

			while ( idi_tmpDev->getDeviceSegment() )
			{
				idi_tmpDev = idi_tmpDev->getDeviceSegment();
			}

			idi_tmpDev->setDeviceSegment( _idi_nextDeviceSegment );
		};

		int getDeviceSegmentsCount()
		{
			InputDeviceInfo* idi_tmpDev = this;
			int i_count( 1 );
			while ( idi_tmpDev->getDeviceSegment() )
			{
				idi_tmpDev = idi_tmpDev->getDeviceSegment();
				++i_count;
			}
			return i_count;
		};

	};


	class InputDevGamepadInfo : public InputDeviceInfo
	{
	protected:
		USHORT us_vid;
		USHORT us_pid;
		std::string str_localId;

		// this is for devices that signs as more than one device
		InputDeviceHIDInfo* idi_NextDeviceSegment;



		InputDeviceInfo( std::string _str_deviceName, HANDLE _h_handle, RID_DEVICE_INFO* _rdi_deviceInfo )

	};



	class InputManager
	{
	private:

		std::map< std::string, InputDevice > m_deviceMap;


	public:



//GetRawInputDeviceInfoA( ri_list[ i ].hDevice, RIDI_DEVICENAME, name, &size );

{
	char lszValue[1000] = {0};
	DWORD dwLength=1000;
	HKEY hKey;

	std::string str( "System\\CurrentControlSet\\Enum\\");
	str.append( str_type ).append( "\\" ).append( str_definition ).append( "\\" ).append( str_localId );

	if ( RegOpenKeyExA( HKEY_LOCAL_MACHINE, str.c_str(), NULL, KEY_READ , &hKey) == ERROR_SUCCESS && RegQueryValueExA( hKey, "DeviceDesc", NULL, NULL, (unsigned char*)lszValue, &dwLength ) == ERROR_SUCCESS )
	{
		systemName = lszValue;
	}

	RegCloseKey(hKey);

//lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, \"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\", 0L, KEY_READ , &hKey);
}
	};

}; //namespace GameEngine

#endif //_INPUT_MANAGER
