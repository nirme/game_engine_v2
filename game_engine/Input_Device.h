#pragma once

#ifndef _INPUT_DEVICE
#define _INPUT_DEVICE


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

//DirectX8 Input
//#define DIRECTINPUT_VERSION  0x0800
//#include <dinput.h>
//#pragma comment (lib, "dinput8.lib")




// +--------------------------------------------------------------------------------------------------+
// |                                           Device data                                            |
// +-------------+------------------------------------------------------------------------------------+
// | name        | \\?\HID#VID_0810&PID_0003#6&2d68c10&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}  |
// +-------------+------------------------------------------------------------------------------------+
// | vendor id   |    0x0810                                                                          |
// +-------------+------------------------------------------------------------------------------------+
// | product id  |    0x0003                                                                          |
// +-------------+------------------------------------------------------------------------------------+
// | version     |    0x0106                                                                          |
// +-------------+------------------------------------------------------------------------------------+
// | usage page  |         1                                                                          |
// +-------------+------------------------------------------------------------------------------------+
// | usage       |         4                                                                          |
// +-------------+------------------------------------------------------------------------------------+
// +==================================================================================================+
// +--------------------------------------------------------------------------------------------------+
// |                                               CAPS                                               |
// +---------------------------------+----------------------------------------------------------------+
// | Input Report Length             |         9                                                      |                                      
// +---------------------------------+----------------------------------------------------------------+
// | Output Report Length            |         5                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Feature Report Length           |         0                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Link Collection Nodes Count     |         3                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Input Button Caps Count         |         2                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Input Value Caps Count          |         6                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Input Data Indices Count        |        19                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Output Button Caps Count        |         0                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Output Value Caps Count         |         1                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Output Data Indices Count       |         1                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Feature Button Caps Count       |         0                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Feature Value Caps Count        |         0                                                      |
// +---------------------------------+----------------------------------------------------------------+
// | Feature Data Indices Count      |         0                                                      |
// +---------------------------------+----------------------------------------------------------------+


//
//	+-------------------------+------------+-------+-----------------------------+
//	| Device Type             | Usage Page | Usage | Hardware ID                 |
//	+-------------------------+------------+-------+-----------------------------+
//	| Pointer                 |  0x01      | 0x01  | HID_DEVICE_SYSTEM_MOUSE     |
//	+-------------------------+------------+-------+-----------------------------+
//	| Mouse                   |  0x01      | 0x02  | HID_DEVICE_SYSTEM_MOUSE     |
//	+-------------------------+------------+-------+-----------------------------+
//	| Joystick                |  0x01      | 0x04  | HID_DEVICE_SYSTEM_GAME      |
//	+-------------------------+------------+-------+-----------------------------+
//	| Game pad                |  0x01      | 0x05  | HID_DEVICE_SYSTEM_GAME      |
//	+-------------------------+------------+-------+-----------------------------+
//	| Keyboard                |  0x01      | 0x06  | HID_DEVICE_SYSTEM_KEYBOARD  |
//	+-------------------------+------------+-------+-----------------------------+
//	| Keypad                  |  0x01      | 0x07  | HID_DEVICE_SYSTEM_KEYBOARD  |
//	+-------------------------+------------+-------+-----------------------------+
//	| System control          |  0x01      | 0x80  | HID_DEVICE_SYSTEM_CONTROL   |
//	+-------------------------+------------+-------+-----------------------------+
//	| Consumer audio control  |  0x0C      | 0x01  | HID_DEVICE_SYSTEM_CONSUMER  |
//	+-------------------------+------------+-------+-----------------------------+
//





