//#include "utility.h"

#include <iostream>


#include "Unicode.h"
#include "Base_Exception.h"
#include "Basic_Logger.h"
#include "Variable_Library.h"

#include "Window Procedure.h"
#include "Input_manager.h"

#include <vector>
//#include "boost/any.hpp"




#include "Rpc.h"
//#pragma comment (lib, "Rpc.lib")
#pragma comment (lib, "Rpcrt4.lib")


using namespace GameEngine;




bool padMsgProcFunc( HWND _hwnd, WPARAM _wparam, LPARAM _lParam )
{
	RAWINPUTHEADER rih_header;
	UINT ui_size = sizeof( RAWINPUTHEADER );
	GetRawInputData( ( HRAWINPUT ) _lParam, RID_HEADER, &rih_header, &ui_size, sizeof( RAWINPUTHEADER ) );
/*
	if ( rih_header.dwType == RIM_TYPEMOUSE )
	{
		RAWINPUT ri_input;
		ui_size = sizeof( RAWINPUT );
		GetRawInputData( ( HRAWINPUT ) _lParam, RID_INPUT, &ri_input, &ui_size, sizeof( RAWINPUTHEADER ) );
		DWORD type = ri_input.header.dwType;
	}
*/
	if ( rih_header.dwType == RIM_TYPEKEYBOARD )
	{
		exit(0);
		RAWINPUT ri_input;
		ui_size = sizeof( RAWINPUT );
		GetRawInputData( ( HRAWINPUT ) _lParam, RID_INPUT, &ri_input, &ui_size, sizeof( RAWINPUTHEADER ) );
		DWORD type = ri_input.header.dwType;
	}

	if ( rih_header.dwType == RIM_TYPEHID )
	{
		std::vector<BOOL> v_mem;
		v_mem.resize( rih_header.dwSize );
//		rih_header.dwSize;
//		RAWINPUT ri_input;
		ui_size = rih_header.dwSize;
		GetRawInputData( ( HRAWINPUT ) _lParam, RID_INPUT, &( v_mem[ 0 ] ), &ui_size, sizeof( RAWINPUTHEADER ) );
		RAWINPUT* ri_input = reinterpret_cast <RAWINPUT*> ( &( v_mem[ 0 ] ) );

		DWORD dw_inputsCount = ri_input->data.hid.dwCount;
		DWORD dw_sizeOfInput = ri_input->data.hid.dwSizeHid;
		char* b_deviceReportData = reinterpret_cast <char*> ( ri_input->data.hid.bRawData );


HANDLE h_deviceHandle = rih_header.hDevice;

UINT ui_bufferSize = 0;
GetRawInputDeviceInfoA( h_deviceHandle, RIDI_PREPARSEDDATA, NULL, &ui_bufferSize );
BYTE* bt_preparsedData = new BYTE [ ui_bufferSize ];
GetRawInputDeviceInfoA( h_deviceHandle, RIDI_PREPARSEDDATA, bt_preparsedData, &ui_bufferSize );



ULONG ul_maxDataListLength = HidP_MaxDataListLength( HidP_Input, reinterpret_cast<PHIDP_PREPARSED_DATA>( bt_preparsedData ) );
HIDP_DATA* hd_dataList = new HIDP_DATA [ ul_maxDataListLength ];
ULONG ul_dataListLength = ul_maxDataListLength;

/*NTSTATUS __stdcall */

HidP_GetData( HidP_Input, hd_dataList, &ul_dataListLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( bt_preparsedData ), b_deviceReportData, dw_inputsCount * dw_sizeOfInput );


Logger& syslog = Logger::getLogger( "gamepad_test" );

char tmp[32];

for ( unsigned int i = 0; i < ul_dataListLength; ++i )
{
/*
	USHORT hd_dataList[i].DataIndex
	USHORT hd_dataList[i].Reserved
	USHORT hd_dataList[i].RawValue
	BOOLEAN hd_dataList[i].On
*/
	//if ( hd_dataList[i].RawValue != 128  && hd_dataList[i].DataIndex != 5 )
	{
		syslog << LogPriority::INFO << "Index: " << std::string( itoa( hd_dataList[i].DataIndex, tmp, 10 ) ) << "\t\traw value: " << std::string( itoa( hd_dataList[i].RawValue, tmp, 10 ) ) << "\t\tbool: " << std::string( itoa( hd_dataList[i].On, tmp, 10 ) );
	}
}






/*
DWORD dw_error = GetLastError();
char c_errMsg[ 2048 ];
FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 2048, NULL );
THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
*/


/*
RID_DEVICE_INFO rdi_deviceInfo;
ui_size = sizeof( RID_DEVICE_INFO );
out = GetRawInputDeviceInfoA( h_deviceHandle, RIDI_DEVICEINFO, &rdi_deviceInfo, &ui_size );
*/

/*
rdi_deviceInfo.hid.dwVendorId
rdi_deviceInfo.hid.dwProductId
rdi_deviceInfo.hid.dwVersionNumber
rdi_deviceInfo.hid.usUsagePage
rdi_deviceInfo.hid.usUsage
*/


/*
// get caps
HIDP_CAPS hc_caps;
HidP_GetCaps( reinterpret_cast<PHIDP_PREPARSED_DATA>( bt_preparsedData ), &hc_caps );
/*
hc_caps.InputReportByteLength
hc_caps.OutputReportByteLength
hc_caps.FeatureReportByteLength
hc_caps.NumberLinkCollectionNodes
hc_caps.NumberInputButtonCaps
hc_caps.NumberInputValueCaps
hc_caps.NumberInputDataIndices
hc_caps.NumberOutputButtonCaps
hc_caps.NumberOutputValueCaps
hc_caps.NumberOutputDataIndices
hc_caps.NumberFeatureButtonCaps
hc_caps.NumberFeatureValueCaps
hc_caps.NumberFeatureDataIndices

*/

/*
	syslog << LogPriority::INFO << " ";
	syslog << LogPriority::INFO << " ";
	syslog << LogPriority::INFO << "Buttons";
	syslog << LogPriority::INFO << " ";

		HIDP_BUTTON_CAPS* hbc_buttonCaps = new HIDP_BUTTON_CAPS [ hc_caps.NumberInputButtonCaps ];
		USHORT us_buttonCapsLength = hc_caps.NumberInputButtonCaps;
		HidP_GetButtonCaps( HidP_Input, hbc_buttonCaps, &us_buttonCapsLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( bt_preparsedData ) );

		HIDP_BUTTON_CAPS* test = &( hbc_buttonCaps[1] );

/****************
		capsLength = Caps.NumberInputButtonCaps;
CHECK( HidP_GetButtonCaps(HidP_Input, pButtonCaps, 
	&capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS )
g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;
****************/
/*
		for ( int i = 0; i < hc_caps.NumberInputButtonCaps; ++i )
		{
	syslog << LogPriority::INFO << " ";
	syslog << LogPriority::INFO << " ";
			syslog << LogPriority::INFO << "Index: " << std::string( itoa( i, tmp, 10 ) );

			syslog << LogPriority::INFO << "UsagePage: " << std::string( itoa( hbc_buttonCaps[i].UsagePage, tmp, 10 ) );
			syslog << LogPriority::INFO << "ReportID: " << std::string( itoa( hbc_buttonCaps[i].ReportID, tmp, 10 ) );
			syslog << LogPriority::INFO << "IsAlias: " << std::string( hbc_buttonCaps[i].IsAlias == TRUE ? "TRUE" : "FALSE" );
			syslog << LogPriority::INFO << "BitField: " << std::string( itoa( hbc_buttonCaps[i].BitField, tmp, 10 ) );
			syslog << LogPriority::INFO << "LinkCollection: " << std::string( itoa( hbc_buttonCaps[i].LinkCollection, tmp, 10 ) );
			syslog << LogPriority::INFO << "LinkUsage: " << std::string( itoa( hbc_buttonCaps[i].LinkUsage, tmp, 10 ) );
			syslog << LogPriority::INFO << "LinkUsagePage: " << std::string( itoa( hbc_buttonCaps[i].LinkUsagePage, tmp, 10 ) );
			syslog << LogPriority::INFO << "IsAbsolute: " << std::string( hbc_buttonCaps[i].IsAbsolute == TRUE ? "TRUE" : "FALSE" );

	syslog << LogPriority::INFO << " ";
			if ( hbc_buttonCaps[i].IsRange == TRUE )
			{
				syslog << LogPriority::INFO << "Range: ";

				if ( hbc_buttonCaps[i].IsStringRange == TRUE )
				{
					syslog << LogPriority::INFO << "StringMin: " << std::string( itoa( hbc_buttonCaps[i].Range.StringMin, tmp, 10 ) );
					syslog << LogPriority::INFO << "StringMax: " << std::string( itoa( hbc_buttonCaps[i].Range.StringMax, tmp, 10 ) );
				}
				else if ( hbc_buttonCaps[i].IsDesignatorRange == TRUE )
				{
					syslog << LogPriority::INFO << "DesignatorMin: " << std::string( itoa( hbc_buttonCaps[i].Range.DesignatorMin, tmp, 10 ) );
					syslog << LogPriority::INFO << "DesignatorMax: " << std::string( itoa( hbc_buttonCaps[i].Range.DesignatorMax, tmp, 10 ) );
				}
				else
				{
					syslog << LogPriority::INFO << "UsageMin: " << std::string( itoa( hbc_buttonCaps[i].Range.UsageMin, tmp, 10 ) );
					syslog << LogPriority::INFO << "UsageMax: " << std::string( itoa( hbc_buttonCaps[i].Range.UsageMax, tmp, 10 ) );
				}

				syslog << LogPriority::INFO << "DataIndexMin: " << std::string( itoa( hbc_buttonCaps[i].Range.DataIndexMin, tmp, 10 ) );
				syslog << LogPriority::INFO << "DataIndexMax: " << std::string( itoa( hbc_buttonCaps[i].Range.DataIndexMax, tmp, 10 ) );
			}
			else
			{
				syslog << LogPriority::INFO << "Not range: ";

				if ( hbc_buttonCaps[i].IsStringRange == TRUE )
				{
					syslog << LogPriority::INFO << "StringIndex: " << std::string( itoa( hbc_buttonCaps[i].NotRange.StringIndex, tmp, 10 ) );
				}
				else if ( hbc_buttonCaps[i].IsDesignatorRange == TRUE )
				{
					syslog << LogPriority::INFO << "DesignatorIndex: " << std::string( itoa( hbc_buttonCaps[i].NotRange.DesignatorIndex, tmp, 10 ) );
				}
				else
				{
					syslog << LogPriority::INFO << "Usage: " << std::string( itoa( hbc_buttonCaps[i].NotRange.Usage, tmp, 10 ) );
				}

				syslog << LogPriority::INFO << "DataIndex: " << std::string( itoa( hbc_buttonCaps[i].NotRange.DataIndex, tmp, 10 ) );
			}
		}


	syslog << LogPriority::INFO << " ";
	syslog << LogPriority::INFO << " ";
	syslog << LogPriority::INFO << " ";
	syslog << LogPriority::INFO << " ";

	syslog << LogPriority::INFO << "Values";
	syslog << LogPriority::INFO << " ";

//		exit(0);

//	int i_buttonsNumber = hbc_buttonCaps.Range.UsageMax - hbc_buttonCaps.Range.UsageMin + 1;


//	HIDP_VALUE_CAPS* hvc_valueCaps = new HIDP_VALUE_CAPS [ hc_caps.NumberInputValueCaps ];
//	USHORT us_valueCapsLength = hc_caps.NumberInputValueCaps;
//	HidP_GetValueCaps( HidP_Input, hvc_valueCaps, &us_valueCapsLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( bt_preparsedData ) );

	HIDP_VALUE_CAPS* hvc_valueCaps = new HIDP_VALUE_CAPS [ hc_caps.NumberInputValueCaps ];
	USHORT us_valueCapsLength = hc_caps.NumberInputValueCaps;
	HidP_GetValueCaps( HidP_Input, hvc_valueCaps, &us_valueCapsLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( bt_preparsedData ) );

for ( int i = 0; i < us_valueCapsLength; ++i )
{
	syslog << LogPriority::INFO << " ";
	syslog << LogPriority::INFO << " ";
	syslog << LogPriority::INFO << "Index: " << std::string( itoa( i, tmp, 10 ) );
	syslog << LogPriority::INFO << "UsagePage: " << std::string( itoa( hvc_valueCaps[i].UsagePage, tmp, 10 ) );
	syslog << LogPriority::INFO << "ReportID: " << std::string( itoa( hvc_valueCaps[i].ReportID, tmp, 10 ) );
	syslog << LogPriority::INFO << "IsAlias: " << std::string( hvc_valueCaps[i].IsAlias == TRUE ? "TRUE" : "FALSE" );
	syslog << LogPriority::INFO << "BitField: " << std::string( itoa( hvc_valueCaps[i].BitField, tmp, 10 ) );
	syslog << LogPriority::INFO << "LinkCollection: " << std::string( itoa( hvc_valueCaps[i].LinkCollection, tmp, 10 ) );
	syslog << LogPriority::INFO << "LinkUsage: " << std::string( itoa( hvc_valueCaps[i].LinkUsage, tmp, 10 ) );
	syslog << LogPriority::INFO << "LinkUsagePage: " << std::string( itoa( hvc_valueCaps[i].LinkUsagePage, tmp, 10 ) );



syslog << LogPriority::INFO << "BitSize: " << std::string( itoa( hvc_valueCaps[i].BitSize, tmp, 10 ) );
syslog << LogPriority::INFO << "ReportCount: " << std::string( itoa( hvc_valueCaps[i].ReportCount, tmp, 10 ) );
syslog << LogPriority::INFO << "UnitsExp: " << std::string( itoa( hvc_valueCaps[i].UnitsExp, tmp, 10 ) );
syslog << LogPriority::INFO << "Units: " << std::string( itoa( hvc_valueCaps[i].Units, tmp, 10 ) );
syslog << LogPriority::INFO << "LogicalMin: " << std::string( itoa( hvc_valueCaps[i].LogicalMin, tmp, 10 ) );
syslog << LogPriority::INFO << "LogicalMax: " << std::string( itoa( hvc_valueCaps[i].LogicalMax, tmp, 10 ) );
syslog << LogPriority::INFO << "PhysicalMin: " << std::string( itoa( hvc_valueCaps[i].PhysicalMin, tmp, 10 ) );
syslog << LogPriority::INFO << "PhysicalMax: " << std::string( itoa( hvc_valueCaps[i].PhysicalMax, tmp, 10 ) );




syslog << LogPriority::INFO << "IsAbsolute: " << std::string( hvc_valueCaps[i].IsAbsolute == TRUE ? "TRUE" : "FALSE" );
syslog << LogPriority::INFO << "HasNull: " << std::string( hvc_valueCaps[i].HasNull == TRUE ? "TRUE" : "FALSE" );



	syslog << LogPriority::INFO << " ";
	if ( hbc_buttonCaps[i].IsRange == TRUE )
	{
		syslog << LogPriority::INFO << "Range: ";

		if ( hbc_buttonCaps[i].IsStringRange == TRUE )
		{
			syslog << LogPriority::INFO << "StringMin: " << std::string( itoa( hvc_valueCaps[i].Range.StringMin, tmp, 10 ) );
			syslog << LogPriority::INFO << "StringMax: " << std::string( itoa( hvc_valueCaps[i].Range.StringMax, tmp, 10 ) );
		}
		else if ( hbc_buttonCaps[i].IsDesignatorRange == TRUE )
		{
			syslog << LogPriority::INFO << "DesignatorMin: " << std::string( itoa( hvc_valueCaps[i].Range.DesignatorMin, tmp, 10 ) );
			syslog << LogPriority::INFO << "DesignatorMax: " << std::string( itoa( hvc_valueCaps[i].Range.DesignatorMax, tmp, 10 ) );
		}
		else
		{
			syslog << LogPriority::INFO << "UsageMin: " << std::string( itoa( hvc_valueCaps[i].Range.UsageMin, tmp, 10 ) );
			syslog << LogPriority::INFO << "UsageMax: " << std::string( itoa( hvc_valueCaps[i].Range.UsageMax, tmp, 10 ) );
		}

		syslog << LogPriority::INFO << "DataIndexMin: " << std::string( itoa( hvc_valueCaps[i].Range.DataIndexMin, tmp, 10 ) );
		syslog << LogPriority::INFO << "DataIndexMax: " << std::string( itoa( hvc_valueCaps[i].Range.DataIndexMax, tmp, 10 ) );
	}
	else
	{
		syslog << LogPriority::INFO << "Not range: ";

		if ( hbc_buttonCaps[i].IsStringRange == TRUE )
		{
			syslog << LogPriority::INFO << "StringIndex: " << std::string( itoa( hvc_valueCaps[i].NotRange.StringIndex, tmp, 10 ) );
		}
		else if ( hbc_buttonCaps[i].IsDesignatorRange == TRUE )
		{
			syslog << LogPriority::INFO << "DesignatorIndex: " << std::string( itoa( hvc_valueCaps[i].NotRange.DesignatorIndex, tmp, 10 ) );
		}
		else
		{
			syslog << LogPriority::INFO << "Usage: " << std::string( itoa( hvc_valueCaps[i].NotRange.Usage, tmp, 10 ) );
		}

		syslog << LogPriority::INFO << "DataIndex: " << std::string( itoa( hvc_valueCaps[i].NotRange.DataIndex, tmp, 10 ) );
	}
}

*/

//		exit(0);

		return true;
	}

	return false;
};


