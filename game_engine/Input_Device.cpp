#include "Input_Device.h"

#pragma warning(disable:4996)
#pragma warning(disable:4018)

namespace GameEngine
{

	////////////////////////////////////////////////////////////////////////////////
	////////                   InputDevice::CallbackPage                    ////////
	////////////////////////////////////////////////////////////////////////////////

	InputDevice::CallbackPage::CallbackPage( const std::string& _str_name ) : 
			str_name( _str_name )
	{};


	InputDevice::CallbackPage::CallbackPage( CallbackPage&& _rhs )
	{
		std::swap( str_name, _rhs.str_name );
		std::swap( cm_onMsgCallbacks, _rhs.cm_onMsgCallbacks );
		std::swap( cm_onPollCallbacks, _rhs.cm_onPollCallbacks );
	};


	InputDevice::CallbackPage::~CallbackPage()
	{
		for ( CallbackMap::iterator it = cm_onMsgCallbacks.begin(); it != cm_onMsgCallbacks.end(); ++it )
		{
			while( it->second.size() )
			{
				delete it->second.back();
				it->second.pop_back();
			}
		}

		cm_onMsgCallbacks.clear();


		for ( CallbackMap::iterator it = cm_onPollCallbacks.begin(); it != cm_onPollCallbacks.end(); ++it )
		{
			while( it->second.size() )
			{
				delete it->second.back();
				it->second.pop_back();
			}
		}

		cm_onPollCallbacks.clear();
	};


	InputDevice::CallbackPage& InputDevice::CallbackPage::operator=( CallbackPage&& _right )
	{	// move from _Right
		std::swap( str_name, _right.str_name );
		cm_onMsgCallbacks.swap( _right.cm_onMsgCallbacks );
		cm_onPollCallbacks.swap( _right.cm_onPollCallbacks );
		return (*this);
	};


	void InputDevice::CallbackPage::registerOnMsgCallback( ControlID _ci_controlId, InputCallback* _ic_callback )
	{
		CallbackMap::iterator it = cm_onMsgCallbacks.find( _ci_controlId.getID() );
		if ( it == cm_onMsgCallbacks.end() )
		{
			it = cm_onMsgCallbacks.insert( CallbackMap::value_type( _ci_controlId.getID(), std::list< InputCallback* >::list() ) ).first;
		}

		it->second.push_back( _ic_callback );
	};


	void InputDevice::CallbackPage::registerOnPollCallback( ControlID _ci_controlId, InputCallback* _ic_callback )
	{
		CallbackMap::iterator it = cm_onPollCallbacks.find( _ci_controlId.getID() );
		if ( it == cm_onPollCallbacks.end() )
		{
			it = cm_onPollCallbacks.insert( CallbackMap::value_type( _ci_controlId.getID(), std::list< InputCallback* >::list() ) ).first;
		}

		it->second.push_back( _ic_callback );
	};


	void InputDevice::CallbackPage::callOnMsgEvent( ControlID _ci_controlId, ControlValue _cv_controlValue )
	{
		CallbackMap::iterator it = cm_onMsgCallbacks.find( _ci_controlId.getID() );
		if ( it != cm_onMsgCallbacks.end() )
		{
			std::list< InputCallback* >::iterator itl = it->second.begin();
			while ( itl != it->second.end() )
			{
				(*itl)->call( _cv_controlValue );
				++itl;
			}
		}
	};


	void InputDevice::CallbackPage::callOnPollEvent( ControlID _ci_controlId, ControlValue _cv_controlValue )
	{
		CallbackMap::iterator it = cm_onPollCallbacks.find( _ci_controlId.getID() );
		if ( it != cm_onPollCallbacks.end() )
		{
			std::list< InputCallback* >::iterator itl = it->second.begin();
			while ( itl != it->second.end() )
			{
				(*itl)->call( _cv_controlValue );
				++itl;
			}
		}
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                          InputDevice                           ////////
	////////////////////////////////////////////////////////////////////////////////

	InputDevice::CallbackPage* InputDevice::getCallbackPage( std::string _str_callbackPageName )
	{
		std::map< std::string, CallbackPage >::iterator it = m_callbackPages.find( _str_callbackPageName );

		if ( it == m_callbackPages.end() )
		{
			it = m_callbackPages.insert( std::make_pair( _str_callbackPageName, CallbackPage( _str_callbackPageName ) ) ).first;
		}

		return &( it->second );
	};


	void InputDevice::pushEvent( ControlID _ci_controlId, ControlValue _cv_controlValue )
	{
		//push into history
		l_events.push_back( std::make_pair( _ci_controlId, _cv_controlValue ) );

		// and now look through callback page
		cp_actualPage->callOnMsgEvent( _ci_controlId, _cv_controlValue );
	};


	void InputDevice::pollEvents()
	{
		// empty history queue
		l_eventsHistory.clear();

		// put events to history queue
		std::swap( l_events, l_eventsHistory );

		// and now look through callback page
		for ( EventQueue::iterator it = l_eventsHistory.begin(); it != l_eventsHistory.end(); ++it )
		{
			cp_actualPage->callOnPollEvent( it->first, it->second );
		}
	};


	InputDevice::InputDevice( std::string _str_name, BYTE _bt_localDeviceID ) : 
			str_name( _str_name ), 
			bt_localDeviceID( _bt_localDeviceID ), 
			cp_actualPage( NULL )
	{
		m_callbackPages.insert( std::make_pair( std::string(""), CallbackPage( std::string("") ) ) );
		cp_actualPage = &( m_callbackPages.begin()->second );
	};


	void InputDevice::setCallbackPage( const std::string& _str_callbackPageName )
	{
		cp_actualPage = getCallbackPage( _str_callbackPageName );
	};


	void InputDevice::registerOnMsgCallback( const std::string& _str_callbackPageName, ControlID _ci_controlId, InputCallback* _ic_callback )
	{
		getCallbackPage( _str_callbackPageName )->registerOnMsgCallback( ControlID( getLocalDeviceID(), _ci_controlId.getControlUsagePage(), _ci_controlId.getControlUsage() ), _ic_callback );
	};


	void InputDevice::registerOnPollCallback( const std::string& _str_callbackPageName, ControlID _ci_controlId, InputCallback* _ic_callback )
	{
		getCallbackPage( _str_callbackPageName )->registerOnPollCallback( ControlID( getLocalDeviceID(), _ci_controlId.getControlUsagePage(), _ci_controlId.getControlUsage() ), _ic_callback );
	};


	void InputDevice::registerOnMsgCallback( ControlID _ci_controlId, InputCallback* _ic_callback )
	{
		cp_actualPage->registerOnMsgCallback( ControlID( getLocalDeviceID(), _ci_controlId.getControlUsagePage(), _ci_controlId.getControlUsage() ), _ic_callback );
	};


	void InputDevice::registerOnPollCallback( ControlID _ci_controlId, InputCallback* _ic_callback )
	{
		cp_actualPage->registerOnPollCallback( ControlID( getLocalDeviceID(), _ci_controlId.getControlUsagePage(), _ci_controlId.getControlUsage() ), _ic_callback );
	};


	InputDevice::EventQueue::const_iterator InputDevice::getEventQueueFirst()
	{
		return l_eventsHistory.begin();
	};


	InputDevice::EventQueue::const_iterator InputDevice::getEventQueueLast()
	{
		return l_eventsHistory.end();
	};


	unsigned int InputDevice::getEventQueueSize()
	{
		return l_eventsHistory.size();
	};


					
	////////////////////////////////////////////////////////////////////////////////
	////////                         RawInputDevice                         ////////
	////////////////////////////////////////////////////////////////////////////////

	RawInputDevice::RawInputDevice( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle, unsigned short _us_usagePage, unsigned short _us_usage ) : 
			InputDevice( _str_name, _bt_localDeviceID ), 
			b_rawInputRegistered( false ), 
			handle_deviceHandle( _handle_deviceHandle ), 
			hwnd_targetWindowHandle( _hwnd_targetWindowHandle )
	{
		rid_deviceInfo.hwndTarget = _hwnd_targetWindowHandle;
		rid_deviceInfo.usUsagePage = _us_usagePage;
		rid_deviceInfo.usUsage = _us_usage;
		rid_deviceInfo.dwFlags = DEFAULT_RAW_INPUT_FLAGS;
	};


	RawInputDevice::~RawInputDevice()
	{
		if ( b_rawInputRegistered )
		{
			unregisterDevice();
		}
	};


	DWORD RawInputDevice::registerDevice( DWORD _dw_additionalFlags )
	{
		if ( !b_rawInputRegistered )
		{
			rid_deviceInfo.dwFlags = DEFAULT_RAW_INPUT_FLAGS | _dw_additionalFlags;
						
			if ( RegisterRawInputDevices( &rid_deviceInfo, 1, sizeof( rid_deviceInfo ) ) )
			{
				DWORD dw_error = GetLastError();
				char c_errMsg[ 1024 ];
				FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 1024, NULL );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
				return dw_error;
			}

			b_rawInputRegistered = true;
			return 0;
		}
		else
		{
			std::string err = std::string( "Device " ) + getName() + " already registered";
			THROW_SYSTEM_ERROR( err );
		}

		return 0;
	};


	DWORD RawInputDevice::unregisterDevice()
	{
		if ( b_rawInputRegistered )
		{
			RAWINPUTDEVICE rid_tmpDeviceInfo;
			rid_tmpDeviceInfo.hwndTarget = 0;
			rid_tmpDeviceInfo.usUsage = rid_deviceInfo.usUsage;
			rid_tmpDeviceInfo.usUsagePage = rid_deviceInfo.usUsagePage;
			rid_tmpDeviceInfo.dwFlags = RIDEV_REMOVE;

			if ( RegisterRawInputDevices( &rid_tmpDeviceInfo, 1, sizeof( rid_tmpDeviceInfo ) ) )
			{
				DWORD dw_error = GetLastError();
				char c_errMsg[ 2048 ];
				FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 2048, NULL );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
				return dw_error;
			}

			rid_deviceInfo.dwFlags = DEFAULT_RAW_INPUT_FLAGS;
			b_rawInputRegistered = false;
		}

