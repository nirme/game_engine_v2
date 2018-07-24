#include "utility.h"


float getAbsoluteDistance(D3DXVECTOR3* point, D3DXVECTOR3* zero)
{
	if (point == NULL)
		return 0.0f;

	if (zero == NULL)
		return D3DXVec3Length(point);

	return D3DXVec3Length(&((*point) - (*zero)));
};

HRESULT setStringA(char** _Dest, const char* _Source, unsigned int maxSize)
{
	if (_Source == NULL)
	{
		if (*_Dest != NULL)
		{
			delete [] *_Dest;
			*_Dest = NULL;
		}
		return S_OK;
	}
	if (*_Dest != NULL)
	{
		delete [] *_Dest;
		*_Dest = NULL;
	}
	unsigned int size = (maxSize != 0) ? (min(maxSize, (strlen(_Source)+1))) : (strlen(_Source)+1);
	*_Dest = new char [size];
	if (SUCCEEDED(StringCchCopyA(*_Dest, size, _Source)))
		return S_OK;
	*_Dest = NULL;
	return E_OUTOFMEMORY;
};

HRESULT setStringW(wchar_t** _Dest, const wchar_t* _Source, unsigned int maxSize)
{
	if (_Source == NULL)
	{
		if (*_Dest != NULL)
		{
			delete [] *_Dest;
			*_Dest = NULL;
		}
		return S_OK;
	}
	if (*_Dest != NULL)
	{
		delete [] *_Dest;
		*_Dest = NULL;
	}
	unsigned int size = (maxSize != 0) ? (min(maxSize, (wcslen(_Source)+1))) : (wcslen(_Source)+1);
	*_Dest = new wchar_t [size];
	if (SUCCEEDED(StringCchCopyW(*_Dest, size, _Source)))
		return S_OK;
	*_Dest = NULL;
	return E_OUTOFMEMORY;
};


bool compare(float _a, float _b, float _epsilon)
{
	if (abs(_a - _b) > _epsilon)
		return false;
	return true;
};
bool compare(double _a, double _b, double _epsilon)
{
	if (abs(_a - _b) > _epsilon)
		return false;
	return true;
};


bool compareCaseInsensitive( const char* _str1, const char* _str2 )
{
	unsigned int i = 0;
	while ( _str1[i] && _str2[i] )
	{
		if ( _str1[i] != _str2[i] )
			return false;
		i++;
	}

	return true;
};


bool operator >(const D3DDISPLAYMODE& a, const D3DDISPLAYMODE& b)
{
	if (a.Width <= b.Width)
		return false;
	if (a.Height <= b.Height)
		return false;
	if (a.Format <= b.Format)
		return false;
	if (a.RefreshRate <= b.RefreshRate)
		return false;
	return true;
};

bool operator <(const D3DDISPLAYMODE& a, const D3DDISPLAYMODE& b)
{
	if (a.Width >= b.Width)
		return false;
	if (a.Height >= b.Height)
		return false;
	if (a.Format >= b.Format)
		return false;
	if (a.RefreshRate >= b.RefreshRate)
		return false;
	return true;
};

bool operator ==(const D3DDISPLAYMODE& a, const D3DDISPLAYMODE& b)
{
	if (a.Format == b.Format && a.Height == b.Height && a.RefreshRate == b.RefreshRate && a.Width == b.Width)
		return true;
	return false;
};


string tolower(string str)
{
	string out;
	unsigned int size = str.size();
	unsigned int i = 0;
	out.resize(size);
	while (i < size)
	{
		out[i] = tolower(str[i++]);
	}
	return out;
};


wstring stringToWstring(const string &_str)
{
	wstring ws;
	ws.assign(_str.begin(), _str.end());
	return ws;
};


float ComputeGaussianFalloffCurve( float _n, float _theta )
{
	return (1.0 / sqrt(CONST_TAU * _theta)) * exp(-(_n * _n) / (2 * _theta * _theta));
};


D3DXMATRIX* D3DXMatrixTransformation( D3DXMATRIX* _pOut, const D3DXVECTOR3* _pScaling, const D3DXQUATERNION* _pRotation, const D3DXVECTOR3* _pTranslation )
{
	_pOut->_11 = _pScaling->x * ( 1.0f - 2.0f * ( _pRotation->y * _pRotation->y + _pRotation->z * _pRotation->z ) );
	_pOut->_12 = _pScaling->x * ( 2.0f * ( ( _pRotation->x * _pRotation->y ) + ( _pRotation->z * _pRotation->w ) ) );
	_pOut->_13 = _pScaling->x * ( 2.0f * ( ( _pRotation->x * _pRotation->z ) - ( _pRotation->y * _pRotation->w ) ) );
	_pOut->_14 = 0.0f;

	_pOut->_21 = _pScaling->y * ( 2.0f * ( ( _pRotation->x * _pRotation->y ) - ( _pRotation->z * _pRotation->w ) ) );
	_pOut->_22 = _pScaling->y * ( 1.0f - 2.0f * ( _pRotation->x * _pRotation->x + _pRotation->z * _pRotation->z ) );
	_pOut->_23 = _pScaling->y * ( 2.0f * ( ( _pRotation->y * _pRotation->z ) + ( _pRotation->x * _pRotation->w ) ) );
	_pOut->_24 = 0.0f;

	_pOut->_31 = _pScaling->z * ( 2.0f * ( ( _pRotation->x * _pRotation->z ) + ( _pRotation->y * _pRotation->w ) ) );
	_pOut->_32 = _pScaling->z * ( 2.0f * ( ( _pRotation->y * _pRotation->z ) - ( _pRotation->x * _pRotation->w ) ) );
	_pOut->_33 = _pScaling->z * ( 1.0f - 2.0f * ( _pRotation->x * _pRotation->x + _pRotation->y * _pRotation->y ) );
	_pOut->_34 = 0.0f;

	_pOut->_41 = _pTranslation->x;
	_pOut->_42 = _pTranslation->y;
	_pOut->_43 = _pTranslation->z;
	_pOut->_44 = 1.0f;
	
	return _pOut;
};