class DeviceInfo
{
private:

public:

	HANDLE h_handle;
	DWORD dw_inputType;

	std::string str_productString;
	std::string str_fullname;

	// root is unused, accept only HID devices
	std::string str_type;
	std::string str_definition;
	USHORT us_vid;
	USHORT us_pid;
	std::string str_localId;
	std::string str_uuid;

	std::string systemName;


	DeviceInfo( HANDLE _h_handle, DWORD _dw_inputType, std::string _str_fullname ) : h_handle( _h_handle ), dw_inputType( _dw_inputType ), us_vid( 0 ), us_pid( 0 )
	{
		str_fullname = _str_fullname;
		UINT tmp1 = 0;
		UINT tmp2 = _str_fullname.find_first_of( '#', tmp1 );
		str_type = std::string( _str_fullname, tmp1, tmp2 );
		str_type = str_type.substr( str_type.find( "\\\\?\\" ) + 4 );

		tmp1 = tmp2 + 1;
		tmp2 = _str_fullname.find_first_of( '#', tmp1 );
		str_definition = std::string( _str_fullname, tmp1, tmp2 - tmp1 );

		std::list<std::pair<std::string, ULONG>> ls_tmpIds;

		std::string str_tmpDef;
		UINT ui_begin = 0, ui_end = 0;

		while ( ( ui_end = str_definition.find_first_of( '&', ui_begin ) ) != std::string::npos )
		{
			str_tmpDef = str_definition.substr( ui_begin, ui_end - ui_begin );
			++ui_begin;

			ui_end = str_tmpDef.find( '_', 0 );

			const char* c_idPtr = str_tmpDef.c_str() + ui_end + 1;
			const char* c_idPtrEnd = c_idPtr;

			while ( ( *c_idPtrEnd >= '0' && *c_idPtrEnd <= '9' ) || ( *c_idPtrEnd >= 'a' && *c_idPtrEnd <= 'f' ) || ( *c_idPtrEnd >= 'A' && *c_idPtrEnd <= 'F' ) ) ++c_idPtrEnd;
			std::string tmp = str_tmpDef.substr( 0, ui_end );

			if ( tmp.compare( "VID" ) == 0 )
				us_vid = strtoul( c_idPtr, NULL, 16 );
			else if ( tmp.compare( "PID" ) == 0 )
				us_pid = strtoul( c_idPtr, NULL, 16 );
		}
		
		tmp1 = tmp2 + 1;
		tmp2 = _str_fullname.find_first_of( '#', tmp1 );
		str_localId = std::string( _str_fullname, tmp1, tmp2 - tmp1 );

		tmp1 = tmp2 + 1;
		tmp2 = _str_fullname.find_first_of( '#', tmp1 );
		str_uuid = std::string( _str_fullname, tmp1 );


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




	};