//	INPUT REPORT
//	+------------------------------------------------------------------------+
//	|                 TRACER Red Arrow PC/PS2/PS3 ( enhanced state )         |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	| b\bit |   7   |   6   |   5   |   4    |   3   |   2   |   1   |   0   |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   0   |   0?  |   0?  |   0?  |   0?   |   0?  |   0?  |   0?  |   0?  |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   1   |   0?  |   0?  |   0?  |   0?   |   0?  |   0?  |   0?  |   1?  |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   2   |  right stick vertical    0 = UP,   128 = CENTER, 255 = DOWN    |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   3   |  right stick horizontal  0 = LEFT, 128 = CENTER, 255 = RIGHT   |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   4   |  left stick horizontal   0 = LEFT, 128 = CENTER, 255 = RIGHT   |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   5   |  left stick vertical     0 = UP,   128 = CENTER, 255 = DOWN    |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   6   | BTN-4 | BTN-3 | BTN-2 | BTN-1  | D-PAD*|     D-PAD encoded*    |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   7   |  R-S* |  L-S* | START | SELECT |   R1  |   L1  |   R2  |   L2  |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	|   8   |   0?  |   0?  |   0?  |   0?   |   0?  |   0?  |   0?  |   0?  |
//	+-------+-------+-------+-------+--------+-------+-------+-------+-------+
//	
//	D-PAD*: signs D-PAD usage with 0 normal state is 1
//	D-PAD encoded*: 
//	UP          0 0 0
//	UP-RIGHT    0 0 1
//	RIGHT       0 1 0
//	RIGHT-DOWN  0 1 1
//	DOWN        1 0 0
//	DOWN-LEFT   1 0 1
//	LEFT        1 1 0
//	UP-LEFT     1 1 1
//	
//	R-S*: Right Stick click
//	L-S*: Left Stick click
//	
//	
//	
//	
//	INPUT REPORT
//	+-----------------------------------------------------------------------+
//	|                       X360 compatible controller                       |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	| b\bit |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|   0   |                   Message type = 0x00                         |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|   1   |                   Packet size  = 0x14                         |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|   2   | R-S*  | L-S*  | back  | start |D-right| D-left| D-down| D-up  |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|   3   | Y     | X     | B     | A     | N/A   | X logo| RB    | LB    |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|   4   |                         Left trigger                          |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|   5   |                         Right trigger                         |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|  6  7 |                       Left stick X-axis                       |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|  8  9 |                       Left stick Y-axis                       |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	| 10 11 |                       Right stick X-axis                      |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	| 12 13 |                       Right stick Y-axis                      |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	|   14 15 16 17 18 19   |              N/A                              |
//	+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//	
//	R-S*: Right Stick click
//	L-S*: Left Stick click




namespace GameEngine
{

	#define    DEFAULT_RAW_INPUT_FLAGS    ( RIDEV_INPUTSINK | WM_INPUT_DEVICE_CHANGE )


	#define    HID_MOUSE_USAGE_PAGE       0x01
	#define    HID_MOUSE_USAGE            0x02

	#define    HID_KEYBOARD_USAGE_PAGE    0x01
	#define    HID_KEYBOARD_USAGE         0x06

	#define    HID_GAMEPAD_USAGE_PAGE     0x01
	#define    HID_GAMEPAD_USAGE          0x04


	#define    DEVICE_MOUSE               0x01
	#define    DEVICE_KEYBOARD            0x02
	#define    DEVICE_GAMEPAD             0x04



	#define    BUTTON_UP                  false
	#define    BUTTON_DOWN                true

	#define    KEY_UP                     false
	#define    KEY_DOWN                   true


	#define    MOUSE_BUTTON               0x10
	#define    MOUSE_AXIS                 0x20


	#define    MOUSE_BUTTON_1             0x10
	#define    MOUSE_BUTTON_2             0x11
	#define    MOUSE_BUTTON_3             0x12
	#define    MOUSE_BUTTON_4             0x13
	#define    MOUSE_BUTTON_5             0x14

	#define    MOUSE_BUTTON_LEFT          MOUSE_BUTTON_1
	#define    MOUSE_BUTTON_RIGHT         MOUSE_BUTTON_2
	#define    MOUSE_BUTTON_MIDDLE        MOUSE_BUTTON_3


	#define    MOUSE_AXIS_X               0x20
	#define    MOUSE_AXIS_Y               0x21
	#define    MOUSE_AXIS_Z               0x22
	#define    MOUSE_WHEEL                MOUSE_AXIS_Z



	// keyboard keys use VK keys scheme from windows message pump



	#define    HID_INPUT_GENERIC_CONTROLS_USAGE_PAGE    0x01

	#define    HID_INPUT_BTN_USAGE_PAGE                 0x09


	#define    HID_INPUT_HAT_SWITCH_USAGE_PAGE          HID_INPUT_GENERIC_CONTROLS_USAGE_PAGE
	#define    HID_INPUT_HAT_SWITCH_USAGE               0x39

	#define    HID_INPUT_DPAD_USAGE_PAGE                HID_INPUT_GENERIC_CONTROLS_USAGE_PAGE

	#define    HID_INPUT_DPAD_UP_USAGE                  0x90
	#define    HID_INPUT_DPAD_DOWN_USAGE                0x91
	#define    HID_INPUT_DPAD_RIGHT_USAGE               0x92
	#define    HID_INPUT_DPAD_LEFT_USAGE                0x93