D3DXMATRIX* D3DXMatrixTransformation( D3DXMATRIX* _pOut, float _scaling, const D3DXQUATERNION* _pRotation, const D3DXVECTOR3* _pTranslation )
{
	_pOut->_11 = _scaling * ( 1.0f - 2.0f * ( _pRotation->y * _pRotation->y + _pRotation->z * _pRotation->z ) );
	_pOut->_12 = _scaling * ( 2.0f * ( ( _pRotation->x * _pRotation->y ) + ( _pRotation->z * _pRotation->w ) ) );
	_pOut->_13 = _scaling * ( 2.0f * ( ( _pRotation->x * _pRotation->z ) - ( _pRotation->y * _pRotation->w ) ) );
	_pOut->_14 = 0.0f;

	_pOut->_21 = _scaling * ( 2.0f * ( ( _pRotation->x * _pRotation->y ) - ( _pRotation->z * _pRotation->w ) ) );
	_pOut->_22 = _scaling * ( 1.0f - 2.0f * ( _pRotation->x * _pRotation->x + _pRotation->z * _pRotation->z ) );
	_pOut->_23 = _scaling * ( 2.0f * ( ( _pRotation->y * _pRotation->z ) + ( _pRotation->x * _pRotation->w ) ) );
	_pOut->_24 = 0.0f;

	_pOut->_31 = _scaling * ( 2.0f * ( ( _pRotation->x * _pRotation->z ) + ( _pRotation->y * _pRotation->w ) ) );
	_pOut->_32 = _scaling * ( 2.0f * ( ( _pRotation->y * _pRotation->z ) - ( _pRotation->x * _pRotation->w ) ) );
	_pOut->_33 = _scaling * ( 1.0f - 2.0f * ( _pRotation->x * _pRotation->x + _pRotation->y * _pRotation->y ) );
	_pOut->_34 = 0.0f;

	_pOut->_41 = _pTranslation->x;
	_pOut->_42 = _pTranslation->y;
	_pOut->_43 = _pTranslation->z;
	_pOut->_44 = 1.0f;
	
	return _pOut;
};


D3DXMATRIX* D3DXMatrixTransformation( D3DXMATRIX* _pOut, const D3DXQUATERNION* _pRotation, const D3DXVECTOR3* _pTranslation )
{
	_pOut->_11 = 1.0f - 2.0f * ( _pRotation->y * _pRotation->y + _pRotation->z * _pRotation->z );
	_pOut->_12 = 2.0f * ( ( _pRotation->x * _pRotation->y ) + ( _pRotation->z * _pRotation->w ) );
	_pOut->_13 = 2.0f * ( ( _pRotation->x * _pRotation->z ) - ( _pRotation->y * _pRotation->w ) );
	_pOut->_14 = 0.0f;

	_pOut->_21 = 2.0f * ( ( _pRotation->x * _pRotation->y ) - ( _pRotation->z * _pRotation->w ) );
	_pOut->_22 = 1.0f - 2.0f * ( _pRotation->x * _pRotation->x + _pRotation->z * _pRotation->z );
	_pOut->_23 = 2.0f * ( ( _pRotation->y * _pRotation->z ) + ( _pRotation->x * _pRotation->w ) );
	_pOut->_24 = 0.0f;

	_pOut->_31 = 2.0f * ( ( _pRotation->x * _pRotation->z ) + ( _pRotation->y * _pRotation->w ) );
	_pOut->_32 = 2.0f * ( ( _pRotation->y * _pRotation->z ) - ( _pRotation->x * _pRotation->w ) );
	_pOut->_33 = 1.0f - 2.0f * ( _pRotation->x * _pRotation->x + _pRotation->y * _pRotation->y );
	_pOut->_34 = 0.0f;

	_pOut->_41 = _pTranslation->x;
	_pOut->_42 = _pTranslation->y;
	_pOut->_43 = _pTranslation->z;
	_pOut->_44 = 1.0f;
	
	return _pOut;
};