	virtual ~DeviceInfo()
	{};

	virtual std::string getInfoString()
	{
		std::stringstream sstr_outStream;
		sstr_outStream << "Full name:  " << str_fullname << "\n";
		sstr_outStream << "Product:    " << str_productString << "\n";

		sstr_outStream << "RIM Type:   ";
		if ( dw_inputType == RIM_TYPEMOUSE )
			sstr_outStream << "RIM_TYPEMOUSE";
		else if ( dw_inputType == RIM_TYPEKEYBOARD )
			sstr_outStream << "RIM_TYPEKEYBOARD";
		else
			sstr_outStream << "RIM_TYPEHID";
		sstr_outStream << "\n";

		char tmp[1000] = {0};
		sstr_outStream << "Handle:     0x" << itoa( reinterpret_cast <int> ( h_handle ), tmp, 16 ) << "\n";

		sstr_outStream << "GUID:       " << str_uuid << "\n";

		sstr_outStream << "Type:       " << str_type << "\n";
		sstr_outStream << "Definition: " << str_definition << "\n";

		if ( us_vid && us_pid )
		{
			sstr_outStream << "    VID:    " << us_vid << "\n";
			sstr_outStream << "    PID:    " << us_pid << "\n";
			sstr_outStream << "    deflen: " << str_definition.size() << "\n";
		}

		sstr_outStream << "Local ID:   " << str_localId << "\n";
		sstr_outStream << "System name:   " << systemName << "\n\n";

		return sstr_outStream.str();
	};

};


