#include "HandlerFFmpeg.h"

#include "resource.h"

#include "DecoderFFmpeg.h"
#include "Utility.h"

HandlerFFmpeg::HandlerFFmpeg()
{
}

std::wstring HandlerFFmpeg::GetDescription() const
{
	const std::wstring description = L"FFmpeg";
	return description;
}

std::set<std::wstring> HandlerFFmpeg::GetSupportedFileExtensions() const
{
	return {};
}

bool HandlerFFmpeg::GetTags( const std::wstring& /*filename*/, Tags& /*tags*/ ) const
{
	return false;
}

bool HandlerFFmpeg::SetTags( const std::wstring& /*filename*/, const Tags& /*tags*/ ) const
{
	return false;
}

Decoder::Ptr HandlerFFmpeg::OpenDecoder( const std::wstring& filename ) const
{
	DecoderFFmpeg* decoderFFmpeg = nullptr;
	try {
		decoderFFmpeg = new DecoderFFmpeg( filename );
	} catch ( const std::runtime_error& ) {
	}
	const Decoder::Ptr stream( decoderFFmpeg );
	return stream;
}

Encoder::Ptr HandlerFFmpeg::OpenEncoder() const
{
	return nullptr;
}

bool HandlerFFmpeg::IsDecoder() const
{
	return true;
}

bool HandlerFFmpeg::IsEncoder() const
{
	return false;
}

bool HandlerFFmpeg::CanConfigureEncoder() const
{
	return false;
}

bool HandlerFFmpeg::ConfigureEncoder( const HINSTANCE /*instance*/, const HWND /*parent*/, std::string& /*settings*/ ) const
{
	return false;
}

void HandlerFFmpeg::SettingsChanged( Settings& /*settings*/ )
{
}