		return 0;
	};


	bool RawInputDevice::processMessage( HWND hwnd, WPARAM wParam, LPARAM lParam )
	{
		RAWINPUTHEADER rih_header;
		UINT ui_size = sizeof( RAWINPUTHEADER );
		GetRawInputData( ( HRAWINPUT ) lParam, RID_HEADER, &rih_header, &ui_size, sizeof( RAWINPUTHEADER ) );

		if ( rih_header.hDevice == getDeviceHandle() )
		{
			RAWINPUT ri_input;
			ui_size = sizeof( RAWINPUT );
			GetRawInputData( ( HRAWINPUT ) lParam, RID_INPUT, &ri_input, &ui_size, sizeof( RAWINPUTHEADER ) );
			return processRawInput( ri_input );
		}

		return false;
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                    IDeviceMouse::MouseState                    ////////
	////////////////////////////////////////////////////////////////////////////////

	IDeviceMouse::MouseState::MouseState() : 
			b_leftButton( false ), 
			b_rightButton( false ), 
			b_middleButton( false ), 
			b_x1Button( false ), 
			b_x2Button( false ), 
			i_axisX( 0 ), 
			i_axisY( 0 ), 
			i_axisZ( 0 )
	{};


	IDeviceMouse::MouseState::MouseState( const MouseState& _rhs ) : 
			b_leftButton( _rhs.b_leftButton ), 
			b_rightButton( _rhs.b_rightButton ), 
			b_middleButton( _rhs.b_middleButton ), 
			b_x1Button( _rhs.b_x1Button ), 
			b_x2Button( _rhs.b_x2Button ), 
			i_axisX( _rhs.i_axisX ), 
			i_axisY( _rhs.i_axisY ), 
			i_axisZ( _rhs.i_axisZ )
	{};


	IDeviceMouse::MouseState& IDeviceMouse::MouseState::operator= ( const MouseState& _rhs )
	{
		b_leftButton = _rhs.b_leftButton;
		b_rightButton = _rhs.b_rightButton;
		b_middleButton = _rhs.b_middleButton;
		b_x1Button = _rhs.b_x1Button;
		b_x2Button = _rhs.b_x2Button;
		i_axisX = _rhs.i_axisX;
		i_axisY = _rhs.i_axisY;
		i_axisZ = _rhs.i_axisZ;
		return *this;
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                          IDeviceMouse                          ////////
	////////////////////////////////////////////////////////////////////////////////

	IDeviceMouse::IDeviceMouse( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle ) : 
			RawInputDevice( _str_name, _bt_localDeviceID, _handle_deviceHandle, _hwnd_targetWindowHandle, HID_MOUSE_USAGE_PAGE, HID_MOUSE_USAGE )
	{};


	IDeviceMouse::~IDeviceMouse()
	{};


	ControlValue IDeviceMouse::getState( ControlID _ci_controlId )
	{
		if ( _ci_controlId.getControlUsagePage() == DEVICE_MOUSE )
		{
			if ( ( _ci_controlId.getControlUsage() & 0xF0 ) == MOUSE_BUTTON )
			{
				return ControlValue( ms_state.b_button[ _ci_controlId.getControlUsage() & 0x0F ] );
			}

			if ( ( _ci_controlId.getControlUsage() & 0xF0 ) == MOUSE_AXIS )
			{
				return ControlValue( ms_state.i_axis[ _ci_controlId.getControlUsage() & 0x0F ] );
			}
		}

		return ControlValue();
	};


	ControlValue IDeviceMouse::getPolledState( ControlID _ci_controlId )
	{
		if ( _ci_controlId.getControlUsagePage() == DEVICE_MOUSE )
		{
			if ( ( _ci_controlId.getControlUsage() & 0xF0 ) == MOUSE_BUTTON )
			{
				return ControlValue( ms_stateHistory.b_button[ _ci_controlId.getControlUsage() & 0x0F ] );
			}

			if ( ( _ci_controlId.getControlUsage() & 0xF0 ) == MOUSE_AXIS )
			{
				return ControlValue( ms_stateHistory.i_axis[ _ci_controlId.getControlUsage() & 0x0F ] );
			}
		}

		return ControlValue();
	};


	void IDeviceMouse::poll()
	{
		pollEvents();
		ms_stateHistory = ms_state;
	};


	IDeviceMouse::MouseState IDeviceMouse::getStateStruct()
	{
		return ms_state;
	};


	IDeviceMouse::MouseState IDeviceMouse::getPolledStateStruct()
	{
		return ms_stateHistory;
	};


	bool IDeviceMouse::processRawInput( const RAWINPUT& _ri_input )
	{
		if ( _ri_input.header.dwType == RIM_TYPEMOUSE )
		{
			USHORT us_buttonFlags = _ri_input.data.mouse.usButtonFlags;

			if ( us_buttonFlags )
			{
				if ( us_buttonFlags & RI_MOUSE_WHEEL )
				{
					int tmp = static_cast < signed short > ( _ri_input.data.mouse.usButtonData );
					ms_state.i_wheel += tmp;
					pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_WHEEL ), ControlValue( tmp ) );
				}

				if ( us_buttonFlags & ~RI_MOUSE_WHEEL )
				{
					if ( us_buttonFlags & RI_MOUSE_BUTTON_1_DOWN )
					{
						ms_state.b_button[ MOUSE_BUTTON_1 & ~MOUSE_BUTTON ] = false;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_1 ), ControlValue( false ) );
					}
					else if ( us_buttonFlags & RI_MOUSE_BUTTON_1_UP )
					{
						ms_state.b_button[ MOUSE_BUTTON_1 & ~MOUSE_BUTTON ] = true;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_1 ), ControlValue( true ) );
					}

					if ( us_buttonFlags & RI_MOUSE_BUTTON_2_DOWN )
					{
						ms_state.b_button[ MOUSE_BUTTON_2 & ~MOUSE_BUTTON ] = false;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_2 ), ControlValue( false ) );
					}
					else if ( us_buttonFlags & RI_MOUSE_BUTTON_2_UP )
					{
						ms_state.b_button[ MOUSE_BUTTON_2 & ~MOUSE_BUTTON ] = true;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_2 ), ControlValue( true ) );
					}

					if ( us_buttonFlags & RI_MOUSE_BUTTON_3_DOWN )
					{
						ms_state.b_button[ MOUSE_BUTTON_3 & ~MOUSE_BUTTON ] = false;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_3 ), ControlValue( false ) );
					}
					else if ( us_buttonFlags & RI_MOUSE_BUTTON_3_UP )
					{
						ms_state.b_button[ MOUSE_BUTTON_3 & ~MOUSE_BUTTON ] = true;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_3 ), ControlValue( true ) );
					}

					if ( us_buttonFlags & RI_MOUSE_BUTTON_4_DOWN )
					{
						ms_state.b_button[ MOUSE_BUTTON_4 & ~MOUSE_BUTTON ] = false;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_4 ), ControlValue( false ) );
					}
					else if ( us_buttonFlags & RI_MOUSE_BUTTON_4_UP )
					{
						ms_state.b_button[ MOUSE_BUTTON_4 & ~MOUSE_BUTTON ] = true;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_4 ), ControlValue( true ) );
					}

					if ( us_buttonFlags & RI_MOUSE_BUTTON_5_DOWN )
					{
						ms_state.b_button[ MOUSE_BUTTON_5 & ~MOUSE_BUTTON ] = false;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_5 ), ControlValue( false ) );
					}
					else if ( us_buttonFlags & RI_MOUSE_BUTTON_5_UP )
					{
						ms_state.b_button[ MOUSE_BUTTON_5 & ~MOUSE_BUTTON ] = true;
						pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_BUTTON_5 ), ControlValue( true ) );
					}
				}
			}


			if ( _ri_input.data.mouse.lLastX )
			{
				pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_AXIS_X ), ControlValue( _ri_input.data.mouse.lLastX ) );
				ms_state.i_axisX += _ri_input.data.mouse.lLastX;
			}

			if ( _ri_input.data.mouse.lLastY )
			{
				pushEvent( ControlID( getLocalDeviceID(), DEVICE_MOUSE, MOUSE_AXIS_Y ), ControlValue( _ri_input.data.mouse.lLastY ) );
				ms_state.i_axisY += _ri_input.data.mouse.lLastY;
			}

			return true;
		}

		return false;
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                 IDeviceKeyboard::KeyboardState                 ////////
	////////////////////////////////////////////////////////////////////////////////

	IDeviceKeyboard::KeyboardState::KeyboardState()
	{
		ZeroMemory( b_key, sizeof( bool ) * 255 );
	};


	IDeviceKeyboard::KeyboardState::KeyboardState( const KeyboardState& _rhs )
	{
		memcpy( b_key, _rhs.b_key, sizeof( bool ) * 255 );
	};


	IDeviceKeyboard::KeyboardState& IDeviceKeyboard::KeyboardState::operator= ( const KeyboardState& _rhs )
	{
		memcpy( b_key, _rhs.b_key, sizeof( bool ) * 255 );
		return *this;
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                         IDeviceKeyboard                        ////////
	////////////////////////////////////////////////////////////////////////////////

	IDeviceKeyboard::IDeviceKeyboard( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle ) : 
			RawInputDevice( _str_name, _bt_localDeviceID, _handle_deviceHandle, _hwnd_targetWindowHandle, HID_KEYBOARD_USAGE_PAGE, HID_KEYBOARD_USAGE )
	{};


	IDeviceKeyboard::~IDeviceKeyboard()
	{};


	ControlValue IDeviceKeyboard::getState( ControlID _ci_controlId )
	{
		if ( _ci_controlId.getControlUsagePage() == DEVICE_KEYBOARD )
		{
			return ControlValue( ks_state.b_key[ _ci_controlId.getControlUsage() ] );
		}

		return ControlValue();
	};


	ControlValue IDeviceKeyboard::getPolledState( ControlID _ci_controlId )
	{
		if ( _ci_controlId.getControlUsagePage() == DEVICE_KEYBOARD )
		{
			return ControlValue( ks_stateHistory.b_key[ _ci_controlId.getControlUsage() ] );
		}

		return ControlValue();
	};


	void IDeviceKeyboard::poll()
	{
		pollEvents();
		ks_stateHistory = ks_state;
	};


	IDeviceKeyboard::KeyboardState IDeviceKeyboard::getStateStruct()
	{
		return ks_state;
	};


	IDeviceKeyboard::KeyboardState IDeviceKeyboard::getPolledStateStruct()
	{
		return ks_stateHistory;
	};


	bool IDeviceKeyboard::processRawInput( const RAWINPUT& _ri_input )
	{
		if ( _ri_input.header.dwType == RIM_TYPEKEYBOARD )
		{
			//bool b_keyState = _ri_input.keyboard.MakeCode & RI_KEY_BREAK ? KEY_UP : KEY_DOWN;
			bool b_keyState = !( _ri_input.data.keyboard.MakeCode & RI_KEY_BREAK );

			if ( ks_state.b_key[ _ri_input.data.keyboard.VKey ] != b_keyState )
			{
				pushEvent( ControlID( getLocalDeviceID(), DEVICE_KEYBOARD, (BYTE) _ri_input.data.keyboard.VKey ), ControlValue( b_keyState ) );
				ks_state.b_key[ _ri_input.data.keyboard.VKey ] = b_keyState;
			}

			return true;
		}

		return false;
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                     HIDInputDevice::HIDIO                      ////////
	////////////////////////////////////////////////////////////////////////////////

	HIDInputDevice::HIDIO::HIDIO( HIDIOType _hiot_inputType, unsigned short _us_dataIndex, unsigned short _us_usagePage, unsigned short _us_usage, BYTE _bt_deviceId ) : 
			hiot_inputType( _hiot_inputType ), 
			us_dataIndex( _us_dataIndex ), 
			us_usagePage( _us_usagePage ), 
			us_usage( _us_usage ), 
			ci_controlID( _bt_deviceId, (BYTE) _us_usagePage, _us_usage )
	{};


	HIDInputDevice::HIDIO::HIDIO( const HIDIO& _rhs ) : 
			hiot_inputType( _rhs.hiot_inputType ), 
			us_usagePage( _rhs.us_usagePage ), 
			us_usage( _rhs.us_usage ), 
			us_dataIndex( _rhs.us_dataIndex ), 
			ci_controlID( _rhs.ci_controlID )
	{};


	HIDInputDevice::HIDIO::HIDIO( const HIDIO&& _rhs ) : 
			hiot_inputType( _rhs.hiot_inputType ), 
			us_usagePage( _rhs.us_usagePage ), 
			us_usage( _rhs.us_usage ), 
			us_dataIndex( _rhs.us_dataIndex ), 
			ci_controlID( _rhs.ci_controlID )
	{};



	////////////////////////////////////////////////////////////////////////////////
	////////                  HIDInputDevice::HIDIOButton                   ////////
	////////////////////////////////////////////////////////////////////////////////

	HIDInputDevice::HIDIOButton::HIDIOButton( unsigned short _us_dataIndex, unsigned short _us_usage, BYTE _bt_deviceId ) : 
			HIDIO( HIDIOType::button, _us_dataIndex, HID_INPUT_BTN_USAGE_PAGE, _us_usage, _bt_deviceId ), 
			b_value( false )
	{};


	HIDInputDevice::HIDIOButton::HIDIOButton( unsigned short _us_dataIndex, unsigned short _us_usagePage, unsigned short _us_usage, BYTE _bt_deviceId ) : 
			HIDIO( HIDIOType::button, _us_dataIndex, _us_usagePage, _us_usage, _bt_deviceId ), 
			b_value( false )
	{};

			
	HIDInputDevice::HIDIOButton::HIDIOButton( const HIDIOButton& _rhs ) : 
			HIDIO( _rhs ), b_value( false )
	{};


	bool HIDInputDevice::HIDIOButton::set( bool _b_value )
	{
		if ( _b_value != b_value )
		{
			b_value = _b_value;
			return true;
		}
		return false;
	};


	bool HIDInputDevice::HIDIOButton::set( BOOLEAN _B_value )
	{
		return set( _B_value == TRUE );
	};


	void HIDInputDevice::HIDIOButton::operator = ( bool _b_value )
	{
		set( _b_value );
	};


	void HIDInputDevice::HIDIOButton::cloneValue( HIDIO* _hiob_value )
	{
		b_value = static_cast < HIDIOButton* > ( _hiob_value )->get();
	};


	HIDInputDevice::HIDIO* HIDInputDevice::HIDIOButton::clone()
	{
		return new HIDIOButton( *this);
	};


	ControlValue HIDInputDevice::HIDIOButton::getValue()
	{
		return ControlValue( b_value );
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                   HIDInputDevice::HIDIOValue                   ////////
	////////////////////////////////////////////////////////////////////////////////

	HIDInputDevice::HIDIOValue::HIDIOValue( unsigned short _us_dataIndex, unsigned short _us_usagePage, unsigned short _us_usage, int _i_min, int _i_max, bool _b_hasNull, BYTE _bt_deviceId ) : 
			HIDIO( HIDIOType::value, _us_dataIndex, _us_usagePage, _us_usage, _bt_deviceId ), 
			i_min( _i_min ), 
			i_max( _i_max ), 
			b_hasNull( _b_hasNull ), 
			ui_value( _i_max - _i_min + 1 ), 
			b_isNull( b_hasNull ? true : false )
	{};


	HIDInputDevice::HIDIOValue::HIDIOValue( const HIDIOValue& _rhs ) : 
			HIDIO( _rhs ), 
			i_min( _rhs.i_min ), 
			i_max( _rhs.i_max ), 
			b_hasNull( _rhs.b_hasNull ), 
			ui_value( _rhs.ui_value ), 
			b_isNull( _rhs.b_isNull )
	{};


	HIDInputDevice::HIDIOValue::HIDIOValue( const HIDIOValue&& _rhs ) : 
			HIDIO( _rhs ), 
			i_min( _rhs.i_min ), 
			i_max( _rhs.i_max ), 
			b_hasNull( _rhs.b_hasNull ), 
			ui_value( _rhs.ui_value ), 
			b_isNull( _rhs.b_isNull )
	{};


	bool HIDInputDevice::HIDIOValue::set( unsigned int _ui_value )
	{
		if ( _ui_value != ui_value )
		{
			bool b_retFlag(true);
			ui_value = _ui_value;

			if ( b_hasNull )
			{
				if ( ui_value < i_min || ui_value > i_max )
				{
					if ( b_isNull ) // value is still null, no change to report
						b_retFlag = false;

					b_isNull = true;
				}
				else
					b_isNull = false;
			}

			set_impl();
			return b_retFlag;
		}
		return false;
	};


	HIDInputDevice::HIDIOValue& HIDInputDevice::HIDIOValue::operator = ( unsigned int _rhs )
	{
		set( _rhs );
		return *this;
	};


	void HIDInputDevice::HIDIOValue::cloneValue( HIDIO* _hiob_value )
	{
		HIDIOValue* hiov_tmp = static_cast < HIDIOValue* > ( _hiob_value );
		ui_value = hiov_tmp->getRawValue();
		b_isNull = hiov_tmp->isNull();
	};


	HIDInputDevice::HIDIO* HIDInputDevice::HIDIOValue::clone()
	{
		return new HIDIOValue( *this);
	};


	ControlValue HIDInputDevice::HIDIOValue::getValue()
	{
		if ( b_isNull ) 
			return ControlValue( 0.0f );
		return ControlValue( b_isNull ? 0.0f : ( ( (float) ( ui_value - i_min ) ) / i_max ) );
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                HIDInputDevice::HIDIODeviceState                ////////
	////////////////////////////////////////////////////////////////////////////////

	HIDInputDevice::HIDIODeviceState::~HIDIODeviceState()
	{
		clear();
	};


	HIDInputDevice::HIDIODeviceState::HIDIODeviceState()
	{};


	HIDInputDevice::HIDIODeviceState::HIDIODeviceState( const HIDIODeviceState& _rhs )
	{
		setUp( _rhs.vec_inputs, _rhs.vec_outputs );
	};


	HIDInputDevice::HIDIODeviceState::HIDIODeviceState( HIDIODeviceState&& _rhs )
	{
		vec_inputs.swap( _rhs.vec_inputs );
		vec_outputs.swap( _rhs.vec_outputs );
	};


	void HIDInputDevice::HIDIODeviceState::setUp( const std::vector< HIDIO* >& _vec_inputs, const std::vector< HIDIO* >& _vec_outputs )
	{
		clear();

		vec_inputs.resize( _vec_inputs.size() );
		for ( unsigned int i = 0; i < _vec_inputs.size(); ++i )
		{
			vec_inputs[ i ] = _vec_inputs[ i ]->clone();
			vec_inputs[ i ]->cloneValue( _vec_inputs[ i ] );
		}

		vec_outputs.resize( _vec_outputs.size() );
		for ( unsigned int i = 0; i < _vec_outputs.size(); ++i )
		{
			vec_outputs[ i ] = _vec_outputs[ i ]->clone();
			vec_outputs[ i ]->cloneValue( _vec_outputs[ i ] );
		}
	};


	void HIDInputDevice::HIDIODeviceState::clear()
	{
		for ( unsigned int i = 0; i < vec_inputs.size(); ++i )
		{
			if ( vec_inputs[ i ] )
				delete vec_inputs[ i ];
		}

		for ( unsigned int i = 0; i < vec_outputs.size(); ++i )
		{
			if ( vec_outputs[ i ] )
				delete vec_outputs[ i ];
		}
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                         HIDInputDevice                         ////////
	////////////////////////////////////////////////////////////////////////////////


	void HIDInputDevice::remapInput( int _i_inputIndex, HIDIO* _hio_input )
	{
		ControlID cid_id = vec_indexedInputs[ _i_inputIndex ]->getID();

		bool b_mappedCorrect( false );
		{
			flat_map::iterator it_mi = map_mappedInputs.find( _hio_input->getID() );
			if ( it_mi != map_mappedInputs.end() ) map_mappedInputs.erase( it_mi );

			it_mi = map_mappedInputs.find( cid_id );
			if ( it_mi != map_mappedInputs.end() && it_mi->second == vec_indexedInputs[ _i_inputIndex ] )
			{
				b_mappedCorrect = true;
				map_mappedInputs.erase( it_mi );
			}

			delete vec_indexedInputs[ _i_inputIndex ];

			vec_indexedInputs[ _i_inputIndex ] = _hio_input->clone();
			map_mappedInputs.insert( flat_map::value_type( vec_indexedInputs[ _i_inputIndex ]->getID(), vec_indexedInputs[ _i_inputIndex ] ) );
		}

		{
			flat_map::iterator it_mi = map_mappedInputsHistory.find( _hio_input->getID() );
			if ( it_mi != map_mappedInputsHistory.end() ) map_mappedInputsHistory.erase( it_mi );

			it_mi = map_mappedInputsHistory.find( cid_id );
			if ( it_mi != map_mappedInputsHistory.end() && b_mappedCorrect )
			{
				map_mappedInputsHistory.erase( it_mi );
			}

			delete vec_indexedInputsHistory[ _i_inputIndex ];

			vec_indexedInputsHistory[ _i_inputIndex ] = _hio_input->clone();
			map_mappedInputsHistory.insert( flat_map::value_type( vec_indexedInputsHistory[ _i_inputIndex ]->getID(), vec_indexedInputsHistory[ _i_inputIndex ] ) );
		}
	};


	bool HIDInputDevice::addVirtualInput( HIDIO* _hio_input )
	{
		flat_map::iterator it_mi = map_mappedInputs.find( _hio_input->getID() );
		if ( it_mi != map_mappedInputs.end() )
			return false;

		HIDIO* hio_input = _hio_input->clone();
		ControlID cid_id = hio_input->getID();
		map_mappedInputs.insert( flat_map::value_type( cid_id, hio_input ) );
		map_mappedInputsHistory.insert( flat_map::value_type( cid_id, hio_input->clone() ) );

		return true;
	};


	HIDInputDevice::HIDInputDevice( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle, unsigned short _us_usagePage, unsigned short _us_usage ) : 
			RawInputDevice( _str_name, _bt_localDeviceID, _handle_deviceHandle, _hwnd_targetWindowHandle, _us_usagePage, _us_usage ), 
			dw_vendorId( 0 ), 
			dw_productId( 0 ), 
			dw_versionNumber( 0 ), 
			pbt_preparsedDataPointer( NULL ), 
			ul_dataListLength( 0 ), 
			hd_dataListPointer( NULL )
	{};


	HIDInputDevice::~HIDInputDevice()
	{
		unregisterDevice();
	};


	std::string HIDInputDevice::getDeviceSystemString()
	{
		return str_deviceSystemString;
	};


	std::string HIDInputDevice::getProductString()
	{
		return str_productString;
	};


	std::string HIDInputDevice::getPhysicalDescriptor()
	{
		return str_physicalDescriptor;
	};


	std::string HIDInputDevice::getSerialNumberString()
	{
		return str_serialNumberString;
	};


	std::string HIDInputDevice::getManufacturerString()
	{
		return str_manufacturerString;
	};


	DWORD HIDInputDevice::registerDevice( DWORD _dw_additionalFlags )
	{
		if ( DWORD out = RawInputDevice::registerDevice( _dw_additionalFlags ) )
		{
			RawInputDevice::unregisterDevice();
			return out;
		}

		// get device system name
		UINT ui_bufferSize = 0;
		if ( GetRawInputDeviceInfoA( getDeviceHandle(), RIDI_DEVICENAME, NULL, &ui_bufferSize ) < 0 )
		{
			DWORD dw_error = GetLastError();
			unregisterDevice();
			char c_errMsg[ 1024 ];
			FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 1024, NULL );
			THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			return dw_error;
		}

		str_deviceSystemString.resize( ui_bufferSize );
		if ( GetRawInputDeviceInfoA( getDeviceHandle(), RIDI_DEVICENAME, &( str_deviceSystemString[0] ), &ui_bufferSize ) < 0 )
		{
			DWORD dw_error = GetLastError();
			unregisterDevice();
			char c_errMsg[ 1024 ];
			FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 1024, NULL );
			THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			return dw_error;
		}


		{
			char c_tmp[ ( 126 * sizeof(wchar_t) ) ];

			if ( HidD_GetProductString( getDeviceHandle(), c_tmp, ( 126 * sizeof(wchar_t) ) ) != TRUE )
			{
				unregisterDevice();
				THROW_SYSTEM_ERROR( std::string( "Function HidD_GetProductString returned error" ) );
			}

			str_productString = c_tmp;


			if ( HidD_GetPhysicalDescriptor( getDeviceHandle(), c_tmp, ( 126 * sizeof(wchar_t) ) ) != TRUE )
			{
				unregisterDevice();
				THROW_SYSTEM_ERROR( std::string( "Function HidD_GetPhysicalDescriptor returned error" ) );
			}

			str_physicalDescriptor = c_tmp;


			if ( HidD_GetSerialNumberString( getDeviceHandle(), c_tmp, ( 126 * sizeof(wchar_t) ) ) != TRUE )
			{
				unregisterDevice();
				THROW_SYSTEM_ERROR( std::string( "Function HidD_GetSerialNumberString returned error" ) );
			}

			str_serialNumberString = c_tmp;


			if ( HidD_GetManufacturerString( getDeviceHandle(), c_tmp, ( 126 * sizeof(wchar_t) ) ) != TRUE )
			{
				unregisterDevice();
				THROW_SYSTEM_ERROR( std::string( "Function HidD_GetManufacturerString returned error" ) );
			}

			str_manufacturerString = c_tmp;
		}


		// get additional device information
		RID_DEVICE_INFO rdi_deviceInfo;
		ZeroMemory( &rdi_deviceInfo, sizeof( RID_DEVICE_INFO ) );
		ui_bufferSize = sizeof( RID_DEVICE_INFO );

		if ( GetRawInputDeviceInfoA( getDeviceHandle(), RIDI_DEVICEINFO, &rdi_deviceInfo, &ui_bufferSize ) < 0 )
		{
			DWORD dw_error = GetLastError();
			unregisterDevice();
			char c_errMsg[ 1024 ];
			FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 1024, NULL );
			THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			return dw_error;
		}


		if ( rdi_deviceInfo.dwType != RIM_TYPEHID )
		{
			unregisterDevice();
			if ( rdi_deviceInfo.dwType == RIM_TYPEMOUSE )
				THROW_ILLEGAL_TYPE_CONV( "Cannot convert HIDInputDevice object into IDeviceMouse" );
			else //RIM_TYPEKEYBOARD
				THROW_ILLEGAL_TYPE_CONV( "Cannot convert HIDInputDevice object into IDeviceKeyboard" );
		}

		dw_vendorId = rdi_deviceInfo.hid.dwVendorId;
		dw_productId = rdi_deviceInfo.hid.dwProductId;
		dw_versionNumber = rdi_deviceInfo.hid.dwVersionNumber;


		ui_bufferSize = 0;
		if ( GetRawInputDeviceInfoA( getDeviceHandle(), RIDI_DEVICEINFO, NULL, &ui_bufferSize ) < 0 )
		{
			DWORD dw_error = GetLastError();
			unregisterDevice();
			char c_errMsg[ 1024 ];
			FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 1024, NULL );
			THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			return dw_error;
		}

		v_preparsedData.resize( ui_bufferSize );
		pbt_preparsedDataPointer = &( v_preparsedData[0] );

		if ( GetRawInputDeviceInfoA( getDeviceHandle(), RIDI_PREPARSEDDATA, &pbt_preparsedDataPointer, &ui_bufferSize ) < 0 )
		{
			DWORD dw_error = GetLastError();
			unregisterDevice();
			char c_errMsg[ 1024 ];
			FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw_error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&c_errMsg, 1024, NULL );
			THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			return dw_error;
		}

		NTSTATUS nts_out = HIDP_STATUS_SUCCESS;

		// get device caps
		nts_out = HidP_GetCaps( reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ), &hc_caps );
		if ( nts_out != HIDP_STATUS_SUCCESS )
		{
			char c_errMsg[ 1024 ];
			sprintf( c_errMsg, "Function HidP_GetCaps returned NTSTATUS error code %04x.", nts_out );
			THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
		}


		// get all input buttons
		if ( hc_caps.NumberInputButtonCaps )
		{
			std::vector<HIDP_BUTTON_CAPS> v_buttonCaps;
			v_buttonCaps.resize( hc_caps.NumberInputButtonCaps );

			USHORT us_buttonCapsLength = hc_caps.NumberInputButtonCaps;
			nts_out = HidP_GetButtonCaps( HidP_Input, &( v_buttonCaps[0] ), &us_buttonCapsLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ) );
			if ( nts_out != HIDP_STATUS_SUCCESS )
			{
				char c_errMsg[ 1024 ];
				sprintf( c_errMsg, "Function HidP_GetButtonCaps returned NTSTATUS error code %04x.", nts_out );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			}

			for ( unsigned int i = 0; i < v_buttonCaps.size(); ++i )
			{
				if ( v_buttonCaps[i].IsRange )
				{
					if ( vec_indexedInputs.size() <= v_buttonCaps[i].Range.DataIndexMax )
						vec_indexedInputs.resize( v_buttonCaps[i].Range.DataIndexMax + 1 );

					for ( int j = 0; j <= ( v_buttonCaps[i].Range.DataIndexMax - v_buttonCaps[i].Range.DataIndexMin ); ++j )
					{
						unsigned short us_dataIndex = v_buttonCaps[i].Range.DataIndexMin + j;

						vec_indexedInputs[ us_dataIndex ] = new HIDIOButton( us_dataIndex, v_buttonCaps[i].UsagePage, v_buttonCaps[i].Range.UsageMin + j, getLocalDeviceID() );
						map_mappedInputs.insert( std::make_pair( vec_indexedInputs[ us_dataIndex ]->getID(), vec_indexedInputs[ us_dataIndex ] ) );

						vec_indexedInputsHistory[ us_dataIndex ] = new HIDIOButton( *( static_cast <HIDIOButton*> ( vec_indexedInputs[ us_dataIndex ] ) ) );
						map_mappedInputsHistory.insert( std::make_pair( vec_indexedInputsHistory[ us_dataIndex ]->getID(), vec_indexedInputsHistory[ us_dataIndex ] ) );
					}
				}
				else
				{
					if ( vec_indexedInputs.size() <= v_buttonCaps[i].NotRange.DataIndex )
						vec_indexedInputs.resize( v_buttonCaps[i].NotRange.DataIndex + 1 );

					unsigned short us_dataIndex = v_buttonCaps[i].NotRange.DataIndex;

					vec_indexedInputs[ us_dataIndex ] = new HIDIOButton( us_dataIndex, v_buttonCaps[i].UsagePage, v_buttonCaps[i].NotRange.Usage, getLocalDeviceID() );
					map_mappedInputs.insert( std::make_pair( vec_indexedInputs[ us_dataIndex ]->getID(), vec_indexedInputs[ us_dataIndex ] ) );

					vec_indexedInputsHistory[ us_dataIndex ] = new HIDIOButton( *( static_cast <HIDIOButton*> ( vec_indexedInputs[ us_dataIndex ] ) ) );
					map_mappedInputsHistory.insert( std::make_pair( vec_indexedInputsHistory[ us_dataIndex ]->getID(), vec_indexedInputsHistory[ us_dataIndex ] ) );
				}
			}
		}


		// get all input values
		if ( hc_caps.NumberInputValueCaps )
		{
			std::vector<HIDP_VALUE_CAPS> v_valueCaps;
			v_valueCaps.resize( hc_caps.NumberInputValueCaps );

			USHORT us_valueCapsLength = hc_caps.NumberInputValueCaps;
			nts_out = HidP_GetValueCaps( HidP_Input, &( v_valueCaps[0] ), &us_valueCapsLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ) );
			if ( nts_out != HIDP_STATUS_SUCCESS )
			{
				char c_errMsg[ 1024 ];
				sprintf( c_errMsg, "Function HidP_GetValueCaps returned NTSTATUS error code %04x.", nts_out );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			}


			for ( int i = 0; i < v_valueCaps.size(); ++i )
			{
				if ( v_valueCaps[i].IsRange )
				{
					if ( vec_indexedInputs.size() <= v_valueCaps[i].Range.DataIndexMax )
						vec_indexedInputs.resize( v_valueCaps[i].Range.DataIndexMax + 1 );

					for ( int j = 0; j <= ( v_valueCaps[i].Range.DataIndexMax - v_valueCaps[i].Range.DataIndexMin ); ++j )
					{
						unsigned short us_dataIndex = v_valueCaps[i].Range.DataIndexMin + j;

						vec_indexedInputs[ us_dataIndex ] = new HIDIOValue( us_dataIndex, v_valueCaps[i].UsagePage, v_valueCaps[i].Range.UsageMin + j, v_valueCaps[i].LogicalMin, v_valueCaps[i].LogicalMax, v_valueCaps[i].HasNull == TRUE, getLocalDeviceID() );
						map_mappedInputs.insert( std::make_pair( vec_indexedInputs[ us_dataIndex ]->getID(), vec_indexedInputs[ us_dataIndex ] ) );

						vec_indexedInputsHistory[ us_dataIndex ] = new HIDIOValue( *( static_cast <HIDIOValue*> ( vec_indexedInputs[ us_dataIndex ] ) ) );
						map_mappedInputsHistory.insert( std::make_pair( vec_indexedInputsHistory[ us_dataIndex ]->getID(), vec_indexedInputsHistory[ us_dataIndex ] ) );
					}
				}
				else
				{
					if ( vec_indexedInputs.size() <= v_valueCaps[i].NotRange.DataIndex )
						vec_indexedInputs.resize( v_valueCaps[i].NotRange.DataIndex + 1 );

					unsigned short us_dataIndex = v_valueCaps[i].NotRange.DataIndex;

					vec_indexedInputs[ us_dataIndex ] = new HIDIOValue( us_dataIndex, v_valueCaps[i].UsagePage, v_valueCaps[i].NotRange.Usage, v_valueCaps[i].LogicalMin, v_valueCaps[i].LogicalMax, v_valueCaps[i].HasNull == TRUE, getLocalDeviceID() );
					map_mappedInputs.insert( std::make_pair( vec_indexedInputs[ us_dataIndex ]->getID(), vec_indexedInputs[ us_dataIndex ] ) );

					vec_indexedInputsHistory[ us_dataIndex ] = new HIDIOValue( *( static_cast <HIDIOValue*> ( vec_indexedInputs[ us_dataIndex ] ) ) );
					map_mappedInputsHistory.insert( std::make_pair( vec_indexedInputsHistory[ us_dataIndex ]->getID(), vec_indexedInputsHistory[ us_dataIndex ] ) );
				}
			}
		}



		// get all output buttons
		if ( hc_caps.NumberOutputButtonCaps )
		{
			std::vector<HIDP_BUTTON_CAPS> v_buttonCaps;
			v_buttonCaps.resize( hc_caps.NumberOutputButtonCaps );

			USHORT us_buttonCapsLength = hc_caps.NumberOutputButtonCaps;
			nts_out = HidP_GetButtonCaps( HidP_Output, &( v_buttonCaps[0] ), &us_buttonCapsLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ) );
			if ( nts_out != HIDP_STATUS_SUCCESS )
			{
				char c_errMsg[ 1024 ];
				sprintf( c_errMsg, "Function HidP_GetButtonCaps returned NTSTATUS error code %04x.", nts_out );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			}


			for ( int i = 0; i < v_buttonCaps.size(); ++i )
			{
				if ( v_buttonCaps[i].IsRange )
				{
					if ( vec_indexedOutputs.size() <= v_buttonCaps[i].Range.DataIndexMax )
						vec_indexedOutputs.resize( v_buttonCaps[i].Range.DataIndexMax + 1 );

					for ( int j = 0; j <= ( v_buttonCaps[i].Range.DataIndexMax - v_buttonCaps[i].Range.DataIndexMin ); ++j )
					{
						unsigned short us_dataIndex = v_buttonCaps[i].Range.DataIndexMin + j;

						vec_indexedOutputs[ us_dataIndex ] = new HIDIOButton( us_dataIndex, v_buttonCaps[i].UsagePage, v_buttonCaps[i].Range.UsageMin + j, getLocalDeviceID() );
						map_mappedOutputs.insert( std::make_pair( vec_indexedOutputs[ us_dataIndex ]->getID(), vec_indexedOutputs[ us_dataIndex ] ) );
					}
				}
				else
				{
					if ( vec_indexedOutputs.size() <= v_buttonCaps[i].NotRange.DataIndex )
						vec_indexedOutputs.resize( v_buttonCaps[i].NotRange.DataIndex + 1 );

					unsigned short us_dataIndex = v_buttonCaps[i].NotRange.DataIndex;

					vec_indexedOutputs[ us_dataIndex ] = new HIDIOButton( us_dataIndex, v_buttonCaps[i].UsagePage, v_buttonCaps[i].NotRange.Usage, getLocalDeviceID() );
					map_mappedOutputs.insert( std::make_pair( vec_indexedOutputs[ us_dataIndex ]->getID(), vec_indexedOutputs[ us_dataIndex ] ) );
				}
			}
		}


		// get all output values
		if ( hc_caps.NumberOutputValueCaps )
		{
			std::vector<HIDP_VALUE_CAPS> v_valueCaps;
			v_valueCaps.resize( hc_caps.NumberOutputValueCaps );

			USHORT us_valueCapsLength = hc_caps.NumberInputValueCaps;
			nts_out = HidP_GetValueCaps( HidP_Output, &( v_valueCaps[0] ), &us_valueCapsLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ) );
			if ( nts_out != HIDP_STATUS_SUCCESS )
			{
				char c_errMsg[ 1024 ];
				sprintf( c_errMsg, "Function HidP_GetValueCaps returned NTSTATUS error code %04x.", nts_out );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			}


			for ( int i = 0; i < v_valueCaps.size(); ++i )
			{
				if ( v_valueCaps[i].IsRange )
				{
					if ( vec_indexedOutputs.size() <= v_valueCaps[i].Range.DataIndexMax )
						vec_indexedOutputs.resize( v_valueCaps[i].Range.DataIndexMax + 1 );

					for ( int j = 0; j <= ( v_valueCaps[i].Range.DataIndexMax - v_valueCaps[i].Range.DataIndexMin ); ++j )
					{
						unsigned short us_dataIndex = v_valueCaps[i].Range.DataIndexMin + j;

						vec_indexedOutputs[ us_dataIndex ] = new HIDIOValue( us_dataIndex, v_valueCaps[i].UsagePage, v_valueCaps[i].Range.UsageMin + j, v_valueCaps[i].LogicalMin, v_valueCaps[i].LogicalMax, v_valueCaps[i].HasNull == TRUE, getLocalDeviceID() );
						map_mappedOutputs.insert( std::make_pair( vec_indexedOutputs[ us_dataIndex ]->getID(), vec_indexedOutputs[ us_dataIndex ] ) );
					}
				}
				else
				{
					if ( vec_indexedOutputs.size() <= v_valueCaps[i].NotRange.DataIndex )
						vec_indexedOutputs.resize( v_valueCaps[i].NotRange.DataIndex + 1 );

					unsigned short us_dataIndex = v_valueCaps[i].NotRange.DataIndex;

					vec_indexedOutputs[ us_dataIndex ] = new HIDIOValue( us_dataIndex, v_valueCaps[i].UsagePage, v_valueCaps[i].NotRange.Usage, v_valueCaps[i].LogicalMin, v_valueCaps[i].LogicalMax, v_valueCaps[i].HasNull == TRUE, getLocalDeviceID() );
					map_mappedOutputs.insert( std::make_pair( vec_indexedOutputs[ us_dataIndex ]->getID(), vec_indexedOutputs[ us_dataIndex ] ) );
				}
			}
		}


		{
			ULONG ul_maxDataListLength = HidP_MaxDataListLength( HidP_Input, reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ) );
			vec_dataList.resize( ul_maxDataListLength );
			hd_dataListPointer = &( vec_dataList[ 0 ] );
		}

		return 0;
	};


	DWORD HIDInputDevice::unregisterDevice()
	{
		if ( pbt_preparsedDataPointer )
			HidD_FreePreparsedData( reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ) );

		str_deviceSystemString.clear();
		str_productString.clear();
		str_physicalDescriptor.clear();
		str_serialNumberString.clear();
		str_manufacturerString.clear();

		dw_vendorId = 0;
		dw_productId = 0;
		dw_versionNumber = 0;
		v_preparsedData.clear();
		pbt_preparsedDataPointer = NULL;


		ZeroMemory( &hc_caps, sizeof( HIDP_CAPS ) );

		while ( vec_indexedInputs.size() )
		{
			if ( *vec_indexedInputs.rbegin() )
				delete ( *vec_indexedInputs.rbegin() );
			vec_indexedInputs.pop_back();
		}

		while ( vec_indexedInputsHistory.size() )
		{
			if ( *vec_indexedInputsHistory.rbegin() )
				delete ( *vec_indexedInputsHistory.rbegin() );
			vec_indexedInputsHistory.pop_back();
		}

		while ( vec_indexedOutputs.size() )
		{
			if ( *vec_indexedOutputs.rbegin() )
				delete ( *vec_indexedOutputs.rbegin() );
			vec_indexedOutputs.pop_back();
		}


		map_mappedInputs.clear();
		map_mappedInputsHistory.clear();
		map_mappedOutputs.clear();


		vec_dataList.clear();
		hd_dataListPointer = NULL;


		return RawInputDevice::unregisterDevice();
	};


	ControlValue HIDInputDevice::getState( ControlID _ci_controlId )
	{
		boost::container::flat_map< ControlID, HIDIO* >::iterator it = map_mappedInputs.find( _ci_controlId );
		if ( it != map_mappedInputs.end() )
			return it->second->getValue();
		return ControlValue();
	};


	ControlValue HIDInputDevice::getPolledState( ControlID _ci_controlId )
	{
		boost::container::flat_map< ControlID, HIDIO* >::iterator it = map_mappedInputsHistory.find( _ci_controlId );
		if ( it != map_mappedInputsHistory.end() )
			return it->second->getValue();
		return ControlValue();
	};


	bool HIDInputDevice::setState( ControlID _ci_controlId, ControlValue _cv_state )
	{
		boost::container::flat_map< ControlID, HIDIO* >::iterator it = map_mappedOutputs.find( _ci_controlId );
		if ( it != map_mappedOutputs.end() )
		{
			HIDP_DATA hd_controlState;
			hd_controlState.DataIndex = it->second->getDataIndex();
			if ( it->second->getInputType() == HIDIOType::button )
				hd_controlState.On = (BOOLEAN) _cv_state.b_value;
			else
				hd_controlState.RawValue = _cv_state.ul_rawValue;

			ULONG ul_dataListLength = 1;
			std::vector<CHAR> vec_report( hc_caps.OutputReportByteLength );

			NTSTATUS nts_out = HIDP_STATUS_SUCCESS;
			nts_out = HidP_SetData( HidP_Output, &hd_controlState, &ul_dataListLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ), &( vec_report[ 0 ] ), hc_caps.OutputReportByteLength );
			if ( nts_out != HIDP_STATUS_SUCCESS )
			{
				char c_errMsg[ 1024 ];
				sprintf( c_errMsg, "Function HidP_SetData returned NTSTATUS error code %04x.", nts_out );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			}

			nts_out = HidD_SetOutputReport( getDeviceHandle(), &( vec_report[ 0 ] ), hc_caps.OutputReportByteLength );
			if ( nts_out != HIDP_STATUS_SUCCESS )
			{
				char c_errMsg[ 1024 ];
				sprintf( c_errMsg, "Function HidP_SetData returned NTSTATUS error code %04x.", nts_out );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			}

			return true;
		}
		return false;
	};


	unsigned int HIDInputDevice::getInputControls( ControlID* _ci_controlsArray )
	{
		if ( _ci_controlsArray )
		{
			for ( boost::container::flat_map< ControlID, HIDIO* >::iterator it = map_mappedInputs.begin(); it != map_mappedInputs.end(); ++it )
			{
				*_ci_controlsArray = it->first;
				++_ci_controlsArray;
			}
		}

		return map_mappedInputs.size();
	};


	std::vector<ControlID> HIDInputDevice::getInputControls()
	{
		std::vector<ControlID> vec_tmp( getInputControls( NULL ) );
		getInputControls( &( vec_tmp[ 0 ] ) );
		return vec_tmp;
	};


	unsigned int HIDInputDevice::getOutputControls( ControlID* _ci_controlsArray )
	{
		if ( _ci_controlsArray )
		{
			for ( boost::container::flat_map< ControlID, HIDIO* >::iterator it = map_mappedOutputs.begin(); it != map_mappedOutputs.end(); ++it )
			{
				*_ci_controlsArray = it->first;
				++_ci_controlsArray;
			}
		}

		return map_mappedOutputs.size();
	};


	std::vector<ControlID> HIDInputDevice::getOutputControls()
	{
		std::vector<ControlID> vec_tmp( getOutputControls( NULL ) );
		getOutputControls( &( vec_tmp[ 0 ] ) );
		return vec_tmp;
	};


	void HIDInputDevice::poll()
	{
		pollEvents();

		for ( unsigned int i = 0; i < vec_indexedInputsHistory.size(); ++i )
		{
			vec_indexedInputsHistory[ i ]->cloneValue( vec_indexedInputs[ i ] );
		}
	};


	bool HIDInputDevice::processRawInput( const RAWINPUT& _ri_input )
	{
		if ( _ri_input.header.dwType == RIM_TYPEHID )
		{
			ul_dataListLength = vec_dataList.size();

			//check for error!!!
			NTSTATUS nts_out = HIDP_STATUS_SUCCESS;
			nts_out = HidP_GetData( HidP_Input, hd_dataListPointer, &ul_dataListLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ), const_cast <PCHAR> ( ( const PCHAR ) _ri_input.data.hid.bRawData ), _ri_input.data.hid.dwSizeHid * _ri_input.data.hid.dwCount );
			if ( nts_out != HIDP_STATUS_SUCCESS )
			{
				char c_errMsg[ 1024 ];
				sprintf( c_errMsg, "Function HidP_SetData returned NTSTATUS error code %04x.", nts_out );
				THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
			}

			for ( unsigned int i = 0; i < ul_dataListLength; ++i )
			{
				if ( vec_indexedInputs[ hd_dataListPointer[ i ].DataIndex ]->getInputType() == HIDIOType::button )
				{
					HIDIOButton* hib_tmp = static_cast < HIDIOButton* > ( vec_indexedInputs[ hd_dataListPointer[ i ].DataIndex ] );
					if ( hib_tmp->set( hd_dataListPointer[ i ].On ) )
						pushEvent( hib_tmp->getID(), ControlValue( hib_tmp->get() ) );
				}
				else
				{
					HIDIOValue* hiv_tmp = static_cast < HIDIOValue* > ( vec_indexedInputs[ hd_dataListPointer[ i ].DataIndex ] );
					if ( hiv_tmp->set( hd_dataListPointer[ i ].RawValue ) )
						pushEvent( hiv_tmp->getID(), hiv_tmp->getValue() );
				}
			}

			return true;
		}

		return false;
	};


	void HIDInputDevice::getDataList( const RAWINPUT& _ri_input )
	{
		ul_dataListLength = vec_dataList.size();

		//check for error!!!
		NTSTATUS nts_out = HIDP_STATUS_SUCCESS;
		nts_out = HidP_GetData( HidP_Input, hd_dataListPointer, &ul_dataListLength, reinterpret_cast<PHIDP_PREPARSED_DATA>( pbt_preparsedDataPointer ), const_cast <PCHAR> ( ( const PCHAR ) _ri_input.data.hid.bRawData ), _ri_input.data.hid.dwSizeHid * _ri_input.data.hid.dwCount );
		if ( nts_out != HIDP_STATUS_SUCCESS )
		{
			char c_errMsg[ 1024 ];
			sprintf( c_errMsg, "Function HidP_SetData returned NTSTATUS error code %04x.", nts_out );
			THROW_SYSTEM_ERROR( std::string( c_errMsg ) );
		}
	};


	HIDInputDevice::HIDIODeviceState HIDInputDevice::getStateStruct()
	{
		HIDIODeviceState tmp_out;
		tmp_out.setUp( vec_indexedInputs, vec_indexedOutputs );
		return tmp_out;
	};


	HIDInputDevice::HIDIODeviceState HIDInputDevice::getPolledStateStruct()
	{
		HIDIODeviceState tmp_out;
		tmp_out.setUp( vec_indexedInputsHistory, vec_indexedOutputs );
		return tmp_out;
	};



















	////////////////////////////////////////////////////////////////////////////////
	////////              HIDInputGamepad::HIDIOValueHatSwitch              ////////
	////////////////////////////////////////////////////////////////////////////////


	void HIDInputGamepad::HIDIOValueHatSwitch::set_impl()
	{
		if ( b_isNull )
		{
			hss_step = HatSwitchStep::UNUSED;
		}
		else
		{
			if ( ( i_max - i_min + 1 ) == 8 )
			{
				hss_step = (HatSwitchStep) ( ui_value - i_min );
			}
			else //4
			{
				hss_step = (HatSwitchStep) ( ( ui_value - i_min ) * 2 );
			}
		}
	};


	HIDInputGamepad::HIDIOValueHatSwitch::HIDIOValueHatSwitch( unsigned short _us_dataIndex, unsigned short _us_usagePage, unsigned short _us_usage, int _i_min, int _i_max, bool _b_hasNull, BYTE _bt_deviceId ) : 
			HIDIOValue( _us_dataIndex, _us_usagePage, _us_usage, _i_min, _i_max, _b_hasNull, _bt_deviceId ), 
			hss_step( HatSwitchStep::UNUSED )
	{};


	HIDInputGamepad::HIDIOValueHatSwitch::HIDIOValueHatSwitch( const HIDIOValueHatSwitch& _rhs ) : 
			HIDIOValue( _rhs ), 
			hss_step( _rhs.hss_step )
	{};


	HIDInputGamepad::HIDIOValueHatSwitch::HIDIOValueHatSwitch( const HIDIOValueHatSwitch&& _rhs ) : 
			HIDIOValue( _rhs ), 
			hss_step( _rhs.hss_step )
	{};


	void HIDInputGamepad::HIDIOValueHatSwitch::cloneValue( HIDIO* _hiob_value )
	{
		HIDIOValue::cloneValue( _hiob_value );
		hss_step = static_cast <HIDIOValueHatSwitch*> ( _hiob_value )->getStep();
	};


	HIDInputDevice::HIDIO* HIDInputGamepad::HIDIOValueHatSwitch::clone()
	{
		return new HIDIOValueHatSwitch( *this );
	};


	ControlValue HIDInputGamepad::HIDIOValueHatSwitch::getValue()
	{
		return ControlValue( (int) hss_step );
	};



	////////////////////////////////////////////////////////////////////////////////
	////////              HIDInputGamepad::HIDIOValueStickAxis              ////////
	////////////////////////////////////////////////////////////////////////////////


	void HIDInputGamepad::HIDIOValueStickAxis::set_impl()
	{
		f_value = (float)( ui_value - i_center ) / ( ( ui_value >= i_center ) ? i_max : i_min );
	};


	HIDInputGamepad::HIDIOValueStickAxis::HIDIOValueStickAxis( const HIDIOValueStickAxis& _rhs ) : 
			HIDIOValue( _rhs ), 
			i_center( _rhs.i_center ), 
			f_value( _rhs.f_value )
	{};


	HIDInputGamepad::HIDIOValueStickAxis::HIDIOValueStickAxis( const HIDIOValue&  _rhs ) : 
			HIDIOValue( _rhs ), 
			i_center( ( i_max - i_min + 1 ) / 2 ), 
			f_value( 0.0f )
	{};


	HIDInputGamepad::HIDIOValueStickAxis::HIDIOValueStickAxis( const HIDIOValue&& _rhs ) : 
			HIDIOValue( _rhs ), 
			i_center( ( i_max - i_min + 1 ) / 2 ), 
			f_value( 0.0f )
	{};


	HIDInputGamepad::HIDIOValueStickAxis::HIDIOValueStickAxis( const HIDIOValue& _rhs, ControlID _ci_id ) : 
			HIDIOValue( _rhs.getDataIndex(), _ci_id.bt_controlUsagePage, _ci_id.bt_controlUsage, _rhs.getMin(), _rhs.getMax(), _rhs.getHasNull(), _ci_id.bt_deviceId ), 
			i_center( ( i_max - i_min + 1 ) / 2 ), 
			f_value( 0.0f )
	{};


	void HIDInputGamepad::HIDIOValueStickAxis::cloneValue( HIDIO* _hiob_value )
	{
		HIDIOValueStickAxis* hsa_tmp = static_cast < HIDIOValueStickAxis* > ( _hiob_value );
		ui_value = hsa_tmp->getRawValue();
		f_value = hsa_tmp->getNormalizedAxis();
	};


	HIDInputDevice::HIDIO* HIDInputGamepad::HIDIOValueStickAxis::clone()
	{
		return new HIDIOValueStickAxis( *this );
	};

	ControlValue HIDInputGamepad::HIDIOValueStickAxis::getValue()
	{
		return ControlValue( f_value );
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                 HIDInputGamepad::GamepadState                  ////////
	////////////////////////////////////////////////////////////////////////////////


	HIDInputGamepad::GamepadState::GamepadState() :	
			b_dPad_UP( false ), 
			b_dPad_DOWN( false ), 
			b_dPad_RIGHT( false ), 
			b_dPad_LEFT( false ), 
			b_buttonY( false ), 
			b_buttonB( false ), 
			b_buttonA( false ), 
			b_buttonX( false ), 
			b_left1( false ), 
			b_right1( false ), 
			b_left2( false ), 
			b_right2( false ), 
			b_select( false ), 
			b_start( false ), 
			b_leftStickButton( false ), 
			b_rightStickButton( false ), 
			f_leftStickAxisX( 0.0f ), 
			f_leftStickAxisY( 0.0f ), 
			f_rightStickAxisX( 0.0f ), 
			f_rightStickAxisY( 0.0f )
	{};


	void HIDInputGamepad::GamepadState::clear()
	{
		b_dPad_UP = false;
		b_dPad_DOWN = false;
		b_dPad_RIGHT = false;
		b_dPad_LEFT = false;
		b_buttonY = false;
		b_buttonB = false;
		b_buttonA = false;
		b_buttonX = false;
		b_left1 = false;
		b_right1 = false;
		b_left2 = false;
		b_right2 = false;
		b_select = false;
		b_start = false;
		b_leftStickButton = false;
		b_rightStickButton = false;
		f_leftStickAxisX = 0.0f;
		f_leftStickAxisY = 0.0f;
		f_rightStickAxisX = 0.0f;
		f_rightStickAxisY = 0.0f;
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                 HIDInputGamepad::ValuesMapping                 ////////
	////////////////////////////////////////////////////////////////////////////////


	std::string HIDInputGamepad::ValuesMapping::serialize() const
	{
		char c_string[ 1024 ] = { 0 };
		char* c_ptr = c_string;
			
		for ( int i = 0; i < 4; ++i )
			c_ptr += sprintf( c_ptr, "dpd:%d:%d;", i, bt_dPap[ i ] );

		for ( int i = 0; i < 4; ++i )
			c_ptr += sprintf( c_ptr, "axs:%d:%d;", i, bt_stickAxes[ i ] );

		for ( int i = 0; i < HID_GAMEPAD_BUTTONS; ++i )
			c_ptr += sprintf( c_ptr, "btn:%d:%d;", i, bt_buttons[ i ] );

		return std::string( c_string );
	};


	void HIDInputGamepad::ValuesMapping::unserialize( const std::string _str_serializedString )
	{
		std::list< std::pair< std::string, std::pair< int, int > > > ls_data;

		int i_begin = 0;
		int i_end = 0;

		while ( ( i_end = _str_serializedString.find_first_of(';', i_end) ) != std::string::npos )
		{
			std::string str_tmp( _str_serializedString, i_begin, i_end - 1 );

			int p1 = str_tmp.find_first_of( ':' );
			int p2 = str_tmp.find_first_of( ':', p1 + 1 );

			if ( p1 != std::string::npos && p2 != std::string::npos )
			{
				ls_data.push_back( std::make_pair( str_tmp.substr( 0, p1 ), std::make_pair( atoi( str_tmp.c_str() + p1 + 1 ), atoi( str_tmp.c_str() + p2 + 1 ) ) ) );
			}

			i_begin = i_end + 1;
		}

		clear();

		for ( std::list< std::pair< std::string, std::pair< int, int > > >::iterator it = ls_data.begin(); it != ls_data.end(); ++it )
		{
			if ( it->first == std::string( "dpd" ) )
			{
				if ( it->second.first < 4 && it->second.second < 4 )
				{
					bt_dPap[ it->second.first ] = it->second.second;
				}
			}
			else if ( it->first == std::string( "axs" ) )
			{
				if ( it->second.first < 4 && it->second.second < 4 )
				{
					bt_stickAxes[ it->second.first ] = it->second.second;
				}
			}
			else if ( it->first == std::string( "btn" ) )
			{
				if ( it->second.first < HID_GAMEPAD_BUTTONS && it->second.second < HID_GAMEPAD_BUTTONS )
				{
					bt_buttons[ it->second.first ] = it->second.second;
				}
			}
		}
	};


	HIDInputGamepad::ValuesMapping::ValuesMapping()
	{
		for ( int i = 0; i < 4; ++i ) bt_dPap[ i ] = i;
		for ( int i = 0; i < HID_GAMEPAD_BUTTONS; ++i ) bt_buttons[ i ] = i;
		for ( int i = 0; i < 4; ++i ) bt_stickAxes[ i ] = i;
	};


	void HIDInputGamepad::ValuesMapping::clear()
	{
		for ( int i = 0; i < 4; ++i ) bt_dPap[ i ] = i;
		for ( int i = 0; i < HID_GAMEPAD_BUTTONS; ++i ) bt_buttons[ i ] = i;
		for ( int i = 0; i < 4; ++i ) bt_stickAxes[ i ] = i;
	};


	HIDInputGamepad::ValuesMapping& HIDInputGamepad::ValuesMapping::operator= ( const ValuesMapping& _rhs )
	{
		bt_hatSwitchShift = _rhs.bt_hatSwitchShift;
		for ( BYTE i = 0; i < 4; ++i ) bt_dPap[ i ] = _rhs.bt_dPap[ i ];
		for ( BYTE i = 0; i < HID_GAMEPAD_BUTTONS; ++i ) bt_buttons[ i ] = _rhs.bt_buttons[ i ];
		for ( BYTE i = 0; i < 4; ++i ) bt_stickAxes[ i ] = _rhs.bt_stickAxes[ i ];

		return *this;
	};



	////////////////////////////////////////////////////////////////////////////////
	////////                         HIDInputGamepad                        ////////
	////////////////////////////////////////////////////////////////////////////////


	bool HIDInputGamepad::_updateFuncDPad( BYTE _bt_innerIndex, const HIDP_DATA& _hd_inputData )
	{
		_bt_innerIndex = vm_userMapping.bt_dPap[ _bt_innerIndex ];
		bool b_tmpValue( _hd_inputData.On == TRUE );
		if ( hio_dPadValuePtr[ _bt_innerIndex ]->set( b_tmpValue ) )
		{
			gs_state.b_dPad[ _bt_innerIndex ] = b_tmpValue;
			pushEvent( hio_dPadValuePtr[ _bt_innerIndex ]->getID(), hio_dPadValuePtr[ _bt_innerIndex ]->getValue() );
			return true;
		}
		return false;
	};


	bool HIDInputGamepad::_updateFuncDPadFromHatSwitch( BYTE _bt_innerIndex, const HIDP_DATA& _hd_inputData )
	{
		if ( _hd_inputData.RawValue <= hio_hatSwitchValuePtr->getMax() )
		{
			int i_tmpStep( (BYTE) _hd_inputData.RawValue );
			if ( hio_hatSwitchValuePtr->getMax() - hio_hatSwitchValuePtr->getMin() + 1 == 4 )
				i_tmpStep = i_tmpStep * 2;
			i_tmpStep = ( i_tmpStep + vm_userMapping.bt_hatSwitchShift ) % 8;

			if ( hio_hatSwitchValuePtr->set( i_tmpStep ) )
			{
				pushEvent( hio_hatSwitchValuePtr->getID(), hio_hatSwitchValuePtr->getValue() );

				static const BYTE bt_dpad_helper[] = { 0x01, 0x05, 0x04, 0x06, 0x02, 0x10, 0x08, 0x09 };
				bool b_tmpBtnValue(false);

				b_tmpBtnValue = ( ( 0x01 << DPAD_INDEX_UP ) & bt_dpad_helper[ i_tmpStep ] ) != 0;
				if ( b_tmpBtnValue != gs_state.b_dPad[ DPAD_INDEX_UP ] )
				{
					hio_dPadValuePtr[ DPAD_INDEX_UP ]->set( b_tmpBtnValue );
					gs_state.b_dPad[ DPAD_INDEX_UP ] = b_tmpBtnValue;
					pushEvent( hio_dPadValuePtr[ DPAD_INDEX_UP ]->getID(), hio_dPadValuePtr[ DPAD_INDEX_UP ]->getValue() );
				}

				b_tmpBtnValue = ( ( 0x01 << DPAD_INDEX_DOWN ) & bt_dpad_helper[ i_tmpStep ] ) != 0;
				if ( b_tmpBtnValue != gs_state.b_dPad[ DPAD_INDEX_DOWN ] )
				{
					hio_dPadValuePtr[ DPAD_INDEX_DOWN ]->set( b_tmpBtnValue );
					gs_state.b_dPad[ DPAD_INDEX_DOWN ] = b_tmpBtnValue;
					pushEvent( hio_dPadValuePtr[ DPAD_INDEX_DOWN ]->getID(), hio_dPadValuePtr[ DPAD_INDEX_DOWN ]->getValue() );
				}

				b_tmpBtnValue = ( ( 0x01 << DPAD_INDEX_RIGHT ) & bt_dpad_helper[ i_tmpStep ] ) != 0;
				if ( b_tmpBtnValue != gs_state.b_dPad[ DPAD_INDEX_RIGHT ] )
				{
					hio_dPadValuePtr[ DPAD_INDEX_RIGHT ]->set( b_tmpBtnValue );
					gs_state.b_dPad[ DPAD_INDEX_RIGHT ] = b_tmpBtnValue;
					pushEvent( hio_dPadValuePtr[ DPAD_INDEX_RIGHT ]->getID(), hio_dPadValuePtr[ DPAD_INDEX_RIGHT ]->getValue() );
				}

				b_tmpBtnValue = ( ( 0x01 << DPAD_INDEX_LEFT ) & bt_dpad_helper[ i_tmpStep ] ) != 0;
				if ( b_tmpBtnValue != gs_state.b_dPad[ DPAD_INDEX_LEFT ] )
				{
					hio_dPadValuePtr[ DPAD_INDEX_LEFT ]->set( b_tmpBtnValue );
					gs_state.b_dPad[ DPAD_INDEX_LEFT ] = b_tmpBtnValue;
					pushEvent( hio_dPadValuePtr[ DPAD_INDEX_LEFT ]->getID(), hio_dPadValuePtr[ DPAD_INDEX_LEFT ]->getValue() );
				}

				return true;
			}
		}
		else
		{
			if ( hio_hatSwitchValuePtr->set( _hd_inputData.RawValue ) )
			{
				pushEvent( hio_hatSwitchValuePtr->getID(), hio_hatSwitchValuePtr->getValue() );

				if ( gs_state.b_dPad[ DPAD_INDEX_UP ] )
				{
					hio_dPadValuePtr[ DPAD_INDEX_UP ]->set( false );
					gs_state.b_dPad[ DPAD_INDEX_UP ] = false;
					pushEvent( hio_dPadValuePtr[ DPAD_INDEX_UP ]->getID(), hio_dPadValuePtr[ DPAD_INDEX_UP ]->getValue() );
				}

				if ( gs_state.b_dPad[ DPAD_INDEX_DOWN ] )
				{
					hio_dPadValuePtr[ DPAD_INDEX_DOWN ]->set( false );
					gs_state.b_dPad[ DPAD_INDEX_DOWN ] = false;
					pushEvent( hio_dPadValuePtr[ DPAD_INDEX_DOWN ]->getID(), hio_dPadValuePtr[ DPAD_INDEX_DOWN ]->getValue() );
				}

				if ( gs_state.b_dPad[ DPAD_INDEX_RIGHT ] )
				{
					hio_dPadValuePtr[ DPAD_INDEX_RIGHT ]->set( false );
					gs_state.b_dPad[ DPAD_INDEX_RIGHT ] = false;
					pushEvent( hio_dPadValuePtr[ DPAD_INDEX_RIGHT ]->getID(), hio_dPadValuePtr[ DPAD_INDEX_RIGHT ]->getValue() );
				}

				if ( gs_state.b_dPad[ 3 ] )
				{
					hio_dPadValuePtr[ DPAD_INDEX_LEFT ]->set( false );
					gs_state.b_dPad[ DPAD_INDEX_LEFT ] = false;
					pushEvent( hio_dPadValuePtr[ DPAD_INDEX_LEFT ]->getID(), hio_dPadValuePtr[ DPAD_INDEX_LEFT ]->getValue() );
				}

				return true;				
			}
		}

		return false;
	};


	bool HIDInputGamepad::_updateFuncButton( BYTE _bt_innerIndex, const HIDP_DATA& _hd_inputData )
	{
		_bt_innerIndex = vm_userMapping.bt_buttons[ _bt_innerIndex ];
		bool b_tmpValue( _hd_inputData.On == TRUE );
		if ( hio_buttonsValuePtr[ _bt_innerIndex ]->set( b_tmpValue ) )
		{
			gs_state.b_buttons[ _bt_innerIndex ] = b_tmpValue;
			pushEvent( hio_buttonsValuePtr[ _bt_innerIndex ]->getID(), hio_buttonsValuePtr[ _bt_innerIndex ]->getValue() );
			return true;
		}
		return false;
	};


	bool HIDInputGamepad::_updateFuncAxis( BYTE _bt_innerIndex, const HIDP_DATA& _hd_inputData )
	{
		_bt_innerIndex = vm_userMapping.bt_stickAxes[ _bt_innerIndex ];
		if ( hio_stickAxisValuePtr[ _bt_innerIndex ]->set( _hd_inputData.RawValue ) )
		{
			gs_state.f_stickAxes[ _bt_innerIndex ] = hio_stickAxisValuePtr[ _bt_innerIndex ]->getNormalizedAxis();
			static const unsigned short bt_axes_usage_helper[] = { HID_INPUT_AXES_X_USAGE_PAGE, HID_INPUT_AXES_Y_USAGE_PAGE, HID_INPUT_AXES_R_X_USAGE_PAGE, HID_INPUT_AXES_R_Y_USAGE_PAGE };
			pushEvent( hio_stickAxisValuePtr[ _bt_innerIndex ]->getID(), hio_stickAxisValuePtr[ _bt_innerIndex ]->getValue() );
			return true;
		}
		return false;
	};


	HIDInputGamepad::HIDInputGamepad( std::string _str_name, BYTE _bt_localDeviceID, HANDLE _handle_deviceHandle, HWND _hwnd_targetWindowHandle, unsigned short _us_usagePage, unsigned short _us_usage ) : 
			HIDInputDevice( _str_name, _bt_localDeviceID, _handle_deviceHandle, _hwnd_targetWindowHandle, _us_usagePage, _us_usage )
	{
		str_serializedMapping = vm_userMapping.serialize();
		ZeroMemory( &hio_hatSwitchValuePtr, sizeof( HIDIOValueHatSwitch* ) );
		ZeroMemory( hio_dPadValuePtr, sizeof( HIDIOButton* ) * 4 );
		ZeroMemory( hio_buttonsValuePtr, sizeof( HIDIOButton* ) * HID_GAMEPAD_BUTTONS );
		ZeroMemory( hio_stickAxisValuePtr, sizeof( HIDIOValueStickAxis* ) * 4 );
	};


	void HIDInputGamepad::setSerializedMapping( const std::string _str_serializedString )
	{
		str_serializedMapping = _str_serializedString;
		vm_userMapping.unserialize( str_serializedMapping );
	};


	std::string HIDInputGamepad::getSerializedMappings()
	{
		return vm_userMapping.serialize();
	};


	void HIDInputGamepad::setMapping( const ValuesMapping& _vm_mapping )
	{
		str_serializedMapping.clear();
		vm_userMapping = _vm_mapping;
	};


	DWORD HIDInputGamepad::registerDevice( DWORD _dw_additionalFlags )
	{
		if ( DWORD out = HIDInputDevice::registerDevice( _dw_additionalFlags ) )
		{
			HIDInputDevice::unregisterDevice();
			return out;
		}

		ZeroMemory( &gs_state, sizeof( GamepadState ) );
		ZeroMemory( &gs_stateHistory, sizeof( GamepadState ) );

		//	setup update functions
		vec_reportIndicesUpdateFuncMaping.clear();
		vec_reportIndicesUpdateFuncMaping.resize( vec_indexedInputs.size() );


		int i_hatSwitchIndex( -1 );
		int i_dPadIndex[ 4 ] = { -1, -1, -1, -1 };
		int i_buttonIndex[ HID_GAMEPAD_BUTTONS ] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
		int i_axes[ 9 ] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };

		for ( int i = 0; i < vec_indexedInputs.size(); ++i )
		{
			ControlID cid_tmpID = vec_indexedInputs[ i ]->getID();

			if ( cid_tmpID.getControlUsagePage() == HID_INPUT_BTN_USAGE_PAGE )
			{
				if ( cid_tmpID.getControlUsage() <= HID_GAMEPAD_BUTTONS )
				{
					i_buttonIndex[ cid_tmpID.getControlUsage() - 1 ] = i;
				}
			}
			else if ( cid_tmpID.getControlUsagePage() == HID_INPUT_GENERIC_CONTROLS_USAGE_PAGE )
			{
				//hat switch
				if ( cid_tmpID.getControlUsage() == HID_INPUT_HAT_SWITCH_USAGE )
					i_hatSwitchIndex = i;

				//DPAD buttons
				else if ( cid_tmpID.getControlUsage() == HID_INPUT_DPAD_UP_USAGE )
					i_dPadIndex[ DPAD_INDEX_UP ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_DPAD_DOWN_USAGE )
					i_dPadIndex[ DPAD_INDEX_DOWN ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_DPAD_RIGHT_USAGE )
					i_dPadIndex[ DPAD_INDEX_RIGHT ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_DPAD_LEFT_USAGE )
					i_dPadIndex[ DPAD_INDEX_LEFT ] = i;

				//axes
				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_X_USAGE_PAGE )
					i_axes[ 0 ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_Y_USAGE_PAGE )
					i_axes[ 1 ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_Z_USAGE_PAGE )
					i_axes[ 2 ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_R_X_USAGE_PAGE )
					i_axes[ 3 ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_R_Y_USAGE_PAGE )
					i_axes[ 4 ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_R_Z_USAGE_PAGE )
					i_axes[ 5 ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_V_X_USAGE_PAGE )
					i_axes[ 6 ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_V_Y_USAGE_PAGE )
					i_axes[ 7 ] = i;

				else if ( cid_tmpID.getControlUsage() == HID_INPUT_AXES_V_Z_USAGE_PAGE )
					i_axes[ 8 ] = i;

			}
		}


		// empty controls pointers
		ZeroMemory( &hio_hatSwitchValuePtr, sizeof( HIDIOValueHatSwitch* ) );
		ZeroMemory( hio_dPadValuePtr, sizeof( HIDIOButton* ) * 4 );
		ZeroMemory( hio_buttonsValuePtr, sizeof( HIDIOButton* ) * HID_GAMEPAD_BUTTONS );
		ZeroMemory( hio_stickAxisValuePtr, sizeof( HIDIOValueStickAxis* ) * 4 );


		// setup dpad buttons
		if ( i_dPadIndex[ DPAD_INDEX_UP ] >= 0 )
		{
			if ( vec_reportIndicesUpdateFuncMaping.size() <= i_dPadIndex[ DPAD_INDEX_UP ] ) vec_reportIndicesUpdateFuncMaping.resize( i_dPadIndex[ DPAD_INDEX_UP ] + 1 );
			vec_reportIndicesUpdateFuncMaping[ i_dPadIndex[ DPAD_INDEX_UP ] ] = std::make_pair( DPAD_INDEX_UP, &HIDInputGamepad::_updateFuncDPad );
			hio_dPadValuePtr[ DPAD_INDEX_UP ] = static_cast <HIDIOButton*> ( vec_indexedInputs[ i_dPadIndex[ DPAD_INDEX_UP ] ] );
		}

		if ( i_dPadIndex[ DPAD_INDEX_DOWN ] >= 0 )
		{
			if ( vec_reportIndicesUpdateFuncMaping.size() <= i_dPadIndex[ DPAD_INDEX_DOWN ] ) vec_reportIndicesUpdateFuncMaping.resize( i_dPadIndex[ DPAD_INDEX_DOWN ] + 1 );
			vec_reportIndicesUpdateFuncMaping[ i_dPadIndex[ DPAD_INDEX_DOWN ] ] = std::make_pair( DPAD_INDEX_DOWN, &HIDInputGamepad::_updateFuncDPad );
			hio_dPadValuePtr[ DPAD_INDEX_DOWN ] = static_cast <HIDIOButton*> ( vec_indexedInputs[ i_dPadIndex[ DPAD_INDEX_DOWN ] ] );
		}

		if ( i_dPadIndex[ DPAD_INDEX_RIGHT ] >= 0 )
		{
			if ( vec_reportIndicesUpdateFuncMaping.size() <= i_dPadIndex[ DPAD_INDEX_RIGHT ] ) vec_reportIndicesUpdateFuncMaping.resize( i_dPadIndex[ DPAD_INDEX_RIGHT ] + 1 );
			vec_reportIndicesUpdateFuncMaping[ i_dPadIndex[ DPAD_INDEX_RIGHT ] ] = std::make_pair( DPAD_INDEX_RIGHT, &HIDInputGamepad::_updateFuncDPad );
			hio_dPadValuePtr[ DPAD_INDEX_RIGHT ] = static_cast <HIDIOButton*> ( vec_indexedInputs[ i_dPadIndex[ DPAD_INDEX_RIGHT ] ] );
		}

		if ( i_dPadIndex[ DPAD_INDEX_LEFT ] >= 0 )
		{
			if ( vec_reportIndicesUpdateFuncMaping.size() <= i_dPadIndex[ DPAD_INDEX_LEFT ] ) vec_reportIndicesUpdateFuncMaping.resize( i_dPadIndex[ DPAD_INDEX_LEFT ] + 1 );
			vec_reportIndicesUpdateFuncMaping[ i_dPadIndex[ DPAD_INDEX_LEFT ] ] = std::make_pair( DPAD_INDEX_LEFT, &HIDInputGamepad::_updateFuncDPad );
			hio_dPadValuePtr[ DPAD_INDEX_LEFT ] = static_cast <HIDIOButton*> ( vec_indexedInputs[ i_dPadIndex[ DPAD_INDEX_LEFT ] ] );
		}


		// setup hatswitch instead of dpad
		if ( i_hatSwitchIndex >= 0 )
		{
			if ( vec_reportIndicesUpdateFuncMaping.size() <= i_hatSwitchIndex ) vec_reportIndicesUpdateFuncMaping.resize( i_hatSwitchIndex + 1 );
			vec_reportIndicesUpdateFuncMaping[ i_hatSwitchIndex ] = std::make_pair( 0, &HIDInputGamepad::_updateFuncDPadFromHatSwitch );

			remapInput( i_hatSwitchIndex, &( HIDIOValueHatSwitch( *( static_cast <HIDIOValueHatSwitch*> ( vec_indexedInputs[ i_hatSwitchIndex ] ) ) ) ) );
			hio_hatSwitchValuePtr = static_cast <HIDIOValueHatSwitch*> ( vec_indexedInputs[ i_hatSwitchIndex ] );

			// add virtual controls hat swith is translating to
			if ( i_dPadIndex[ DPAD_INDEX_UP ] < 0 )
			{
				addVirtualInput( &( HIDIOButton( i_hatSwitchIndex, HID_INPUT_DPAD_USAGE_PAGE, HID_INPUT_DPAD_UP_USAGE,    getLocalDeviceID() ) ) );
				hio_dPadValuePtr[ DPAD_INDEX_UP ] = static_cast <HIDIOButton*> ( map_mappedInputs.find( ControlID( getLocalDeviceID(), HID_INPUT_DPAD_USAGE_PAGE, HID_INPUT_DPAD_UP_USAGE )    )->second );
			}

			if ( i_dPadIndex[ DPAD_INDEX_DOWN ] < 0 )
			{
				addVirtualInput( &( HIDIOButton( i_hatSwitchIndex, HID_INPUT_DPAD_USAGE_PAGE, HID_INPUT_DPAD_DOWN_USAGE,  getLocalDeviceID() ) ) );
				hio_dPadValuePtr[ DPAD_INDEX_DOWN ] = static_cast <HIDIOButton*> ( map_mappedInputs.find( ControlID( getLocalDeviceID(), HID_INPUT_DPAD_USAGE_PAGE, HID_INPUT_DPAD_DOWN_USAGE ) )->second );
			}

			if ( i_dPadIndex[ DPAD_INDEX_RIGHT ] < 0 )
			{
				addVirtualInput( &( HIDIOButton( i_hatSwitchIndex, HID_INPUT_DPAD_USAGE_PAGE, HID_INPUT_DPAD_RIGHT_USAGE, getLocalDeviceID() ) ) );
				hio_dPadValuePtr[ DPAD_INDEX_RIGHT ] = static_cast <HIDIOButton*> ( map_mappedInputs.find( ControlID( getLocalDeviceID(), HID_INPUT_DPAD_USAGE_PAGE, HID_INPUT_DPAD_RIGHT_USAGE ) )->second );
			}

			if ( i_dPadIndex[ DPAD_INDEX_LEFT ] < 0 )
			{
				addVirtualInput( &( HIDIOButton( i_hatSwitchIndex, HID_INPUT_DPAD_USAGE_PAGE, HID_INPUT_DPAD_LEFT_USAGE,  getLocalDeviceID() ) ) );
				hio_dPadValuePtr[ DPAD_INDEX_LEFT ] = static_cast <HIDIOButton*> ( map_mappedInputs.find( ControlID( getLocalDeviceID(), HID_INPUT_DPAD_USAGE_PAGE, HID_INPUT_DPAD_LEFT_USAGE ) )->second );
			}
		}


		// setup buttons
		for ( int i = 0; i < HID_GAMEPAD_BUTTONS; ++i )
		{
			if ( i_buttonIndex[ i ] >= 0 )
			{
				if ( vec_reportIndicesUpdateFuncMaping.size() <= i_buttonIndex[ i ] ) vec_reportIndicesUpdateFuncMaping.resize( i_buttonIndex[ i ] + 1 );
				vec_reportIndicesUpdateFuncMaping[ i_buttonIndex[ i ] ] = std::make_pair( i, &HIDInputGamepad::_updateFuncButton );

				if ( hio_buttonsValuePtr[ i ] )
					hio_buttonsValuePtr[ i ] = static_cast <HIDIOButton*> ( vec_indexedInputs[ i_buttonIndex[ i ] ] );
			}
		}


		// enumerate and setup axes
		unsigned short us_usagePages[4] = { HID_INPUT_AXES_X_USAGE_PAGE, HID_INPUT_AXES_Y_USAGE_PAGE, HID_INPUT_AXES_R_X_USAGE_PAGE, HID_INPUT_AXES_R_Y_USAGE_PAGE };
		int j = 0;
		for ( int i = 0; i < 9 && j < 4; ++i )
		{
			if ( i_axes[ i ] >= 0 )
			{
				if ( vec_reportIndicesUpdateFuncMaping.size() <= i_axes[ i ] ) vec_reportIndicesUpdateFuncMaping.resize( i_axes[ i ] + 1 );
				vec_reportIndicesUpdateFuncMaping[ i_axes[ i ] ] = std::make_pair( j, &HIDInputGamepad::_updateFuncAxis );

				remapInput( i_axes[ i ], &( HIDIOValueStickAxis( *( static_cast <HIDIOValueStickAxis*> ( vec_indexedInputs[ i_axes[ i ] ] ) ), ControlID( getLocalDeviceID(), HID_INPUT_AXES_USAGE_PAGE, us_usagePages[ j ] ) ) ) );
				hio_stickAxisValuePtr[ j ] = static_cast <HIDIOValueStickAxis*> ( vec_indexedInputs[ i_axes[ i ] ] );

				j++;
			}
		}

		return 0;
	};


	DWORD HIDInputGamepad::unregisterDevice()
	{
		if ( DWORD out = HIDInputDevice::unregisterDevice() )
			return out;

		ZeroMemory( &gs_state, sizeof( GamepadState ) );
		ZeroMemory( &gs_stateHistory, sizeof( GamepadState ) );

		hio_hatSwitchValuePtr = NULL;
		ZeroMemory( hio_dPadValuePtr, sizeof( HIDIOButton* ) * 4 );
		ZeroMemory( hio_buttonsValuePtr, sizeof( HIDIOButton* ) * HID_GAMEPAD_BUTTONS );
		ZeroMemory( hio_stickAxisValuePtr, sizeof( HIDIOValueStickAxis* ) * 4 );
		vec_reportIndicesUpdateFuncMaping.clear();

		return 0;
	};


	void HIDInputGamepad::poll()
	{
		HIDInputDevice::poll();
		memcpy( &gs_stateHistory, &gs_state, sizeof( GamepadState ) );
	};



	bool HIDInputGamepad::processRawInput( const RAWINPUT& _ri_input )
	{
		if ( _ri_input.header.dwType == RIM_TYPEHID )
		{
			bool b_ret = false;
			getDataList( _ri_input );

			for ( unsigned int i = 0; i < ul_dataListLength; ++i )
			{
				if ( vec_reportIndicesUpdateFuncMaping[ hd_dataListPointer[ i ].DataIndex ].second )
				{
					b_ret = b_ret || ( this->*( vec_reportIndicesUpdateFuncMaping[ hd_dataListPointer[ i ].DataIndex ].second ) )( vec_reportIndicesUpdateFuncMaping[ hd_dataListPointer[ i ].DataIndex ].first, hd_dataListPointer[ i ] );
				}
			}

			return b_ret;
		}

		return false;
	};

}