class HIDDeviceInfo : public DeviceInfo
{
public:
	USHORT us_usage;
	USHORT us_usagePage;


	HIDDeviceInfo( HANDLE _h_handle, DWORD _dw_inputType, std::string _str_fullname, USHORT _us_usage, USHORT _us_usagePage ) : DeviceInfo( _h_handle, _dw_inputType, _str_fullname ), us_usage( _us_usage ), us_usagePage( _us_usagePage )
	{};

	virtual ~HIDDeviceInfo()
	{};

	virtual std::string getInfoString()
	{
		std::string str_out = DeviceInfo::getInfoString();

		std::stringstream sstr_outStream;
		sstr_outStream << DeviceInfo::getInfoString();
		sstr_outStream << "Usage:      " << us_usage << "\n";
		sstr_outStream << "Usage Page: " << us_usagePage << "\n";

		return sstr_outStream.str();
	};

};






int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WindowProcedure* proc = NULL;
	WindowProcedure::create("test", &proc );
	WindowProcedure::setProcedure("test");



	Logger& syslog = ( Logger::addLogger( "gamepad_test",LogPriority::DEBUG ) ).second;
	{
		FileLoggingInterface* li = new FileLoggingInterface( "gamepad_test", "gamepad_test.log", false, false );
		li->setThreshold( LogPriority::NOTSET );
		syslog.setLoggingInterface( li );
	}

	syslog << LogPriority::INFO << "IT'S ALIVE!!!";



	UINT ui_devices = 0;

	UINT ret = GetRawInputDeviceList( NULL, &ui_devices, sizeof( RAWINPUTDEVICELIST ) );
	RAWINPUTDEVICELIST* ri_list = new RAWINPUTDEVICELIST [ ui_devices ];
	GetRawInputDeviceList( ri_list, &ui_devices, sizeof( RAWINPUTDEVICELIST ) /** ui_devices*/ );



	std::list< DeviceInfo* > l_mouseInfo;
	std::list< DeviceInfo* > l_keyboardInfo;
	std::list< DeviceInfo* > l_hidInfo;


	for ( UINT i = 0; i < ui_devices; ++i )
	{
		char name[2014] = {0};
		UINT size = 2014;
		std::string nfo = "\n";

		GetRawInputDeviceInfoA( ri_list[ i ].hDevice, RIDI_DEVICENAME, name, &size );

		if ( ri_list[ i ].dwType == RIM_TYPEMOUSE )
		{
			l_mouseInfo.push_back( new DeviceInfo( ri_list[ i ].hDevice, ri_list[ i ].dwType, name ) );
		}
		else if ( ri_list[ i ].dwType == RIM_TYPEKEYBOARD )
		{
			l_keyboardInfo.push_back( new DeviceInfo( ri_list[ i ].hDevice, ri_list[ i ].dwType, name ) );
		}
		else
		{
			RID_DEVICE_INFO devInfo;
			size = sizeof( RID_DEVICE_INFO );
			GetRawInputDeviceInfoA( ri_list[ i ].hDevice, RIDI_DEVICEINFO, &devInfo, &size );
			l_hidInfo.push_back( new HIDDeviceInfo( ri_list[ i ].hDevice, ri_list[ i ].dwType, name, devInfo.hid.usUsage, devInfo.hid.usUsagePage ) );
		}
	}