	#define    HID_INPUT_AXES_USAGE_PAGE                HID_INPUT_GENERIC_CONTROLS_USAGE_PAGE

	#define    HID_INPUT_AXES_X_USAGE_PAGE              0x30
	#define    HID_INPUT_AXES_Y_USAGE_PAGE              0x31
	#define    HID_INPUT_AXES_Z_USAGE_PAGE              0x32

	#define    HID_INPUT_AXES_R_X_USAGE_PAGE            0x33
	#define    HID_INPUT_AXES_R_Y_USAGE_PAGE            0x34
	#define    HID_INPUT_AXES_R_Z_USAGE_PAGE            0x35

	#define    HID_INPUT_AXES_V_X_USAGE_PAGE            0x40
	#define    HID_INPUT_AXES_V_Y_USAGE_PAGE            0x41
	#define    HID_INPUT_AXES_V_Z_USAGE_PAGE            0x42




	#define    MATH_CONST_PI    3.14159265359


	#define    HID_GAMEPAD_BUTTONS    12
	#define    HAT_SWITCH_STEP_DEG    45
	#define    HAT_SWITCH_STEP_RAD    1.57079632679


	#define     DPAD_INDEX_UP		0x00
	#define     DPAD_INDEX_DOWN		0x01
	#define     DPAD_INDEX_RIGHT	0x02
	#define     DPAD_INDEX_LEFT		0x03


	#define     BTN_INDEX_BUTTON_Y			0x00
	#define     BTN_INDEX_BUTTON_B			0x01
	#define     BTN_INDEX_BUTTON_A			0x02
	#define     BTN_INDEX_BUTTON_X			0x03

	#define     BTN_INDEX_LEFT_1			0x04
	#define     BTN_INDEX_LEFT_BUMPER		0x04

	#define     BTN_INDEX_RIGHT_1			0x05
	#define     BTN_INDEX_RIGHT_BUMPER		0x05

	#define     BTN_INDEX_LEFT_2			0x06
	#define     BTN_INDEX_LEFT_TRIGGER		0x06

	#define     BTN_INDEX_RIGHT_2			0x07
	#define     BTN_INDEX_RIGHT_TRIGGER		0x07

	#define     BTN_INDEX_SELECT			0x08
	#define     BTN_INDEX_BACK				0x08

	#define     BTN_INDEX_START				0x09

	#define     BTN_INDEX_LEFT_STICK		0x0A
	#define     BTN_INDEX_RIGHT_STICK		0x0B



	#define	    STICK_INDEX_LEFT_X		0x00
	#define	    STICK_INDEX_LEFT_Y		0x01
	#define	    STICK_INDEX_RIGHT_X		0x02
	#define	    STICK_INDEX_RIGHT_Y		0x03











	struct ControlID;
	struct ControlValue;

	class InputCallback;

	class InputDevice;
//	struct InputDevice::CallbackPage;


	class RawInputDevice;

	class IDeviceMouse;
//	struct IDeviceMouse::MouseState;

	class IDeviceKeyboard;
//	struct IDeviceKeyboard::KeyboardState;

	class HIDInputDevice;
//	class HIDInputDevice::HIDIO;
//	class HIDInputDevice::HIDIOButton;
//	class HIDInputDevice::HIDIOValue;
//	struct HIDInputDevice::HIDIODeviceState;


	class HIDInputGamepad;
//	class HIDInputGamepad::HIDIOValueHatSwitch;
//	class HIDInputGamepad::HIDIOValueStickAxis;
//	struct HIDInputGamepad::GamepadState;
//	struct HIDInputGamepad::ValuesMapping;





	struct ControlID
	{
		union
		{
			unsigned int ui_id;
			struct
			{
				const BYTE bt_deviceId;
				const BYTE bt_controlUsagePage;
				const unsigned short bt_controlUsage;
			};
		};

		ControlID() : bt_deviceId( 0 ), bt_controlUsagePage( 0 ), bt_controlUsage( 0 ), ui_id( 0 ) {};
		ControlID( BYTE _bt_deviceId, BYTE _bt_controlUsagePage, unsigned short _bt_controlUsage ) : ui_id( 0 ), bt_deviceId( _bt_deviceId ), bt_controlUsagePage( _bt_controlUsagePage ), bt_controlUsage( _bt_controlUsage ) {};
		ControlID( unsigned int _ui_id ) : bt_deviceId( 0 ), bt_controlUsagePage( 0 ), bt_controlUsage( 0 ), ui_id( _ui_id ) {};
		ControlID( const ControlID& _rhs ) : bt_deviceId( 0 ), bt_controlUsagePage( 0 ), bt_controlUsage( 0 ), ui_id( _rhs.ui_id ) {};