/*
	{
		std::list< DeviceInfo* >::iterator it = l_mouseInfo.begin();
		while ( it != l_mouseInfo.end() )
		{
			if ( (*it)->us_pid && (*it)->us_vid )
			{
				std::list< DeviceInfo* >::iterator jt = l_hidInfo.begin();
				while ( jt != l_hidInfo.end() )
				{
					if ( (*jt)->us_pid == (*it)->us_pid && (*jt)->us_vid == (*it)->us_vid )
					{
						std::list< DeviceInfo* >::iterator toRemove = jt;
						++jt;
						delete (*toRemove);
						l_hidInfo.erase( toRemove );
					}
					else
						++jt;
				}

				++it;
			}
			else
			{
				std::list< DeviceInfo* >::iterator toRemove = it;
				++it;
				delete (*toRemove);
				l_mouseInfo.erase( toRemove );
			}
		}
	}



	{
		std::list< DeviceInfo* >::iterator it = l_mouseInfo.begin();
		while ( it != l_mouseInfo.end() )
		{
			if ( (*it)->str_type.compare( "HID" ) )
			{
				std::list< DeviceInfo* >::iterator jt = it;
				++it;

				delete (*it);
				l_mouseInfo.erase( it );
			}
			else
			{
				std::list< DeviceInfo* >::iterator jt = it;
				++jt;

				while ( jt != l_hidInfo.end() )
				{
					if ( (*jt)->us_pid == (*it)->us_pid && (*jt)->us_vid == (*it)->us_vid )
					{
						if ( 
					}
					++jt;
				}


				if ( (*it)->us_pid && (*it)->us_vid )
				{
					std::list< DeviceInfo* >::iterator jt = l_hidInfo.begin();
					while ( jt != l_hidInfo.end() )
					{
						if ( (*jt)->us_pid == (*it)->us_pid && (*jt)->us_vid == (*it)->us_vid )
						{
							std::list< DeviceInfo* >::iterator toRemove = jt;
							++jt;
							delete (*toRemove);
							l_hidInfo.erase( toRemove );
						}
						else
							++jt;
					}

					++it;
				}
				else
				{
					std::list< DeviceInfo* >::iterator toRemove = it;
					++it;
					delete (*toRemove);
					l_mouseInfo.erase( toRemove );
				}
			}
		}
	}





	{
		std::list< DeviceInfo* >::iterator it = l_keyboardInfo.begin();
		while ( it != l_keyboardInfo.end() )
		{
			if ( (*it)->str_type.compare( "HID" ) )
			{
				std::list< DeviceInfo* >::iterator jt = it;
				++it;
				delete (*it);
				l_keyboardInfo.erase( it );
			}
			else
			{
				if ( (*it)->us_pid && (*it)->us_vid )
				{
					std::list< DeviceInfo* >::iterator jt = l_hidInfo.begin();
					while ( jt != l_hidInfo.end() )
					{
						if ( (*jt)->us_pid == (*it)->us_pid && (*jt)->us_vid == (*it)->us_vid )
						{
							std::list< DeviceInfo* >::iterator toRemove = jt;
							++jt;
							delete (*toRemove);
							l_hidInfo.erase( toRemove );
						}
						else
							++jt;
					}

					++it;
				}
				else
				{
					std::list< DeviceInfo* >::iterator toRemove = it;
					++it;
					delete (*toRemove);
					l_keyboardInfo.erase( toRemove );
				}
			}
		}
	}


	{
		std::list< std::pair< USHORT, USHORT > > l_usages;
		l_usages.push_back( std::make_pair( 4, 1 ) );
		l_usages.push_back( std::make_pair( 5, 1 ) );

		std::list< DeviceInfo* >::iterator it = l_hidInfo.begin();
		while ( it != l_hidInfo.end() )
		{
			if ( (*it)->str_type.compare( "HID" ) )
			{
				std::list< DeviceInfo* >::iterator jt = it;
				++it;
				delete (*it);
				l_hidInfo.erase( it );
			}
			else
			{
				HIDDeviceInfo* hdi = static_cast <HIDDeviceInfo*> ( *it );
				bool b_flag = true;

				std::list< std::pair< USHORT, USHORT > >::iterator it_testPage = l_usages.begin();
				while ( it_testPage != l_usages.end() )
				{
					if ( hdi->us_usage == it_testPage->first && hdi->us_usagePage == it_testPage->second )
					{
						b_flag = false;
						break;
					}
					++it_testPage;
				}

				std::list< DeviceInfo* >::iterator itToRemove = it;
				++it;
				if ( b_flag )
				{
				//	l_hidInfo.erase( itToRemove );
				}
			}
		}
	}

/*
	std::list< DeviceInfo* >::iterator it = l_hidInfo.begin();
	while ( it != l_hidInfo.end() )
	{
		std::list< DeviceInfo* >::iterator jt = it;
		++jt;
		while ( jt != l_hidInfo.end() )
		{
			if ( (*jt)->us_pid == (*it)->us_pid && (*jt)->us_vid == (*it)->us_vid )
			
		}
	}
*/



	while ( l_mouseInfo.size() )
	{
		syslog << LogPriority::INFO << "\n" << l_mouseInfo.front()->getInfoString();
		delete l_mouseInfo.front();
		l_mouseInfo.pop_front();
	}

	while ( l_keyboardInfo.size() )
	{
		syslog << LogPriority::INFO << "\n" << l_keyboardInfo.front()->getInfoString();
		delete l_keyboardInfo.front();
		l_keyboardInfo.pop_front();
	}

	while ( l_hidInfo.size() )
	{
		syslog << LogPriority::INFO << "\n" << l_hidInfo.front()->getInfoString();
		delete l_hidInfo.front();
		l_hidInfo.pop_front();
	}