		inline unsigned int getID() { return ui_id; };
		inline BYTE getDeviceId() { return bt_deviceId; };
		inline BYTE getControlUsagePage() { return bt_controlUsagePage; };
		inline unsigned short getControlUsage() { return bt_controlUsage; };

		inline ControlID& operator = ( const ControlID& _rhs ) { ui_id = _rhs.ui_id; return *this; };
		inline bool operator == ( const ControlID& _rhs ) const { return ui_id == _rhs.ui_id; };
		inline bool operator != ( const ControlID& _rhs ) const { return ui_id != _rhs.ui_id; };
		inline bool operator <  ( const ControlID& _rhs ) const { return ui_id <  _rhs.ui_id; };
		inline bool operator >  ( const ControlID& _rhs ) const { return ui_id >  _rhs.ui_id; };
		inline bool operator <= ( const ControlID& _rhs ) const { return ui_id <= _rhs.ui_id; };
		inline bool operator >= ( const ControlID& _rhs ) const { return ui_id >= _rhs.ui_id; };
	};

	struct ControlValue
	{
		enum ValueType
		{
			type_float = 0x00,
			type_int = 0x01,
			type_bool = 0x02,
			type_error = 0xff,
		};

		const ValueType vt_type;

		union
		{
			const float f_value;
			const int   i_value;
			const bool  b_value;
		    const ULONG ul_rawValue;
		};


		ControlValue() : i_value( 0 ), vt_type( type_error ) {};
		ControlValue( float _f_value ) : f_value( _f_value ), vt_type( type_float ) {};
		ControlValue( int _i_value ) : i_value( _i_value ), vt_type( type_int ) {};
		ControlValue( bool _b_value ) : b_value( _b_value ), vt_type( type_bool ) {};
	};


	class InputCallback
	{
	public:

		const std::string str_name;

		InputCallback( const std::string& _str_name ) : str_name( _str_name ) {};
		virtual ~InputCallback() {};

		virtual void call( ControlValue _cv_value ) = 0;
	};


	class InputDevice
	{
	private:

		typedef boost::container::flat_map< unsigned short /*control id*/, std::list< InputCallback* > > CallbackMap;

		struct CallbackPage
		{
			std::string str_name;
			CallbackMap cm_onMsgCallbacks;
			CallbackMap cm_onPollCallbacks;

			CallbackPage( const std::string& _str_name );
			CallbackPage( CallbackPage&& _rhs );
			~CallbackPage();

			CallbackPage& operator=( CallbackPage&& _right );

			void registerOnMsgCallback( ControlID _ci_controlId, InputCallback* _ic_callback );
			void registerOnPollCallback( ControlID _ci_controlId, InputCallback* _ic_callback );

			void callOnMsgEvent( ControlID _ci_controlId, ControlValue _cv_controlValue );
			void callOnPollEvent( ControlID _ci_controlId, ControlValue _cv_controlValue );
		};


		typedef boost::container::flat_map< const std::string, CallbackPage > CallbackPages;

		typedef std::map< UINT /*_ui_controlId*/, short /*_s_value*/ > EventsMap;

		typedef std::pair< ControlID, ControlValue > Event;
		typedef std::list< std::pair< ControlID, ControlValue > > EventQueue;


		const std::string str_name;
		const BYTE bt_localDeviceID;

		std::map< std::string, CallbackPage > m_callbackPages;
		CallbackPage* cp_actualPage;

		EventQueue l_events;
		EventQueue l_eventsHistory;

		CallbackPage* getCallbackPage( std::string _str_callbackPageName );


	protected:

		void pushEvent( ControlID _ci_controlId, ControlValue _cv_controlValue );
		void pollEvents();


	public:

		InputDevice( std::string _str_name, BYTE _bt_localDeviceID );
		virtual ~InputDevice() {};

		inline std::string getName() { return str_name; };
		inline BYTE getLocalDeviceID() { return bt_localDeviceID; };

		void setCallbackPage( const std::string& _str_callbackPageName );

		void registerOnMsgCallback( const std::string& _str_callbackPageName, ControlID _ci_controlId, InputCallback* _ic_callback );
		void registerOnPollCallback( const std::string& _str_callbackPageName, ControlID _ci_controlId, InputCallback* _ic_callback );