/*
	for ( UINT i = 0; i < ui_devices; ++i )
	{
		char name[2014] = {0};
		UINT size = 2014;
		std::string nfo = "\n";

		GetRawInputDeviceInfoA( ri_list[ i ].hDevice, RIDI_DEVICENAME, name, &size );

		if ( ri_list[ i ].dwType != RIM_TYPEHID )
		{
			l_devicesInfo.push_back( new DeviceInfo( ri_list[ i ].hDevice, ri_list[ i ].dwType, name ) );
		}
		else
		{
			RID_DEVICE_INFO devInfo;
			size = sizeof( RID_DEVICE_INFO );
			GetRawInputDeviceInfoA( ri_list[ i ].hDevice, RIDI_DEVICEINFO, &devInfo, &size );
			l_devicesInfo.push_back( new HIDDeviceInfo( ri_list[ i ].hDevice, ri_list[ i ].dwType, name, devInfo.hid.usUsage, devInfo.hid.usUsagePage ) );
		}
	}

	while ( l_devicesInfo.size() )
	{
		syslog << LogPriority::INFO << "\n" << l_devicesInfo.front()->getInfoString();
		delete l_devicesInfo.front();
		l_devicesInfo.pop_front();
	}
*/



	exit(0);






	//HINSTANCE hInstance = GetModuleHandleA(NULL);
	HWND hWnd = 0;
	WNDCLASSEXA wndClassEx;
	UINT wWidth = 600;
	UINT wHeight = 400;


	ZeroMemory(&wndClassEx, sizeof(WNDCLASSEXA));
	wndClassEx.cbSize =			sizeof(WNDCLASSEXA);
	wndClassEx.style =			0;//CS_HREDRAW | CS_VREDRAW;
	wndClassEx.lpfnWndProc =	WindowProcedure::WndProc;