		void registerOnMsgCallback( ControlID _ci_controlId, InputCallback* _ic_callback );
		void registerOnPollCallback( ControlID _ci_controlId, InputCallback* _ic_callback );

		EventQueue::const_iterator getEventQueueFirst();
		EventQueue::const_iterator getEventQueueLast();
		unsigned int getEventQueueSize();

		virtual ControlValue getState( ControlID _ci_controlId ) = 0;
		virtual ControlValue getPolledState( ControlID _ci_controlId ) = 0;
		virtual void poll() = 0;
	};


	class RawInputDevice : public InputDevice
	{
	private:

		bool b_rawInputRegistered;
		RAWINPUTDEVICE rid_deviceInfo;
		const HWND hwnd_targetWindowHandle;
		const HANDLE handle_deviceHandle;


	protected:
		inline bool isRegistered() { return b_rawInputRegistered; };

	public:

		RawInputDevice( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle, unsigned short _us_usagePage, unsigned short _us_usage );
		~RawInputDevice();

		inline HWND getTargetWindowHandle() { return hwnd_targetWindowHandle; };
		inline HANDLE getDeviceHandle() { return handle_deviceHandle; };

		virtual DWORD registerDevice( DWORD _dw_additionalFlags = 0 );
		virtual DWORD unregisterDevice();

		virtual bool processRawInput( const RAWINPUT& ri_input ) = 0;
		bool processMessage( HWND hwnd, WPARAM wParam, LPARAM lParam );
	};


	class IDeviceMouse : public RawInputDevice
	{
	private:

		struct MouseState
		{
			union
			{
				bool b_button[5];
				struct
				{
					bool b_leftButton;
					bool b_rightButton;
					bool b_middleButton;
					bool b_x1Button;
					bool b_x2Button;
				};
			};

			union
			{
				int i_axis[3];
				struct
				{
					int i_axisX;
					int i_axisY;
					union
					{
						int i_axisZ;
						int i_wheel;
					};
				};
			};

			MouseState();
			MouseState( const MouseState& _rhs );
			MouseState& operator= ( const MouseState& _rhs );
		};

		MouseState ms_state;
		MouseState ms_stateHistory;


	public:

		IDeviceMouse( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle );
		~IDeviceMouse();

		ControlValue getState( ControlID _ci_controlId );
		ControlValue getPolledState( ControlID _ci_controlId );

		void poll();

		MouseState getStateStruct();
		MouseState getPolledStateStruct();

		bool processRawInput( const RAWINPUT& _ri_input );
	};


	class IDeviceKeyboard : public RawInputDevice
	{
	private:

		struct KeyboardState
		{
			bool b_key[255];

			KeyboardState();
			KeyboardState( const KeyboardState& _rhs );
			KeyboardState& operator= ( const KeyboardState& _rhs );
		};

		KeyboardState ks_state;
		KeyboardState ks_stateHistory;


	public:

		IDeviceKeyboard( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle );
		~IDeviceKeyboard();

		ControlValue getState( ControlID _ci_controlId );
		ControlValue getPolledState( ControlID _ci_controlId );

		void poll();

		KeyboardState getStateStruct();
		KeyboardState getPolledStateStruct();

		bool processRawInput( const RAWINPUT& _ri_input );
	};


	class HIDInputDevice : public RawInputDevice
	{
	protected:

		enum class HIDIOType : BYTE
		{
			button = 0x01,
			value = 0x02,
		};

		class HIDIO
		{
		private:

			const HIDIOType hiot_inputType;
			const unsigned short us_usagePage;
			const unsigned short us_usage;

			const unsigned short us_dataIndex;
			const ControlID ci_controlID;


		public:

			HIDIO( HIDIOType _hiot_inputType, unsigned short _us_dataIndex, unsigned short _us_usagePage, unsigned short _us_usage, BYTE _bt_deviceId );
			HIDIO( const HIDIO& _rhs );
			HIDIO( const HIDIO&& _rhs );

			inline HIDIOType getInputType() const { return hiot_inputType; };
			inline unsigned short getUsagePage() const { return us_usagePage; };
			inline unsigned short getUsage() const { return us_usage; };
			inline unsigned short getDataIndex() const { return us_dataIndex; };
			inline ControlID getID() const { return ci_controlID; };

			virtual void cloneValue( HIDIO* _hiob_value ) = 0;
			virtual HIDIO* clone() = 0;
			virtual ControlValue getValue() = 0;
		};

		class HIDIOButton : public HIDIO
		{
		protected:
			bool b_value;

		public:
			HIDIOButton( unsigned short _us_dataIndex, unsigned short _us_usage, BYTE _bt_deviceId );
			HIDIOButton( unsigned short _us_dataIndex, unsigned short _us_usagePage, unsigned short _us_usage, BYTE _bt_deviceId );
			
			HIDIOButton( const HIDIOButton& _rhs );

			bool set( bool _b_value );
			bool set( BOOLEAN _B_value );

			inline bool get() { return b_value; };
			inline operator bool () { return b_value; };

			void operator = ( bool _b_value );
			virtual void cloneValue( HIDIO* _hiob_value );
			virtual HIDIO* clone();
			virtual ControlValue getValue();
		};

		class HIDIOValue : public HIDIO
		{
		protected:
			int i_min;
			int i_max;
			bool b_hasNull;

			unsigned int ui_value;
			bool b_isNull;

			virtual void set_impl() {};

		public:

			HIDIOValue( unsigned short _us_dataIndex, unsigned short _us_usagePage, unsigned short _us_usage, int _i_min, int _i_max, bool _b_hasNull, BYTE _bt_deviceId );
			HIDIOValue( const HIDIOValue& _rhs );
			HIDIOValue( const HIDIOValue&& _rhs );

			bool set( unsigned int _ui_value );

			inline unsigned int getRawValue() { return ui_value; };
			inline operator unsigned int () { return ui_value; };
			inline bool isNull() { return b_isNull; };

			HIDIOValue& operator = ( unsigned int _rhs );
			virtual void cloneValue( HIDIO* _hiob_value );
			virtual HIDIO* clone();
			virtual ControlValue getValue();

			inline int getMin() const { return i_min; };
			inline int getMax() const { return i_max; };
			inline bool getHasNull() const { return b_hasNull; };
		};

		struct HIDIODeviceState
		{
			friend class HIDInputDevice;

			std::vector< HIDIO* > vec_inputs;
			std::vector< HIDIO* > vec_outputs;


			~HIDIODeviceState();

		private:
			HIDIODeviceState();
			HIDIODeviceState( const HIDIODeviceState& _rhs );
			HIDIODeviceState( HIDIODeviceState&& _rhs );

			void setUp( const std::vector< HIDIO* >& _vec_inputs, const std::vector< HIDIO* >& _vec_outputs );
			void clear();
		};

		std::string str_deviceSystemString;
		std::string str_productString;
		std::string str_physicalDescriptor;
		std::string str_serialNumberString;
		std::string str_manufacturerString;

		DWORD dw_vendorId;
		DWORD dw_productId;
		DWORD dw_versionNumber;
		std::vector<BYTE> v_preparsedData;
		BYTE* pbt_preparsedDataPointer;

		HIDP_CAPS hc_caps;

		typedef boost::container::flat_map< ControlID, HIDIO* > flat_map;

		std::vector< HIDIO* > vec_indexedInputs;
		flat_map map_mappedInputs;

		// poll history still needs optimisation
		std::vector< HIDIO* > vec_indexedInputsHistory;
		flat_map map_mappedInputsHistory;


		std::vector< HIDIO* > vec_indexedOutputs;
		flat_map map_mappedOutputs;


		std::vector< HIDP_DATA > vec_dataList;
		ULONG ul_dataListLength;
		HIDP_DATA* hd_dataListPointer;


		void remapInput( int _i_inputIndex, HIDIO* _hio_input );
		bool addVirtualInput( HIDIO* _hio_input );

	public:

		HIDInputDevice( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle, unsigned short _us_usagePage, unsigned short _us_usage );
		virtual ~HIDInputDevice();

		std::string getDeviceSystemString();
		std::string getProductString();
		std::string getPhysicalDescriptor();
		std::string getSerialNumberString();
		std::string getManufacturerString();


		virtual DWORD registerDevice( DWORD _dw_additionalFlags = 0 );
		virtual DWORD unregisterDevice();



		ControlValue getState( ControlID _ci_controlId );
		ControlValue getPolledState( ControlID _ci_controlId );

		virtual bool setState( ControlID _ci_controlId, ControlValue _cv_state );

		unsigned int getInputControls( ControlID* _ci_controlsArray );
		std::vector<ControlID> getInputControls();

		unsigned int getOutputControls( ControlID* _ci_controlsArray );
		std::vector<ControlID> getOutputControls();


		virtual void poll();
		virtual bool processRawInput( const RAWINPUT& _ri_input );

		void getDataList( const RAWINPUT& _ri_input );