//	wndClassEx.cbClsExtra =		NULL;
//	wndClassEx.cbWndExtra =		NULL;
	wndClassEx.hInstance =		hInstance;
//	wndClassEx.hIcon =			LoadIcon(0, IDI_APPLICATION);
	wndClassEx.hCursor =		LoadCursor(0, IDC_ARROW);
	wndClassEx.hbrBackground =	(HBRUSH)(COLOR_WINDOW+1);//(HBRUSH)GetStockObject(WHITE_BRUSH);
//	wndClassEx.lpszMenuName =	NULL;
	wndClassEx.lpszClassName =	"TEST";
//	wndClassEx.hIconSm =		LoadIcon(0, IDI_APPLICATION);



	RegisterClassExA(&wndClassEx);

	hWnd = CreateWindowExA(	WS_EX_CLIENTEDGE,
							"TEST",
							"TEST",
							WS_OVERLAPPEDWINDOW,//WS_EX_TOPMOST | WS_POPUP,
							CW_USEDEFAULT, CW_USEDEFAULT, //0, 0,
							wWidth, wHeight,// set window to new resolution
							NULL,
							NULL,
							hInstance,
							NULL);
	if (!hWnd)
	{
		DWORD err = GetLastError();
		syslog << LogPriority::INFO << "Error no: " << err;

		return 0;
	}