		HIDIODeviceState getStateStruct();
		HIDIODeviceState getPolledStateStruct();

	};



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////														////
////	BASIC GAMEPAD STRUCTURE								////
////	                                                	////
////	B7 - L2 - L TRIGGER			B8 - R2 - R TRIGGER		////
////	B5 - L1 - L BUMPER			B6 - R1 - R BUMPER		////
////		___                                         	////
////	   / U \						B1 - Y          	////
////	  |L * R|				B4 - X			B2 - B  	////
////	   \_D_/						B3 - A          	////
////	HAT SWITCH - D-PAD                              	////
////	                                                	////
////		B9 - SELECT - BACK		B10 - START         	////
////	                                                	////
////	L STICK							R STICK         	////
////	AX1 - AXIS X					AX3 - AXIS X    	////
////	IAX2 - AXIS Y					IAX6 - AXIS Y   	////
////	B11 - BUTTON					B12 - BUTTON    	////
////	                                                	////
////	                                                	////
////	ADDITIONALLY :                                  	////
////	FORCE FEEDBACK                                  	////
////														////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


	class HIDInputGamepad : public HIDInputDevice
	{
	public:

		enum class HatSwitchStep : BYTE
		{
			UP          = 0x00,
			RIGHT_UP    = 0x01,
			RIGHT       = 0x02,
			RIGHT_DOWN  = 0x03,
			DOWN        = 0x04,
			LEFT_DOWN   = 0x05,
			LEFT        = 0x06,
			LEFT_UP     = 0x07,
			UNUSED      = 0x15,
		};

		class HIDIOValueHatSwitch : public HIDIOValue
		{
		protected:
			HatSwitchStep hss_step;

			virtual void set_impl();

		public:
			HIDIOValueHatSwitch( unsigned short _us_dataIndex, unsigned short _us_usagePage, unsigned short _us_usage, int _i_min, int _i_max, bool _b_hasNull, BYTE _bt_deviceId );
			HIDIOValueHatSwitch( const HIDIOValueHatSwitch& _rhs );
			HIDIOValueHatSwitch( const HIDIOValueHatSwitch&& _rhs );

			HIDIOValueHatSwitch( const HIDIOValue& _rhs ) : HIDIOValue( _rhs ), hss_step( HatSwitchStep::UNUSED )
			{};

			inline HatSwitchStep getStep() { return hss_step; };

			void cloneValue( HIDIO* _hiob_value );
			virtual HIDIO* clone();

			ControlValue getValue();
		};

		class HIDIOValueStickAxis : public HIDIOValue
		{
		protected:
			int i_center;
			float f_value;

			virtual void set_impl();

		public:

			HIDIOValueStickAxis( const HIDIOValueStickAxis& _rhs );
			HIDIOValueStickAxis( const HIDIOValue&  _rhs );
			HIDIOValueStickAxis( const HIDIOValue&& _rhs );
			HIDIOValueStickAxis( const HIDIOValue& _rhs, ControlID _ci_id );

			inline float getNormalizedAxis() { return f_value; };

			void cloneValue( HIDIO* _hiob_value );

			HIDIO* clone();

			ControlValue getValue();

		};

		struct GamepadState
		{
		public:

			//DPAD state
			union
			{
				bool b_dPad[ 4 ];
				struct
				{
					bool b_dPad_UP;
					bool b_dPad_DOWN;
					bool b_dPad_RIGHT;
					bool b_dPad_LEFT;
				};
			};

			//buttons state
			union
			{
				bool b_buttons[ HID_GAMEPAD_BUTTONS ];

				struct
				{
					bool b_buttonY;
					bool b_buttonB;
					bool b_buttonA;
					bool b_buttonX;
					bool b_left1;
					bool b_right1;
					bool b_left2;
					bool b_right2;
					bool b_select;
					bool b_start;
					bool b_leftStickButton;
					bool b_rightStickButton;
				};

				struct
				{
					bool b_reserved1;
					bool b_reserved2;
					bool b_reserved3;
					bool b_reserved4;
					bool b_leftBumper;
					bool b_rightBumper;
					bool b_leftTrigger;
					bool b_rightTrigger;
					bool b_back;
					bool b_reserver10;
					bool b_reserver11;
					bool b_reserver12;
				};
			};

			//analog sticks state normalized

			union
			{
				float f_stickAxes[ 4 ];
				struct
				{
					float f_leftStickAxisX;
					float f_leftStickAxisY;
					float f_rightStickAxisX;
					float f_rightStickAxisY;
				};
			};