{
	

	WindowProcedure::get("test")->addMessageProcessor( "gamepad_test", WM_INPUT, padMsgProcFunc );


	RAWINPUTDEVICE rid_deviceInfo;
	rid_deviceInfo.dwFlags = RIDEV_INPUTSINK; //DEFAULT_RAW_INPUT_FLAGS;
	rid_deviceInfo.hwndTarget = hWnd;
	rid_deviceInfo.usUsagePage = HID_GAMEPAD_USAGE_PAGE;
	rid_deviceInfo.usUsage = HID_GAMEPAD_USAGE;
						
	if ( RegisterRawInputDevices( &rid_deviceInfo, 1, sizeof( rid_deviceInfo ) ) == FALSE)
	{
		DWORD dw_error = GetLastError();
		char c_errMsg[ 2048 ];
		FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 2048, NULL );
		std::cerr << c_errMsg;
	}

}













	ShowWindow(hWnd, SW_SHOW);

	UpdateWindow(hWnd);

	//CoInitializeEx(NULL,COINIT_MULTITHREADED);



	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while( GetMessageA( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}



/*

	while( msg.message != WM_QUIT )
	{

		if( PeekMessageA( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
*/
	//CoUninitialize();

//	std::list< RAWMOUSE >::iterator it = mouse->inputTest.begin();
//	std::list< std::vector< std::vector< BYTE > > >::iterator it = gamepad->inputTest.begin();

/*
	for ( ; it != mouse->inputTest.end(); ++it )
	{
		if ( it->usFlags )
			syslog << LogPriority::INFO << "Indicator flags: " << it->usFlags;

//		if ( it->lLastX || it->lLastY )
//			syslog << LogPriority::INFO << "move direction X: "<< it->lLastX << " Y: " << it->lLastY;

		if ( it->usButtonFlags || it->usButtonData )
		{
			if ( it->usButtonFlags == RI_MOUSE_WHEEL )
				syslog << LogPriority::INFO << "mouse wheel: " << (short) ( it->usButtonData );
			else
				syslog << LogPriority::INFO << "mouse buttons state: flags: " << it->usButtonFlags << " data: " << (int) it->usButtonData;
		}

		if ( it->ulRawButtons )
			syslog << LogPriority::INFO << "mouse buttons raw state: " << it->ulRawButtons;

		if ( it->ulExtraInformation )
			syslog << LogPriority::INFO << "Device-specific additional information for the event: " << it->ulExtraInformation;


		if ( it->usFlags || /** it->lLastX || it->lLastY || /** it->usButtonFlags || it->usButtonData || it->ulRawButtons || it->ulExtraInformation )
		{
			syslog << LogPriority::INFO << "--------------------------------------------------------------------------------------------------";
			syslog << LogPriority::INFO << "--------------------------------------------------------------------------------------------------";
		}
	}
*/


	Logger::shutdown();





/*
	VarLibrary::setBool(	"entry bool", true );
	VarLibrary::setInt(		"entry int", -24568 );
	VarLibrary::setFloat(	"entry float", 543.66f );
	VarLibrary::setDouble(	"entry double", 45.96754 );
	VarLibrary::setString(	"entry string", "new string varliable" );

	VarLibrary::setBool(	"test dictionary", "entry bool", true );
	VarLibrary::setInt(		"test dictionary", "entry int", -24568 );
	VarLibrary::setFloat(	"test dictionary", "entry float", 543.66f );
	VarLibrary::setDouble(	"test dictionary", "entry double", 45.96754 );
	VarLibrary::setString(	"test dictionary", "entry string", "new string varliable" );

	VarLibrary::writeLibraryFile( "varlib.txt" );
*/


/*
	try
	{
		VarLibrary::readLibraryFile( "varlib.txt" );

		std::cout << "dictionary: \"\"" << std::endl;

		std::cout << "entry bool: " <<		VarLibrary::getBool(	"entry bool")		<< std::endl;
		std::cout << "entry int: " <<		VarLibrary::getInt(		"entry int" )		<< std::endl;
		std::cout << "entry float: " <<		VarLibrary::getFloat(	"entry float" )		<< std::endl;
		std::cout << "entry double: " <<	VarLibrary::getDouble(	"entry double" )	<< std::endl;
		std::cout << "entry string: " <<	VarLibrary::getString(	"entry string" )	<< std::endl;

		std::cout << "dictionary: \"test dictionary\"" << std::endl;

		std::cout << "entry bool: " <<		VarLibrary::getBool(	"test dictionary", "entry bool")	<< std::endl;
		std::cout << "entry int: " <<		VarLibrary::getInt(		"test dictionary", "entry int" )	<< std::endl;
		std::cout << "entry float: " <<		VarLibrary::getFloat(	"test dictionary", "entry float" )	<< std::endl;
		std::cout << "entry double: " <<	VarLibrary::getDouble(	"test dictionary", "entry double" )	<< std::endl;
		std::cout << "entry string: " <<	VarLibrary::getString(	"test dictionary", "entry string" )	<< std::endl;

	}
	catch ( std::exception& e )
	{
		std::cout << std::endl << "unexpected error occured: " << std::endl << e.what() << std::endl << std::endl;
	}

*/

/*
	IDirectInput8* dInput = NULL;

	HRESULT hr = S_OK;
	if (FAILED(hr = DirectInput8Create( gethInst(), DIRECTINPUT_VERSION, IID_IDirectInput8W, (void**)&dInput, NULL)))
	{
		shutdown();
		std::cout << "DirectInput8Create error: " << DXGetErrorStringA(hr) << " error description: " << DXGetErrorDescriptionA(hr) << "\n";
		system("PAUSE");
		return -1;
	}
*/

/*
	Logger& syslog = ( Logger::addLogger( "syslog",LogPriority::DEBUG ) ).second;
	{
		FileLoggingInterface* li = new FileLoggingInterface( "syslog", "system.log", false, false );
		li->setThreshold( LogPriority::NOTSET );
		syslog.setLoggingInterface( li );
	}

	syslog << LogPriority::INFO << "nfo test number " << 1;
	syslog << LogPriority::INFO << "nfo test number " << 2;
	syslog << LogPriority::INFO << "nfo test number " << 3;
	syslog << LogPriority::INFO << "nfo test number " << 4;

	Logger::shutdown();
*/


	//system("PAUSE");
	return 0;
}