			GamepadState();
			void clear();
		};

		struct ValuesMapping
		{
			BYTE bt_hatSwitchShift;
			BYTE bt_dPap[ 4 ];
			BYTE bt_buttons[ HID_GAMEPAD_BUTTONS ];
			BYTE bt_stickAxes[ 4 ];

			std::string serialize() const;
			void unserialize( const std::string _str_serializedString );

			ValuesMapping();
			void clear();

			ValuesMapping& operator= ( const ValuesMapping& _rhs );
		};

	private:

	// hard set control ids to look for in device:
	//
	// hat switch -> this should be translated into 4 dpad buttons
	// ControlID( _bt_localDeviceID, HID_INPUT_HAT_SWITCH_USAGE_PAGE, HID_INPUT_HAT_SWITCH_USAGE )
	//
	// dpad buttons
	// ControlID( _bt_localDeviceID, HID_INPUT_DPAD_USAGE_PAGE, { HID_INPUT_DPAD_UP_USAGE, HID_INPUT_DPAD_DOWN_USAGE, HID_INPUT_DPAD_RIGHT_USAGE, HID_INPUT_DPAD_LEFT_USAGE } )
	//
	// buttons -> this ones very straight forward, buttons have different usage page than generic devices/controls and the isage is the index of button held down starting with 1, zero means buttons are not used
	// ControlID( _bt_localDeviceID, HID_INPUT_BTN_USAGE_PAGE,  { 1 - 12 } )
	//
	// stick axes -> this should be translated into usages X, Y and R_X, R_Y in the usage order: if found X, Y, Z, R_X : X->X, Y->Y, Z->R_X, R_X->R_Y etc
	// ControlID( _bt_localDeviceID, HID_INPUT_AXES_USAGE_PAGE, { HID_INPUT_AXES_X_USAGE_PAGE,   HID_INPUT_AXES_Y_USAGE_PAGE,   HID_INPUT_AXES_Z_USAGE_PAGE
	//															  HID_INPUT_AXES_R_X_USAGE_PAGE, HID_INPUT_AXES_R_Y_USAGE_PAGE, HID_INPUT_AXES_R_Z_USAGE_PAGE
	//															  HID_INPUT_AXES_V_X_USAGE_PAGE, HID_INPUT_AXES_V_Y_USAGE_PAGE, HID_INPUT_AXES_V_Z_USAGE_PAGE } )
	//


		typedef bool (HIDInputGamepad::*pt2UpdateFunc)( BYTE, const HIDP_DATA& );

		// values remapping
		// this part needs to maps the inputs before it is registered into the device state
		ValuesMapping vm_userMapping;
		std::string str_serializedMapping;

		//state
		GamepadState gs_state;
		GamepadState gs_stateHistory;


		// pointers to actual values from base class
		HIDIOValueHatSwitch* hio_hatSwitchValuePtr;
		HIDIOButton* hio_dPadValuePtr[ 4 ];
		HIDIOButton* hio_buttonsValuePtr[ HID_GAMEPAD_BUTTONS ];
		HIDIOValueStickAxis* hio_stickAxisValuePtr[ 4 ];

		// indexed update functions to speedup state update, gamepads send update data once every 32ms or so with at least list of all values
		std::vector< std::pair< int /*index*/, pt2UpdateFunc > > vec_reportIndicesUpdateFuncMaping;


		// update function for hookup
		bool _updateFuncDPad( BYTE _bt_innerIndex, const HIDP_DATA& _hd_inputData );
		bool _updateFuncDPadFromHatSwitch( BYTE _bt_innerIndex, const HIDP_DATA& _hd_inputData );
		bool _updateFuncButton( BYTE _bt_innerIndex, const HIDP_DATA& _hd_inputData );
		bool _updateFuncAxis( BYTE _bt_innerIndex, const HIDP_DATA& _hd_inputData );


	public:

		HIDInputGamepad( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle, unsigned short _us_usagePage, unsigned short _us_usage );

		void setSerializedMapping( const std::string _str_serializedString );
		std::string getSerializedMappings();
		void setMapping( const ValuesMapping& _vm_mapping );

		DWORD registerDevice( DWORD _dw_additionalFlags = 0 );
		DWORD unregisterDevice();

		void poll();
		virtual bool processRawInput( const RAWINPUT& _ri_input );

// virtual bool setState( ControlID _ci_controlId, ControlValue _cv_state );
// vibrations?

	};

}; //namespace GameEngine

#endif //_INPUT_DEVICE
