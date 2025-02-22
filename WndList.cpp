#include "WndList.h"

#include "resource.h"

#include <windowsx.h>

#include "DlgAddStream.h"
#include "Settings.h"
#include "Utility.h"
#include "VUPlayer.h"

#include <iomanip>
#include <sstream>
#include <vector>

// Column information.
WndList::ColumnFormats WndList::s_ColumnFormats = {
	{ Playlist::Column::Filepath, ColumnFormat( {				ID_SHOWCOLUMNS_FILEPATH,			ID_SORTPLAYLIST_FILEPATH,				IDS_COLUMN_FILEPATH,			LVCFMT_LEFT,		static_cast<int>( 200 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::Filetime, ColumnFormat( {				ID_SHOWCOLUMNS_FILETIME,			ID_SORTPLAYLIST_FILETIME,				IDS_COLUMN_FILETIME,			LVCFMT_LEFT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::Filesize, ColumnFormat( {				ID_SHOWCOLUMNS_FILESIZE,			ID_SORTPLAYLIST_FILESIZE,				IDS_COLUMN_FILESIZE,			LVCFMT_RIGHT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::Duration, ColumnFormat( {				ID_SHOWCOLUMNS_DURATION,			ID_SORTPLAYLIST_DURATION,				IDS_COLUMN_DURATION,			LVCFMT_RIGHT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::SampleRate, ColumnFormat( {			ID_SHOWCOLUMNS_SAMPLERATE,		ID_SORTPLAYLIST_SAMPLERATE,			IDS_COLUMN_SAMPLERATE,		LVCFMT_RIGHT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::BitsPerSample, ColumnFormat( {	ID_SHOWCOLUMNS_BITSPERSAMPLE,	ID_SORTPLAYLIST_BITSPERSAMPLE,	IDS_COLUMN_BITSPERSAMPLE,	LVCFMT_CENTER,	static_cast<int>( 50 * GetDPIScaling() ) /*width*/,		false /*canEdit*/ } ) },
	{ Playlist::Column::Channels, ColumnFormat( {				ID_SHOWCOLUMNS_CHANNELS,			ID_SORTPLAYLIST_CHANNELS,				IDS_COLUMN_CHANNELS,			LVCFMT_CENTER,	static_cast<int>( 50 * GetDPIScaling() ) /*width*/,		false /*canEdit*/ } ) },
	{ Playlist::Column::Artist, ColumnFormat( {					ID_SHOWCOLUMNS_ARTIST,				ID_SORTPLAYLIST_ARTIST,					IDS_COLUMN_ARTIST,				LVCFMT_LEFT,		static_cast<int>( 150 * GetDPIScaling() ) /*width*/,	true /*canEdit*/ } ) },
	{ Playlist::Column::Title, ColumnFormat( {					ID_SHOWCOLUMNS_TITLE,					ID_SORTPLAYLIST_TITLE,					IDS_COLUMN_TITLE,					LVCFMT_LEFT,		static_cast<int>( 150 * GetDPIScaling() ) /*width*/,	true /*canEdit*/ } ) },
	{ Playlist::Column::Album, ColumnFormat( {					ID_SHOWCOLUMNS_ALBUM,					ID_SORTPLAYLIST_ALBUM,					IDS_COLUMN_ALBUM,					LVCFMT_LEFT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	true /*canEdit*/ } ) },
	{ Playlist::Column::Genre, ColumnFormat( {					ID_SHOWCOLUMNS_GENRE,					ID_SORTPLAYLIST_GENRE,					IDS_COLUMN_GENRE,					LVCFMT_LEFT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	true /*canEdit*/ } ) },
	{ Playlist::Column::Year, ColumnFormat( {						ID_SHOWCOLUMNS_YEAR,					ID_SORTPLAYLIST_YEAR,						IDS_COLUMN_YEAR,					LVCFMT_CENTER,	static_cast<int>( 50 * GetDPIScaling() ) /*width*/,		true /*canEdit*/ } ) },
	{ Playlist::Column::Track, ColumnFormat( {					ID_SHOWCOLUMNS_TRACK,					ID_SORTPLAYLIST_TRACK,					IDS_COLUMN_TRACK,					LVCFMT_CENTER,	static_cast<int>( 50 * GetDPIScaling() ) /*width*/,		true /*canEdit*/ } ) },
	{ Playlist::Column::Type, ColumnFormat( {						ID_SHOWCOLUMNS_TYPE,					ID_SORTPLAYLIST_TYPE,						IDS_COLUMN_TYPE,					LVCFMT_LEFT,		static_cast<int>( 50 * GetDPIScaling() ) /*width*/,		false /*canEdit*/ } ) },
	{ Playlist::Column::Version, ColumnFormat( {				ID_SHOWCOLUMNS_VERSION,				ID_SORTPLAYLIST_VERSION,				IDS_COLUMN_VERSION,				LVCFMT_LEFT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::GainTrack, ColumnFormat( {			ID_SHOWCOLUMNS_TRACKGAIN,			ID_SORTPLAYLIST_TRACKGAIN,			IDS_COLUMN_GAINTRACK,			LVCFMT_RIGHT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::GainAlbum, ColumnFormat( {			ID_SHOWCOLUMNS_ALBUMGAIN,			ID_SORTPLAYLIST_ALBUMGAIN,			IDS_COLUMN_GAINALBUM,			LVCFMT_RIGHT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::Bitrate, ColumnFormat( {				ID_SHOWCOLUMNS_BITRATE,				ID_SORTPLAYLIST_BITRATE,				IDS_COLUMN_BITRATE,				LVCFMT_RIGHT,		static_cast<int>( 100 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) },
	{ Playlist::Column::Filename, ColumnFormat( {				ID_SHOWCOLUMNS_FILENAME,			ID_SORTPLAYLIST_FILENAME,				IDS_COLUMN_FILENAME,			LVCFMT_LEFT,		static_cast<int>( 150 * GetDPIScaling() ) /*width*/,	false /*canEdit*/ } ) }
};

// List control ID
UINT_PTR WndList::s_WndListID = 1000;

// File added message ID.
static const UINT MSG_FILEADDED = WM_APP + 100;

// File removed message ID.
static const UINT MSG_FILEREMOVED = WM_APP + 101;

// Message ID for reordering the dummy column after a drag operation.
static const UINT MSG_REORDERDUMMY = WM_APP + 102;

// Item updated message ID.
static const UINT MSG_ITEMUPDATED = WM_APP + 103;

// Drag timer ID.
static const UINT_PTR s_DragTimerID = 1010;

// Drag timer millisecond interval.
static const UINT s_DragTimerInterval = 20;

LRESULT CALLBACK WndList::ListProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	WndList* wndList = reinterpret_cast<WndList*>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
	if ( nullptr != wndList ) {
		switch( message )	{
			case WM_COMMAND : {
				const UINT commandID = LOWORD( wParam );
				wndList->OnCommand( commandID );
				break;
			}
			case WM_DROPFILES : {
				wndList->OnDropFiles( reinterpret_cast<HDROP>( wParam ) );
				break;
			}
			case MSG_FILEADDED : {
				AddedItem* addedItem = reinterpret_cast<AddedItem*>( wParam );
				wndList->AddFileHandler( addedItem );
				delete addedItem;
				break;
			}
			case MSG_FILEREMOVED : {
				const long removedItemID = static_cast<long>( wParam );
				wndList->RemoveFileHandler( removedItemID );
				break;
			}
			case MSG_ITEMUPDATED : {
				Playlist::Item* item = reinterpret_cast<Playlist::Item*>( wParam );
				wndList->ItemUpdatedHandler( item );
				delete item;
				break;
			}
			case MSG_REORDERDUMMY : {
				wndList->ReorderDummyColumn();
				break;
			}
			case WM_LBUTTONDBLCLK : {
				LVHITTESTINFO info = {};
				info.pt.x = LOWORD( lParam );
				info.pt.y = HIWORD( lParam );
				if ( ( ListView_SubItemHitTest( hwnd, &info ) >= 0 ) && ( info.iItem >= 0 ) ) {
					const long playlistItemID = wndList->GetPlaylistItemID( info.iItem );
					wndList->OnPlay( playlistItemID );
				}
				break;
			}
			case WM_CONTEXTMENU : {
				POINT pt = {};
				if ( -1 == lParam ) {
					const int itemIndex = wndList->GetCurrentSelectedIndex();
					if ( ( itemIndex >= 0 ) && ListView_IsItemVisible( hwnd, itemIndex ) ) {
						RECT rect = {};
						ListView_GetItemRect( hwnd, itemIndex, &rect, LVIR_BOUNDS );
						pt.x = rect.left;
						pt.y = rect.bottom;
					} else {
						const HWND header = ListView_GetHeader( hwnd );
						if ( nullptr != header ) {
							RECT rect = {};
							GetClientRect( header, &rect );
							pt.y = rect.bottom - rect.top;
						}
					}
					ClientToScreen( hwnd, &pt );
				} else {
					pt.x = LOWORD( lParam );
					pt.y = HIWORD( lParam );
				}
				wndList->OnContextMenu( pt );
				break;
			}
			case WM_DESTROY : {
				wndList->SaveSettings();
				SetWindowLongPtr( hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( wndList->GetDefaultWndProc() ) );
				break;
			}
			case WM_KEYDOWN : {
				switch ( wParam ) {
					case VK_DELETE : {
						wndList->DeleteSelectedItems();
						return 0;
					}
					case VK_SPACE : {
						if ( Output::State::Stopped != wndList->m_Output.GetState() ) {
							wndList->m_Output.Pause();
							return 0;
						}
					}
				}
				break;
			}
			case WM_LBUTTONUP : {
				if ( GetCapture() == hwnd ) {
					ReleaseCapture();
				}
				break;
			}
			case WM_CAPTURECHANGED : {
				wndList->OnEndDrag();
				break;
			}
			case WM_MOUSEMOVE : {
				if ( GetCapture() == hwnd ) {
					const POINT pt = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };
					wndList->OnDrag( pt );
				}
				break;
			}
			case WM_TIMER : {
				if ( ( GetCapture() == hwnd ) && ( s_DragTimerID == wParam ) ) {
					wndList->OnDragTimer();
					return 0;
				}
				break;
			}
			default : {
				break;
			}
		}
	}
	return CallWindowProc( wndList->GetDefaultWndProc(), hwnd, message, wParam, lParam );
}

LRESULT CALLBACK WndList::EditControlProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	WndList* wndList = reinterpret_cast<WndList*>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
	if ( nullptr != wndList ) {
		switch( message ) {
			case WM_WINDOWPOSCHANGING : {
				wndList->RepositionEditControl( reinterpret_cast<WINDOWPOS*>( lParam ) );
				break;
			}
			case WM_DESTROY : {
				SetWindowLongPtr( hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( wndList->GetEditControlWndProc() ) );
				break;
			}
			case WM_GETDLGCODE : {
				return DLGC_WANTALLKEYS;
			}
			default : {
				break;
			}
		}
	}
	return CallWindowProc( wndList->GetEditControlWndProc(), hwnd, message, wParam, lParam );
}

WndList::WndList( HINSTANCE instance, HWND parent, Settings& settings, Output& output ) :
	m_hInst( instance ),
	m_hWnd( NULL ),
	m_DefaultWndProc( NULL ),
	m_Playlist(),
	m_Settings( settings ),
	m_Output( output ),
	m_ColourFont( GetSysColor( COLOR_WINDOWTEXT ) ),
	m_ColourBackground( GetSysColor( COLOR_WINDOW ) ),
	m_ColourHighlight( GetSysColor( COLOR_HIGHLIGHT ) ),
	m_ColourStatusIcon( DEFAULT_ICONCOLOUR ),
	m_ChosenFont( NULL ),
	m_EditItem( -1 ),
	m_EditSubItem( -1 ),
	m_EditControlWndProc( NULL ),
	m_IsDragging( false ),
	m_DragImage( NULL ),
	m_OldCursor( NULL ),
	m_FilenameToIDs(),
	m_FilenameToSelect(),
	m_IconMap(),
	m_IconStatus( { -1, Output::State::Stopped } ),
	m_EnableStatusIcon( false ),
	m_IsHighContrast( IsHighContrastActive() )
{
	const DWORD exStyle = WS_EX_ACCEPTFILES;
	LPCTSTR className = WC_LISTVIEW;
	LPCTSTR windowName = NULL;
	const DWORD style = WS_CHILD | WS_TABSTOP | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS;
	const int x = 100;
	const int y = 100;
	const int width = 600;
	const int height = 400;
	LPVOID param = NULL;
	m_hWnd = CreateWindowEx( exStyle, className, windowName, style, x, y, width, height, parent, reinterpret_cast<HMENU>( s_WndListID++ ), instance, param );
	SetWindowLongPtr( m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( this ) );
	ListView_SetExtendedListViewStyle( m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER );
	SetWindowLongPtr( ListView_GetHeader( m_hWnd ), GWLP_USERDATA, reinterpret_cast<LONG_PTR>( this ) );
	m_DefaultWndProc = reinterpret_cast<WNDPROC>( SetWindowLongPtr( m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( ListProc ) ) );
	SetWindowAccessibleName( instance, m_hWnd, IDS_ACCNAME_LISTVIEW );

	// Insert the main column, which will contain the search text and status icon.
	LVCOLUMN lvc = {};
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_FIXED_WIDTH;
	ListView_InsertColumn( m_hWnd, 0 /*iCol*/, &lvc );

	ApplySettings();
}

WndList::~WndList()
{
	if ( nullptr != m_ChosenFont ) {
		DeleteObject( m_ChosenFont );
	}
}

WNDPROC WndList::GetDefaultWndProc()
{
	return m_DefaultWndProc;
}

HWND WndList::GetWindowHandle()
{
	return m_hWnd;
}

void WndList::ApplySettings()
{
	Settings::PlaylistColumns columns;
	LOGFONT logFont = GetFont();
	m_Settings.GetPlaylistSettings( columns, m_EnableStatusIcon, logFont, m_ColourFont, m_ColourBackground, m_ColourHighlight, m_ColourStatusIcon );

	if ( columns.empty() ) {
		columns = {
			{ static_cast<int>( Playlist::Column::Artist ), static_cast<int>( 150 * GetDPIScaling() ) /*width*/ },
			{ static_cast<int>( Playlist::Column::Title ), static_cast<int>( 150 * GetDPIScaling() ) /*width*/ },
			{ static_cast<int>( Playlist::Column::Album ), static_cast<int>( 150 * GetDPIScaling() ) /*width*/ },
			{ static_cast<int>( Playlist::Column::Duration ), static_cast<int>( 100 * GetDPIScaling() ) /*width*/ }
		};
	}
	bool titleShown = false;
	for ( auto iter = columns.begin(); !titleShown && ( iter != columns.end() ); iter++ ) {
		titleShown = ( Playlist::Column::Title == static_cast<Playlist::Column>( iter->ID ) );
	}
	if ( !titleShown ) {
		columns.push_front( { static_cast<int>( Playlist::Column::Title ), static_cast<int>( 150 * GetDPIScaling() ) /*width*/ } );
	}
	for ( const auto& iter : columns ) {
		ShowColumn( static_cast<Playlist::Column>( iter.ID ), iter.Width, true /*visible*/ );
	}

	SetColours();
	SetFont( logFont );
}

void WndList::SaveSettings()
{
	Settings::PlaylistColumns columnSettings;
	const HWND headerWnd = ListView_GetHeader( m_hWnd );
	if ( NULL != headerWnd ) {
		const int columnCount = Header_GetItemCount( headerWnd );
		std::vector<int> columnArray( columnCount );
		if ( FALSE != ListView_GetColumnOrderArray( m_hWnd, columnCount, &columnArray[ 0 ] ) ) {
			for ( const auto& iter : columnArray ) {
				LVCOLUMN lvc = {};
				lvc.mask = LVCF_SUBITEM | LVCF_WIDTH;
				if ( ( FALSE != ListView_GetColumn( m_hWnd, iter, &lvc ) ) && ( lvc.iSubItem > 0 ) ) {
					const Settings::PlaylistColumn column = { lvc.iSubItem, lvc.cx };
					columnSettings.push_back( column );
				}
			}
		}
	}

	const LOGFONT logFont = GetFont();
	const COLORREF fontColour = GetFontColour();
	const COLORREF backgroundColour = GetBackgroundColour();
	const COLORREF highlightColour = GetHighlightColour();
	const COLORREF statusIconColour = GetStatusIconColour();
	const bool showStatusIcon = GetStatusIconEnabled();
	m_Settings.SetPlaylistSettings( columnSettings, showStatusIcon, logFont, fontColour, backgroundColour, highlightColour, statusIconColour );
}

void WndList::ShowColumn( const Playlist::Column column, const int width, const bool show )
{
	const HWND headerWnd = ListView_GetHeader( m_hWnd );
	if ( NULL != headerWnd ) {
		bool columnShown = false;
		const int columnCount = Header_GetItemCount( headerWnd );

		bool validVisibilityChange = true;

		if ( !show && ( 2 == columnCount ) ) {
			// If the last column is about to be hidden, force the file path column to be shown.
			int filepathColumnWidth = 0;
			UINT filepathColumnID = 0;
			if ( const auto formatIter = s_ColumnFormats.find( Playlist::Column::Filepath ); s_ColumnFormats.end() != formatIter ) {
				filepathColumnWidth = formatIter->second.Width;
				filepathColumnID = formatIter->second.ShowID;

				std::set<UINT> shownColumns;
				std::set<UINT> hiddenColumns;
				GetColumnVisibility( shownColumns, hiddenColumns );
				if ( const auto filepathColumnIter = shownColumns.find( filepathColumnID ); shownColumns.end() == filepathColumnIter ) {
					ShowColumn( Playlist::Column::Filepath, filepathColumnWidth, true );
				} else {	
					// The file path column is already the only visible column, so do nothing.
					validVisibilityChange = false;
				}
			}
		}

		if ( validVisibilityChange ) {
			for ( int columnIndex = 1; columnIndex < columnCount; columnIndex++ ) {
				LVCOLUMN lvc = {};
				lvc.mask = LVCF_SUBITEM;
				if ( FALSE != ListView_GetColumn( m_hWnd, columnIndex, &lvc ) ) {
					const Playlist::Column columnID = static_cast<Playlist::Column>( lvc.iSubItem );
					if ( columnID == column ) {
						columnShown = true;
						if ( !show ) {
							ListView_DeleteColumn( m_hWnd, columnIndex );
						}
						break;
					}
				}
			}

			if ( show && !columnShown ) {
				const auto iter = s_ColumnFormats.find( column );
				if ( iter != s_ColumnFormats.end() ) {
					const ColumnFormat& columnFormat = iter->second;
					LVCOLUMN lvc = {};
					lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					lvc.cx = ( width > 0 ) ? width : columnFormat.Width;
					lvc.fmt = columnFormat.Alignment;
					lvc.iSubItem = static_cast<int>( column );
					const int bufSize = 32;
					WCHAR buffer[ bufSize ] = {};
					LoadString( m_hInst, columnFormat.HeaderID, buffer, bufSize );
					lvc.pszText = buffer;
					ListView_InsertColumn( m_hWnd, columnCount /*iCol*/, &lvc );
					RefreshListViewItemText();
					UpdateSortIndicator();

					// Force an update to show the horizontal scrollbar if necessary.
					ListView_Scroll( m_hWnd, 0, 0 );
				}
			}
		}
	}
}

void WndList::OnDropFiles( const HDROP hDrop )
{
	if ( nullptr != hDrop ) {
		const bool addToExisting = ( m_Playlist && ( ( Playlist::Type::User == m_Playlist->GetType() ) || ( Playlist::Type::All == m_Playlist->GetType() ) || ( Playlist::Type::Favourites == m_Playlist->GetType() ) ) );
		if ( !addToExisting ) {
			VUPlayer* vuplayer = VUPlayer::Get();
			if ( nullptr != vuplayer ) {
				m_Playlist = vuplayer->NewPlaylist();
			}
		}
		const int bufSize = 512;
		WCHAR filename[ bufSize ];
		const UINT fileCount = DragQueryFile( hDrop, 0xffffffff, nullptr /*filename*/, 0 /*bufSize*/ );
		for( UINT fileIndex = 0; fileIndex < fileCount; fileIndex++ ) {
			if ( 0 != DragQueryFile( hDrop, fileIndex, filename, bufSize ) ) {
				const DWORD attributes = GetFileAttributes( filename );
				if ( INVALID_FILE_ATTRIBUTES != attributes ) {
					if( attributes & FILE_ATTRIBUTE_DIRECTORY ) {
						AddFolderToPlaylist( filename );
					} else {
						AddFileToPlaylist( filename );
					}
				}
			}
		}
	}
}

void WndList::AddFileToPlaylist( const std::wstring& filename )
{
	if ( m_Playlist ) {
		if ( Playlist::IsSupportedPlaylist( filename ) ) {
			m_Playlist->AddPlaylist( filename );
		} else {		
			m_Playlist->AddPending( filename );
		}
	}
}

void WndList::AddFolderToPlaylist( const std::wstring& folder )
{
	WIN32_FIND_DATA findData;
	std::wstring str = folder;
	if ( !str.empty() && ( '\\' != str.back() ) ) {
		str += '\\';
	}
	str += L"*.*";
	const HANDLE handle = FindFirstFile( str.c_str(), &findData );
	if ( INVALID_HANDLE_VALUE != handle ) {
		BOOL found = TRUE;
		while ( found ) {
			if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
				if ( findData.cFileName[ 0 ] != '.' ) {
					str = folder;
					if ( !str.empty() && ( '\\' != str.back() ) ) {
						str += '\\';
					}
					str += findData.cFileName;
					AddFolderToPlaylist( str );
				}
			}
			else {
				str = folder;
				if ( !str.empty() && ( '\\' != str.back() ) ) {
					str += '\\';
				}
				str += findData.cFileName;
				AddFileToPlaylist( str );
			}
			found = FindNextFile( handle, &findData );
		}
		FindClose( handle );
	}	
}

void WndList::InsertListViewItem( const Playlist::Item& playlistItem, const int position )
{
	LVITEM item = {};
	item.mask = LVIF_PARAM | LVIF_IMAGE;
	item.iItem = ( position < 0 ) ? ListView_GetItemCount( m_hWnd ) : position;
	item.iSubItem = 0;
	item.iImage = -1;
	item.lParam = static_cast<LPARAM>( playlistItem.ID );
	item.iItem = ListView_InsertItem( m_hWnd, &item );
	if ( item.iItem >= 0 ) {
		if ( auto filename = m_FilenameToIDs.insert( FilenameToIDs::value_type( playlistItem.Info.GetFilename(), {} ) ).first; m_FilenameToIDs.end() != filename ) {
			filename->second.insert( playlistItem.ID );
		}
		SetListViewItemText( item.iItem, playlistItem );
	}
}

void WndList::DeleteListViewItem( const int itemIndex )
{
	ListView_DeleteItem( m_hWnd, itemIndex );
}

void WndList::SetListViewItemText( int itemIndex, const Playlist::Item& playlistItem )
{
	LVCOLUMN column = {};
	column.mask = LVCF_SUBITEM;
	int columnIndex = 0;
	const MediaInfo& mediaInfo = playlistItem.Info;
	while ( FALSE != ListView_GetColumn( m_hWnd, columnIndex, &column ) ) {
		const Playlist::Column columnID = static_cast<Playlist::Column>( column.iSubItem );
		switch( columnID ) {
			case Playlist::Column::Album : {
				const std::wstring str = mediaInfo.GetAlbum();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Artist : {
				const std::wstring str = mediaInfo.GetArtist();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Bitrate : {
				std::wstringstream ss;
				const auto bitrate = mediaInfo.GetBitrate( true /*calculate*/ );
				if ( bitrate.has_value() ) {
					const int bufSize = 16;
					WCHAR buf[ bufSize ] = {};
					if ( 0 != LoadString( m_hInst, IDS_UNITS_BITRATE, buf, bufSize ) ) {
						ss << std::to_wstring( std::lroundf( bitrate.value() ) ) << L" " << buf;
					}
				}
				const std::wstring str = ss.str();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::BitsPerSample : {
				const long bitsPerSample = mediaInfo.GetBitsPerSample().value_or( 0 );
				const std::wstring str = ( bitsPerSample > 0 ) ? std::to_wstring( bitsPerSample ) : std::wstring();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Channels : {
				const long channels = mediaInfo.GetChannels();
				const std::wstring str = ( channels > 0 ) ? std::to_wstring( channels ) : std::wstring();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Duration : {
				const float duration = mediaInfo.GetDuration();
				const std::wstring str = ( duration > 0 ) ? DurationToString( m_hInst, mediaInfo.GetDuration(), true /*colonDelimited*/ ) : std::wstring();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Filepath : {
				std::wstring str = mediaInfo.GetFilename();
				if ( !playlistItem.Duplicates.empty() ) {
					const int bufSize = 32;
					WCHAR buffer[ bufSize ] = {};
					LoadString( m_hInst, IDS_MULTIPLE_SOURCES, buffer, bufSize );
					str = L"(" + std::wstring( buffer ) + L")";
				}
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Filename : {
				const auto filename = std::filesystem::path( mediaInfo.GetFilename() ).filename();
				std::wstring str = filename.native();
				if ( !playlistItem.Duplicates.empty() ) {
					const int bufSize = 32;
					WCHAR buffer[ bufSize ] = {};
					LoadString( m_hInst, IDS_MULTIPLE_SOURCES, buffer, bufSize );
					str = L"(" + std::wstring( buffer ) + L")";
				}
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Filesize : {
				const std::wstring str = FilesizeToString( m_hInst, mediaInfo.GetFilesize() );
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Filetime : {
				std::wstringstream ss;
				const long long filetime = mediaInfo.GetFiletime();
				if ( filetime > 0 ) {
					FILETIME ft;
					ft.dwHighDateTime = static_cast<DWORD>( filetime >> 32 );
					ft.dwLowDateTime = static_cast<DWORD>( filetime & 0xffffffff );
					SYSTEMTIME st;
					if ( 0 !=	FileTimeToSystemTime( &ft, &st ) ) {
						SYSTEMTIME lt;
						if ( 0 != SystemTimeToTzSpecificLocalTime( NULL /*timeZone*/, &st, &lt ) ) {
							const int bufSize = 128;
							WCHAR dateBuf[ bufSize ] = {};
							WCHAR timeBuf[ bufSize ] = {};
							if ( ( 0 != GetDateFormat( LOCALE_USER_DEFAULT, DATE_SHORTDATE, &lt, NULL /*format*/, dateBuf, bufSize ) ) &&
									( 0 != GetTimeFormat( LOCALE_USER_DEFAULT, TIME_NOSECONDS, &lt, NULL /*format*/, timeBuf, bufSize ) ) ) {
								ss << dateBuf << L" " << timeBuf;
							}
						}
					}
				}
				const std::wstring str = ss.str();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::GainAlbum :
			case Playlist::Column::GainTrack : {
				std::wstringstream ss;
				const auto gain = ( Playlist::Column::GainAlbum == columnID ) ? mediaInfo.GetGainAlbum() : mediaInfo.GetGainTrack();
				if ( gain.has_value() ) {
					const int bufSize = 16;
					WCHAR buf[ bufSize ] = {};
					if ( 0 != LoadString( m_hInst, IDS_UNITS_DB, buf, bufSize ) ) {
						ss << std::fixed << std::setprecision( 2 ) << std::showpos << gain.value() << L" " << buf;
					}
				}
				const std::wstring str = ss.str();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Genre : {
				const std::wstring str = mediaInfo.GetGenre();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::SampleRate : {
				std::wstringstream ss;
				const long rate = mediaInfo.GetSampleRate();
				if ( rate > 0 ) {
					const int bufSize = 16;
					WCHAR buf[ bufSize ] = {};
					if ( 0 != LoadString( m_hInst, IDS_UNITS_HZ, buf, bufSize ) ) {
						ss << std::to_wstring( rate ) << L" " << buf;
					}
				}
				const std::wstring str = ss.str();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;				
			}
			case Playlist::Column::Title : {
				const std::wstring str = mediaInfo.GetTitle( true /*filenameAsTitle*/ );
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Track : {
				const long track = mediaInfo.GetTrack();
				const std::wstring str = ( track > 0 ) ? std::to_wstring( track ) : std::wstring();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Type : {
				std::wstring str = mediaInfo.GetType();
				if ( str.empty() && IsURL( mediaInfo.GetFilename() ) ) {
					const int bufSize = 16;
					WCHAR buf[ bufSize ] = {};
					if ( 0 != LoadString( m_hInst, IDS_TYPE_STREAM, buf, bufSize ) ) {
						str = buf;
					}
				}
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Version : {
				const std::wstring str = mediaInfo.GetVersion();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			case Playlist::Column::Year : {
				const long year = mediaInfo.GetYear();
				const std::wstring str = ( year > 0 ) ? std::to_wstring( year ) : std::wstring();
				ListView_SetItemText( m_hWnd, itemIndex, columnIndex, const_cast<LPWSTR>( str.c_str() ) );
				break;
			}
			default : {
				break;
			}
		}
		++columnIndex;
	}
	const std::wstring str = mediaInfo.GetTitle( true /*filenameAsTitle*/ );
	ListView_SetItemText( m_hWnd, itemIndex, 0 /*subItem*/, const_cast<LPWSTR>( str.c_str() ) );
}

void WndList::OnPlay( const long itemID )
{
	m_Output.Play( m_Playlist, itemID );
}

void WndList::PlaySelected()
{
	const Playlist::Item item = GetCurrentSelectedItem();
	if ( item.ID > 0 ) {
		OnPlay( item.ID );
	}
}

void WndList::OnContextMenu( const POINT& position )
{
	HMENU menu = LoadMenu( m_hInst, MAKEINTRESOURCE( IDR_MENU_LIST ) );
	if ( NULL != menu ) {
		HMENU listmenu = GetSubMenu( menu, 0 /*pos*/ );
		if ( NULL != listmenu ) {
			std::set<UINT> shownColumns;
			std::set<UINT> hiddenColumns;
			GetColumnVisibility( shownColumns, hiddenColumns );
			for ( const auto& hiddenColumn : hiddenColumns ) {
				CheckMenuItem( listmenu, hiddenColumn, MF_BYCOMMAND | MF_UNCHECKED );
			}
			for ( const auto& shownColumn : shownColumns ) {
				CheckMenuItem( listmenu, shownColumn, MF_BYCOMMAND | MF_CHECKED );
			}
			const UINT statusIconEnabled = GetStatusIconEnabled() ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem( listmenu, ID_SHOWCOLUMNS_STATUS, MF_BYCOMMAND | statusIconEnabled );

			if ( ( 1 == shownColumns.size() ) && ( *shownColumns.begin() == ID_SHOWCOLUMNS_FILEPATH ) ) {
				// If the only visible column is the file path column, disable the option to hide it.
				EnableMenuItem( listmenu, ID_SHOWCOLUMNS_FILEPATH, MF_BYCOMMAND | MF_DISABLED );
			} else {
				EnableMenuItem( listmenu, ID_SHOWCOLUMNS_FILEPATH, MF_BYCOMMAND | MF_ENABLED );
			}

			const bool hasItems = ( ListView_GetItemCount( m_hWnd ) > 0 );
			const bool hasSelectedItems = ( ListView_GetSelectedCount( m_hWnd ) > 0 );
			const bool allowPaste = ( m_Playlist && ( ( Playlist::Type::User == m_Playlist->GetType() ) || ( Playlist::Type::All == m_Playlist->GetType() ) || ( Playlist::Type::Favourites == m_Playlist->GetType() ) ) );
			const bool allowCut = ( m_Playlist && ( ( Playlist::Type::User == m_Playlist->GetType() ) || ( Playlist::Type::Favourites == m_Playlist->GetType() ) ) );
			const bool allowCopy = ( m_Playlist && ( Playlist::Type::CDDA != m_Playlist->GetType() ) );
			const bool allowAddStream = ( m_Playlist && ( ( Playlist::Type::Streams == m_Playlist->GetType() ) || ( Playlist::Type::User == m_Playlist->GetType() ) || ( Playlist::Type::All == m_Playlist->GetType() ) || ( Playlist::Type::Favourites == m_Playlist->GetType() ) ) );

			const UINT enablePaste = ( allowPaste && ( IsClipboardFormatAvailable( CF_TEXT ) || IsClipboardFormatAvailable( CF_UNICODETEXT ) || IsClipboardFormatAvailable( CF_HDROP ) ) ) ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_PASTE, MF_BYCOMMAND | enablePaste );

			const UINT enableCut = ( allowCut && hasSelectedItems ) ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_CUT, MF_BYCOMMAND | enableCut );

			const UINT enableCopy = ( allowCopy && hasSelectedItems ) ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_COPY, MF_BYCOMMAND | enableCopy );

			const UINT enableSelectAll = hasItems ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_SELECTALL, MF_BYCOMMAND | enableSelectAll );

			const UINT enableTrackInfo = hasSelectedItems ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_VIEW_TRACKINFORMATION, MF_BYCOMMAND | enableTrackInfo );

			const UINT enableAddFiles = allowPaste ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_PLAYLISTADDFOLDER, MF_BYCOMMAND | enableAddFiles );
			EnableMenuItem( listmenu, ID_FILE_PLAYLISTADDFILES, MF_BYCOMMAND | enableAddFiles );
			const UINT enableAddStream = allowAddStream ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_PLAYLISTADDSTREAM, MF_BYCOMMAND | enableAddStream );
			const UINT enableRemoveFiles = ( m_Playlist && hasSelectedItems && ( Playlist::Type::CDDA != m_Playlist->GetType() ) && ( Playlist::Type::Folder != m_Playlist->GetType() ) ) ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_PLAYLISTREMOVEFILES, MF_BYCOMMAND | enableRemoveFiles );
			const UINT enableAddToFavourites = ( m_Playlist && ( Playlist::Type::Favourites != m_Playlist->GetType() ) && ( Playlist::Type::CDDA != m_Playlist->GetType() ) && hasSelectedItems ) ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_ADDTOFAVOURITES, MF_BYCOMMAND | enableAddToFavourites );

			if ( m_Playlist && ( Playlist::Type::CDDA == m_Playlist->GetType() ) ) {		
				const int bufferSize = 64;
				WCHAR buffer[ bufferSize ] = {};
				LoadString( m_hInst, IDS_EXTRACT_TRACKS, buffer, bufferSize );
				ModifyMenu( menu, ID_FILE_CONVERT, MF_BYCOMMAND | MF_STRING, ID_FILE_CONVERT, buffer );
			}

			const UINT enableExtract = ( m_Playlist && m_Playlist->CanConvertAnyItems() ) ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_CONVERT, MF_BYCOMMAND | enableExtract );

			const UINT enableGainCalculator = hasSelectedItems ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_CALCULATEGAIN, MF_BYCOMMAND | enableGainCalculator );

			VUPlayer* vuplayer = VUPlayer::Get();

			const UINT musicbrainzEnabled = ( m_Playlist && ( Playlist::Type::CDDA == m_Playlist->GetType() ) && ( nullptr != vuplayer ) && vuplayer->IsMusicBrainzEnabled() ) ? MF_ENABLED : MF_DISABLED;
			EnableMenuItem( listmenu, ID_FILE_MUSICBRAINZ_QUERY, MF_BYCOMMAND | musicbrainzEnabled );

			const UINT enableColourChoice = IsHighContrastActive() ? MF_DISABLED : MF_ENABLED;
			EnableMenuItem( menu, ID_LISTMENU_FONTCOLOUR, MF_BYCOMMAND | enableColourChoice );
			EnableMenuItem( menu, ID_LISTMENU_BACKGROUNDCOLOUR, MF_BYCOMMAND | enableColourChoice );
			EnableMenuItem( menu, ID_LISTMENU_HIGHLIGHTCOLOUR, MF_BYCOMMAND | enableColourChoice );
			EnableMenuItem( menu, ID_LISTMENU_STATUSICONCOLOUR, MF_BYCOMMAND | enableColourChoice );

			if ( nullptr != vuplayer ) {
				vuplayer->InsertAddToPlaylists( listmenu, ID_FILE_ADDTOFAVOURITES, false /*addPrefix*/ );
			}

			const UINT flags = TPM_RIGHTBUTTON;
			TrackPopupMenu( listmenu, flags, position.x, position.y, 0 /*reserved*/, m_hWnd, NULL /*rect*/ );
		}
		DestroyMenu( menu );
	}
}

void WndList::OnCommand( const UINT command )
{
	switch ( command ) {
		case ID_SHOWCOLUMNS_ARTIST :
		case ID_SHOWCOLUMNS_ALBUM :
		case ID_SHOWCOLUMNS_GENRE :
		case ID_SHOWCOLUMNS_YEAR :
		case ID_SHOWCOLUMNS_TRACK :
		case ID_SHOWCOLUMNS_TYPE :
		case ID_SHOWCOLUMNS_VERSION :
		case ID_SHOWCOLUMNS_SAMPLERATE :
		case ID_SHOWCOLUMNS_CHANNELS :
		case ID_SHOWCOLUMNS_BITRATE :
		case ID_SHOWCOLUMNS_BITSPERSAMPLE :
		case ID_SHOWCOLUMNS_DURATION :
		case ID_SHOWCOLUMNS_FILESIZE :
		case ID_SHOWCOLUMNS_FILEPATH :
		case ID_SHOWCOLUMNS_FILENAME :
		case ID_SHOWCOLUMNS_FILETIME :
		case ID_SHOWCOLUMNS_TRACKGAIN :
		case ID_SHOWCOLUMNS_ALBUMGAIN : 
		case ID_SHOWCOLUMNS_STATUS : {
			OnShowColumn( command );
			break;
		}
		case ID_SORTPLAYLIST_ARTIST :
		case ID_SORTPLAYLIST_ALBUM :
		case ID_SORTPLAYLIST_GENRE :
		case ID_SORTPLAYLIST_YEAR :
		case ID_SORTPLAYLIST_TRACK :
		case ID_SORTPLAYLIST_TYPE :
		case ID_SORTPLAYLIST_VERSION :
		case ID_SORTPLAYLIST_SAMPLERATE :
		case ID_SORTPLAYLIST_CHANNELS :
		case ID_SORTPLAYLIST_BITRATE :
		case ID_SORTPLAYLIST_BITSPERSAMPLE :
		case ID_SORTPLAYLIST_DURATION :
		case ID_SORTPLAYLIST_FILESIZE :
		case ID_SORTPLAYLIST_FILEPATH :
		case ID_SORTPLAYLIST_FILENAME :
		case ID_SORTPLAYLIST_FILETIME :
		case ID_SORTPLAYLIST_TRACKGAIN :
		case ID_SORTPLAYLIST_ALBUMGAIN : {
			OnSortPlaylist( command );
			break;
		}
		case ID_LISTMENU_FONTSTYLE : {
			OnSelectFont();
			break;
		}
		case ID_LISTMENU_FONTCOLOUR : 
		case ID_LISTMENU_BACKGROUNDCOLOUR :
		case ID_LISTMENU_HIGHLIGHTCOLOUR : 
		case ID_LISTMENU_STATUSICONCOLOUR : {
			OnSelectColour( command );
			break;
		}
		case ID_FILE_SELECTALL : {
			OnSelectAll();
			break;
		}
		case ID_FILE_PLAYLISTADDSTREAM : {
			OnCommandAddStream();
			break;
		}
		case ID_FILE_PLAYLISTADDFOLDER : {
			OnCommandAddFolder();
			break;
		}
		case ID_FILE_PLAYLISTADDFILES : {
			OnCommandAddFiles();
			break;
		}
		case ID_FILE_PLAYLISTREMOVEFILES : {
			DeleteSelectedItems();
			break;
		}
		case ID_FILE_CUT : {
			OnCutCopy( true /*cut*/ );
			break;
		}
		case ID_FILE_COPY : {
			OnCutCopy( false /*cut*/ );
			break;
		}
		case ID_FILE_PASTE : {
			OnPaste();
			break;
		}
		default : {
			VUPlayer* vuplayer = VUPlayer::Get();
			if ( nullptr != vuplayer ) {
				vuplayer->OnCommand( command );
			}
			break;
		}
	}
}

bool WndList::IsColumnShown( const Playlist::Column& column ) const
{
	bool shown = false;
	const HWND headerWnd = ListView_GetHeader( m_hWnd );
	if ( NULL != headerWnd ) {
		const int columnCount = Header_GetItemCount( headerWnd );
		for ( int columnIndex = 1; !shown && ( columnIndex < columnCount ); columnIndex++ ) {
			LVCOLUMN lvc = {};
			lvc.mask = LVCF_SUBITEM;
			if ( FALSE != ListView_GetColumn( m_hWnd, columnIndex, &lvc ) ) {
				shown = ( static_cast<Playlist::Column>( lvc.iSubItem ) == column );
			}
		}
	}
	return shown;
}

void WndList::RefreshListViewItemText()
{
	if ( m_Playlist ) {
		const int itemCount = ListView_GetItemCount( m_hWnd );
		for ( int itemIndex = 0; itemIndex < itemCount; itemIndex++ ) {
			Playlist::Item item( { GetPlaylistItemID( itemIndex ), MediaInfo() } );
			if ( m_Playlist->GetItem( item ) ) {
				SetListViewItemText( itemIndex, item );
			}
		}
	}
}

void WndList::DeleteSelectedItems()
{
	if ( m_Playlist && ( Playlist::Type::Folder != m_Playlist->GetType() ) && ( Playlist::Type::CDDA != m_Playlist->GetType() ) ) {
		MediaInfo::List deletedMedia;

		SendMessage( m_hWnd, WM_SETREDRAW, FALSE, 0 );
		int itemIndex = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );
		const int selectItem = itemIndex;
		while ( itemIndex != -1 ) {
			Playlist::Item playlistItem = {};
			playlistItem.ID = GetPlaylistItemID( itemIndex );
			if ( m_Playlist->GetItem( playlistItem ) ) {
				deletedMedia.push_back( playlistItem.Info );
				for ( const auto& duplicate : playlistItem.Duplicates ) {
					MediaInfo mediaInfo( playlistItem.Info );
					mediaInfo.SetFilename( duplicate );
					deletedMedia.push_back( mediaInfo );
				}
				m_Playlist->RemoveItem( playlistItem );
				if ( auto filename = m_FilenameToIDs.find( playlistItem.Info.GetFilename() ); m_FilenameToIDs.end() != filename ) {
					filename->second.erase( playlistItem.ID );
					if ( filename->second.empty() ) {
						m_FilenameToIDs.erase( filename );
					}
				}
			}
			DeleteListViewItem( itemIndex );
			itemIndex = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );
		}
		ListView_SetItemState( m_hWnd, selectItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		SendMessage( m_hWnd, WM_SETREDRAW, TRUE, 0 );

		const Playlist::Type type = m_Playlist->GetType();
		switch ( type ) {
			case Playlist::Type::All :
			case Playlist::Type::Artist :
			case Playlist::Type::Album :
			case Playlist::Type::Genre :
			case Playlist::Type::Year : {
				VUPlayer* vuplayer = VUPlayer::Get();
				if ( nullptr != vuplayer ) {
					vuplayer->OnRemoveFromLibrary( deletedMedia );
				}
				break;
			}
			default : {
				break;
			}
		}
	}
}

void WndList::SetPlaylist( const Playlist::Ptr playlist, const bool initSelection, const std::wstring& filename )
{
	SendMessage( m_hWnd, WM_SETREDRAW, FALSE, 0 );
	ListView_DeleteAllItems( m_hWnd );
	m_FilenameToIDs.clear();
	m_IconStatus = {};
	m_FilenameToSelect = filename;
	if ( m_Playlist != playlist ) {
		m_Playlist = playlist;
	}
	if ( m_Playlist ) {
		int selectedIndex = -1;
		const Playlist::ItemList playlistItems = m_Playlist->GetItems();
		for ( const auto& iter : playlistItems ) {
			if ( ( iter.Info.GetFilename() == m_FilenameToSelect ) && ( -1 == selectedIndex ) ) {
				selectedIndex = ListView_GetItemCount( m_hWnd );
			}
			InsertListViewItem( iter );
		}
		if ( -1 != selectedIndex ) {
			ListView_SetItemState( m_hWnd, selectedIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
			ListView_EnsureVisible( m_hWnd, selectedIndex, FALSE /*partialOK*/ );	
			m_FilenameToSelect.clear();
		}
	}
	if ( initSelection ) {
		const int itemCount = ListView_GetItemCount( m_hWnd );
		if ( itemCount > 0 ) {
			int selectedIndex = 0;
			const long currentPlaying = m_Output.GetCurrentPlaying().PlaylistItem.ID;
			for ( int itemIndex = 0; itemIndex < itemCount; itemIndex++ ) {
				const long playlistID = GetPlaylistItemID( itemIndex );
				if ( playlistID == currentPlaying ) {
					selectedIndex = itemIndex;
					break;
				}
			}
			ListView_SetItemState( m_hWnd, selectedIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
			ListView_EnsureVisible( m_hWnd, selectedIndex, FALSE /*partialOK*/ );
		}
	}

	SendMessage( m_hWnd, WM_SETREDRAW, TRUE, 0 );
	RedrawWindow( m_hWnd, NULL /*rect*/, NULL /*rgn*/, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN );

	UpdateSortIndicator();
	UpdateStatusIcon();
}

void WndList::UpdateSortIndicator()
{
	Playlist::Column column = Playlist::Column::_Undefined;
	bool sortAscending = false;
	if ( m_Playlist ) {
		m_Playlist->GetSort( column, sortAscending );
	}
	const int sortColumn = ( Playlist::Column::_Undefined == column ) ? 0 : static_cast<int>( column );
	const HWND headerWnd = ListView_GetHeader( m_hWnd );
	if ( nullptr != headerWnd ) {
		const int itemCount = Header_GetItemCount( headerWnd );
		for ( int itemIndex = 0; itemIndex < itemCount; itemIndex++ ) {
			HDITEM headerItem = {};
			headerItem.mask = HDI_LPARAM | HDI_FORMAT;
			if ( ( TRUE == Header_GetItem( headerWnd, itemIndex, &headerItem ) ) && ( 0 != headerItem.lParam ) ) {
				const int previousFormat = headerItem.fmt;
				headerItem.fmt &= ~( HDF_SORTUP | HDF_SORTDOWN );
				if ( headerItem.lParam == sortColumn ) {
					headerItem.fmt |= sortAscending ? HDF_SORTUP : HDF_SORTDOWN;
				}
				if ( headerItem.fmt != previousFormat ) {
					headerItem.mask = HDI_FORMAT;
					Header_SetItem( headerWnd, itemIndex, &headerItem );
				}
			}
		}
	}
}

void WndList::OnFileAdded( Playlist* playlist, const Playlist::Item& item, const int position )
{
	if ( ( nullptr != playlist ) && ( m_Playlist.get() == playlist ) ) {
		AddedItem* addedItem = new AddedItem( { playlist, item, position } );
		PostMessage( m_hWnd, MSG_FILEADDED, reinterpret_cast<WPARAM>( addedItem ), 0 /*lParam*/ );
	}
}

void WndList::AddFileHandler( const AddedItem* addedItem )
{
	if ( nullptr != addedItem ) {
		if ( addedItem->Playlist == m_Playlist.get() ) {
			InsertListViewItem( addedItem->Item, addedItem->Position );

			int selectedIndex = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );

			if ( m_FilenameToSelect.empty() ) {
				if ( ( -1 == selectedIndex ) && ( 1 == ListView_GetItemCount( m_hWnd ) ) ) {
					ListView_SetItemState( m_hWnd, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
				}
			} else {
				if ( -1 == selectedIndex ) {
					if ( addedItem->Item.Info.GetFilename() == m_FilenameToSelect ) {
						ListView_SetItemState( m_hWnd, addedItem->Position, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
						ListView_EnsureVisible( m_hWnd, addedItem->Position, FALSE /*partialOK*/ );
						m_FilenameToSelect.clear();
					}
				} else {
					m_FilenameToSelect.clear();
				}
			}
		}
	}
}

void WndList::OnFileRemoved( Playlist* playlist, const Playlist::Item& item )
{
	if ( playlist == m_Playlist.get() ) {
		PostMessage( m_hWnd, MSG_FILEREMOVED, item.ID, 0 /*lParam*/ );
	}
}

void WndList::RemoveFileHandler( const long removedItemID )
{
	if ( const int itemIndex = FindItemIndex( removedItemID ); itemIndex >= 0 ) {
		DeleteListViewItem( itemIndex );
	}
}

void WndList::OnItemUpdated( Playlist* playlist, const Playlist::Item& item )
{
	if ( playlist == m_Playlist.get() ) {
		Playlist::Item* itemCopy = new Playlist::Item( item );
		PostMessage( m_hWnd, MSG_ITEMUPDATED, reinterpret_cast<WPARAM>( itemCopy ), 0 /*lParam*/ );
	}
}

void WndList::ItemUpdatedHandler( const Playlist::Item* item )
{
	if ( nullptr != item ) {
		if ( const int itemIndex = FindItemIndex( item->ID ); itemIndex >= 0 ) {
				SetListViewItemText( itemIndex, *item );
		}
	}
}

void WndList::SortPlaylist( const Playlist::Column column )
{
	if ( m_Playlist ) {
		m_Playlist->Sort( column );
		SetPlaylist( m_Playlist );
	}
}

COLORREF WndList::GetFontColour() const
{
	return m_ColourFont;
}

COLORREF WndList::GetBackgroundColour() const
{
	return m_ColourBackground;
}

COLORREF WndList::GetHighlightColour() const
{
	return m_ColourHighlight;
}

COLORREF WndList::GetStatusIconColour() const
{
	return m_ColourStatusIcon;
}

bool WndList::GetStatusIconEnabled() const
{
	return m_EnableStatusIcon;
}

void WndList::OnSelectFont()
{
	LOGFONT logFont = GetFont();

	CHOOSEFONT chooseFont = {};
	chooseFont.lStructSize = sizeof( CHOOSEFONT );
	chooseFont.hwndOwner = m_hWnd;
	chooseFont.Flags = CF_FORCEFONTEXIST | CF_NOVERTFONTS | CF_LIMITSIZE | CF_INITTOLOGFONTSTRUCT;
	chooseFont.nSizeMax = 36;
	chooseFont.lpLogFont = &logFont;

	if ( ( TRUE == ChooseFont( &chooseFont ) ) && ( nullptr != chooseFont.lpLogFont ) ) {
		SetFont( *chooseFont.lpLogFont );
	}
}

void WndList::OnSelectColour( const UINT commandID )
{
	COLORREF initialColour = 0;
	switch ( commandID ) {
		case ID_LISTMENU_FONTCOLOUR : {
			initialColour = GetFontColour();
			break;
		}
		case ID_LISTMENU_BACKGROUNDCOLOUR : {
			initialColour = GetBackgroundColour();
			break;
		}
		case ID_LISTMENU_HIGHLIGHTCOLOUR : {
			initialColour = GetHighlightColour();
			break;
		}
		case ID_LISTMENU_STATUSICONCOLOUR : {
			initialColour = GetStatusIconColour();
			break;
		}
		default : {
			break;
		}
	}

	VUPlayer* vuplayer = VUPlayer::Get();
	COLORREF* customColours = ( nullptr != vuplayer ) ? vuplayer->GetCustomColours() : nullptr;
	if ( const auto colour = ChooseColour( m_hWnd, initialColour, customColours ); colour.has_value() ) {
		switch ( commandID ) {
			case ID_LISTMENU_FONTCOLOUR : {
				m_ColourFont = colour.value();
				SetColours();
				break;
			}
			case ID_LISTMENU_BACKGROUNDCOLOUR : {
				m_ColourBackground = colour.value();
				SetColours();
				break;
			}
			case ID_LISTMENU_HIGHLIGHTCOLOUR : {
				m_ColourHighlight = colour.value();
				break;
			}
			case ID_LISTMENU_STATUSICONCOLOUR : {
				m_ColourStatusIcon = colour.value();
				CreateImageList();
				break;
			}
			default : {
				break;
			}
		}
		SaveSettings();
		RedrawWindow( m_hWnd, NULL /*rect*/, NULL /*rgn*/, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN );
	}
}

LOGFONT WndList::GetFont()
{
	LOGFONT logFont = {};
	const HFONT hFont = reinterpret_cast<HFONT>( SendMessage( m_hWnd, WM_GETFONT, 0, 0 ) );
	if ( nullptr != hFont ) {
		const int bufSize = GetObject( hFont, 0, NULL );
		if ( bufSize > 0 ) {
			std::vector<unsigned char> buf( bufSize );
			if ( bufSize == GetObject( hFont, bufSize, buf.data() ) ) {
				LOGFONT* currentLogFont = reinterpret_cast<LOGFONT*>( buf.data() );
				if ( nullptr != currentLogFont ) {
					logFont = *currentLogFont;
				}
			}
		}
	}
	return logFont;
}

void WndList::SetFont( const LOGFONT& logFont )
{
	const HFONT hNewFont = CreateFontIndirect( &logFont );
	SendMessage( m_hWnd, WM_SETFONT, reinterpret_cast<WPARAM>( hNewFont ), FALSE /*redraw*/ );
	RedrawWindow( m_hWnd, NULL /*rect*/, NULL /*rgn*/, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN );
	if ( nullptr != m_ChosenFont ) {
		DeleteObject( m_ChosenFont );
	}
	m_ChosenFont = hNewFont;
	CreateImageList();
}

BOOL WndList::OnBeginLabelEdit( const LVITEM& item )
{
	BOOL denyEdit = TRUE;

	LVHITTESTINFO lvh = {};
	GetCursorPos( &lvh.pt );
	ScreenToClient( m_hWnd, &lvh.pt );
	if ( ( ListView_SubItemHitTest( m_hWnd, &lvh ) >= 0 ) && ( lvh.iItem == item.iItem ) ) {
		LVCOLUMN lvc = {};
		lvc.mask = LVCF_SUBITEM;
		if ( TRUE == ListView_GetColumn( m_hWnd, lvh.iSubItem, &lvc ) ) {
			const Playlist::Column columnID = static_cast<Playlist::Column>( lvc.iSubItem );
			const auto columnIter = s_ColumnFormats.find( columnID );
			const bool denyTrackNumberEdit = ( Playlist::Column::Track == columnID ) && m_Playlist && ( Playlist::Type::CDDA == m_Playlist->GetType() );
			if ( ( s_ColumnFormats.end() != columnIter ) && ( columnIter->second.CanEdit ) && !denyTrackNumberEdit ) {
				if ( const HWND editControl = ListView_GetEditControl( m_hWnd ); nullptr != editControl ) {
					const int bufSize = 1024;
					WCHAR buf[ bufSize ] = {};
					ListView_GetItemText( m_hWnd, lvh.iItem, lvh.iSubItem, buf, bufSize );
					SetWindowText( editControl, buf );
					m_EditItem = lvh.iItem;
					m_EditSubItem = lvh.iSubItem;
					SetWindowLongPtr( editControl, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( this ) );
					m_EditControlWndProc = reinterpret_cast<WNDPROC>( SetWindowLongPtr( editControl, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( EditControlProc ) ) );
					denyEdit = FALSE;
				}
			}
		}		
	}
	return denyEdit;
}

void WndList::OnEndLabelEdit( const LVITEM& item )
{
	if ( nullptr != ListView_GetEditControl( m_hWnd ) )	{
		if ( nullptr != item.pszText ) {
			if ( m_Playlist ) {
				Playlist::Item playlistItem = {	static_cast<long>( item.lParam ), MediaInfo() };
				if ( m_Playlist->GetItem( playlistItem ) ) {
					m_Playlist->GetLibrary().GetMediaInfo( playlistItem.Info, false /*checkFileAttributes*/, false /*scanMedia*/ );
					MediaInfo previousMediaInfo( playlistItem.Info );
					LVCOLUMN lvc = {};
					lvc.mask = LVCF_SUBITEM;
					if ( FALSE != ListView_GetColumn( m_hWnd, m_EditSubItem, &lvc ) ) {
						Library::Column libraryColumnID = Library::Column::_Undefined;
						const Playlist::Column playlistColumnID = static_cast<Playlist::Column>( lvc.iSubItem );
						switch ( playlistColumnID ) {
							case Playlist::Column::Album : {
								libraryColumnID = Library::Column::Album;
								playlistItem.Info.SetAlbum( item.pszText );
								break;
							}
							case Playlist::Column::Artist : {
								libraryColumnID = Library::Column::Artist;
								playlistItem.Info.SetArtist( item.pszText );
								break;
							}
							case Playlist::Column::Genre : {
								libraryColumnID = Library::Column::Genre;
								playlistItem.Info.SetGenre( item.pszText );
								break;
							}
							case Playlist::Column::Title : {
								libraryColumnID = Library::Column::Title;
								playlistItem.Info.SetTitle( item.pszText );
								break;
							}
							case Playlist::Column::Track : {
								long tracknumber = 0;
								try {
									tracknumber = std::stol( item.pszText );
								} catch ( ... ) {
								}
								libraryColumnID = Library::Column::Track;
								playlistItem.Info.SetTrack( tracknumber );
								break;
							}
							case Playlist::Column::Year : {
								long year = 0;
								try {
									year = std::stol( item.pszText );
								} catch ( ... ) {
								}
								libraryColumnID = Library::Column::Year;
								playlistItem.Info.SetYear( year );
								break;
							}
							default : {
								break;
							}
						}
						if ( Library::Column::_Undefined != libraryColumnID ) {
							Library& library = m_Playlist->GetLibrary();
							MediaInfo updatedMediaInfo( playlistItem.Info );
							library.UpdateMediaTags( previousMediaInfo, updatedMediaInfo );
							for ( const auto& duplicate : playlistItem.Duplicates ) {
								previousMediaInfo.SetFilename( duplicate );
								updatedMediaInfo.SetFilename( duplicate );
								library.UpdateMediaTags( previousMediaInfo, updatedMediaInfo );
							}
						}
					}
				}
			}
		}
		m_EditItem = -1;
		m_EditSubItem = -1;
	}
}

void WndList::OnEndDragColumn()
{
	PostMessage( m_hWnd, MSG_REORDERDUMMY, 0 /*wParam*/, 0 /*lParam*/ );
}

void WndList::ReorderDummyColumn()
{
	const HWND headerWnd = ListView_GetHeader( m_hWnd );
	if ( NULL != headerWnd ) {
		const int columnCount = Header_GetItemCount( headerWnd );
		if ( columnCount > 1 ) {
			std::vector<int> columnArray( columnCount );
			if ( ( FALSE != ListView_GetColumnOrderArray( m_hWnd, columnCount, &columnArray[ 0 ] ) ) && ( columnArray[ 1 ] == 0 ) ) {
				columnArray[ 1 ] = columnArray[ 0 ];
				columnArray[ 0 ] = 0;
				ListView_SetColumnOrderArray( m_hWnd, columnCount, &columnArray[ 0 ] );
			}
		}
	}
}

WNDPROC WndList::GetEditControlWndProc()
{
	return m_EditControlWndProc;
}

void WndList::RepositionEditControl( WINDOWPOS* position )
{
	RECT rect = {};
	if ( 0 != ListView_GetSubItemRect( m_hWnd, m_EditItem, m_EditSubItem, LVIR_BOUNDS, &rect ) ) {
		position->x = ( rect.left < 0 ) ? 0 : rect.left;
		position->y = rect.top;
		position->cy = rect.bottom - rect.top;
		position->cx = rect.right - rect.left;
		GetClientRect( m_hWnd, &rect );
		if( ( position->x + position->cx ) > rect.right ) {
			position->cx = rect.right - position->x;
		}
	}
}

void WndList::OnUpdatedMedia( const MediaInfo& mediaInfo )
{
	if ( m_Playlist ) {
		if ( const auto itemFilename = m_FilenameToIDs.find( mediaInfo.GetFilename() ); m_FilenameToIDs.end() != itemFilename ) {
			const auto& itemIDs = itemFilename->second;
			for ( const auto itemID : itemIDs ) {
				if ( Playlist::Item item = { itemID }; m_Playlist->GetItem( item ) && ( item.Info.GetFilename() == itemFilename->first ) ) {
					if ( const int itemIndex = FindItemIndex( itemID ); itemIndex >= 0 ) {
						const Playlist::Item playlistItem = { itemID, mediaInfo };
						SetListViewItemText( itemIndex, playlistItem );
					}
				}
			}
		}
	}
}

long WndList::GetPlaylistItemID( const int itemIndex )
{
	LVITEM item = {};
	item.iItem = itemIndex;
	item.mask = LVIF_PARAM;
	const long playlistItemID = ( TRUE == ListView_GetItem( m_hWnd, &item ) ) ? static_cast<long>( item.lParam ) : 0;
	return playlistItemID;
}

Playlist::Ptr WndList::GetPlaylist()
{
	return m_Playlist;
}

Playlist::Item WndList::GetCurrentSelectedItem()
{
	Playlist::Item item = {};
	const int itemIndex = GetCurrentSelectedIndex();
	if ( itemIndex >= 0 ) {
		item = { GetPlaylistItemID( itemIndex ), MediaInfo() };
		if ( m_Playlist ) { 
			m_Playlist->GetItem( item );
		}
	}
	return item;
}

int WndList::GetCurrentSelectedIndex()
{
	int itemIndex = -1;
	if ( ListView_GetSelectedCount( m_hWnd ) > 0 ) {
		itemIndex = static_cast<int>( ListView_GetNextItem( m_hWnd, -1 /*start*/, LVNI_ALL | LVNI_FOCUSED | LVNI_SELECTED ) );
		if ( -1 == itemIndex ) {
			itemIndex = static_cast<int>( ListView_GetNextItem( m_hWnd, -1 /*start*/, LVNI_ALL | LVNI_SELECTED ) );
		}
	}
	return itemIndex;
}

void WndList::OnBeginDrag( const int itemIndex )
{
	SetCapture( m_hWnd );
	m_OldCursor = SetCursor( LoadCursor( NULL /*inst*/, IDC_HAND ) );
	m_IsDragging = true;
	const POINT point = {};
	m_DragImage = ListView_CreateDragImage( m_hWnd, itemIndex, &point );
	SetTimer( m_hWnd, s_DragTimerID, s_DragTimerInterval, NULL /*timerFunc*/ );
}

void WndList::OnEndDrag()
{
	if ( m_IsDragging ) {
		m_IsDragging = false;
		KillTimer( m_hWnd, s_DragTimerID );
		if ( nullptr != m_DragImage ) {
			ImageList_Destroy( m_DragImage );
			m_DragImage = m_DragImage;
		}
		if ( nullptr != m_OldCursor ) {
			SetCursor( m_OldCursor );
			m_OldCursor = nullptr;
		}
		LVINSERTMARK lvim = {};
		lvim.cbSize = sizeof( LVINSERTMARK );
		ListView_GetInsertMark( m_hWnd, &lvim );
		int insertionIndex = lvim.iItem;
		if ( insertionIndex >= 0 ) {
			if ( LVIM_AFTER & lvim.dwFlags ) {
				++insertionIndex;
			}
			MoveSelectedItems( insertionIndex );
		}
		lvim.iItem = -1;
		ListView_SetInsertMark( m_hWnd, &lvim );
	}
}

void WndList::OnDrag( const POINT& point )
{
	if ( m_IsDragging ) {
		LVHITTESTINFO hittest = {};
		hittest.pt = point;
		const int itemIndex = ListView_HitTest( m_hWnd, &hittest );
		LVINSERTMARK lvim = {};
		lvim.cbSize = sizeof( LVINSERTMARK );
		lvim.iItem = -1;
		if ( -1 != itemIndex ) {
			lvim.iItem = hittest.iItem;
			const int itemCount = ListView_GetItemCount( m_hWnd );
			if ( 1 + lvim.iItem == itemCount ) {
				RECT itemRect = {};
				ListView_GetItemRect( m_hWnd, lvim.iItem, &itemRect, LVIR_BOUNDS );
				if ( static_cast<int>( hittest.pt.y ) > ( ( itemRect.top + itemRect.bottom ) / 2 ) ) {
					lvim.dwFlags = LVIM_AFTER;
				}
			}
		}
		ListView_SetInsertMark( m_hWnd, &lvim );
	}
}

void WndList::OnDragTimer()
{
	POINT pt = {};
	GetCursorPos( &pt );
	RECT listRect = {};
	GetWindowRect( m_hWnd, &listRect );
	if ( ( pt.y < listRect.top ) || ( pt.y > listRect.bottom ) ) {
		RECT itemRect = {};
		ListView_GetItemRect( m_hWnd, 0, &itemRect, LVIR_BOUNDS );
		const int itemHeight = itemRect.bottom - itemRect.top;
		const int dy = ( pt.y < listRect.top ) ? ( pt.y - listRect.top - itemHeight / 2 ) : ( pt.y - listRect.bottom + itemHeight / 2 );
		LVINSERTMARK lvim = {};
		lvim.cbSize = sizeof( LVINSERTMARK );
		lvim.iItem = -1;
		ListView_SetInsertMark( m_hWnd, &lvim );
		ListView_Scroll( m_hWnd, 0 /*dx*/, dy );	
	}
}

void WndList::MoveSelectedItems( const int insertionIndex )
{
	if ( m_Playlist ) {
		int itemIndex = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );
		std::list<long> itemsToMove;
		while ( itemIndex >= 0 ) {
			const long itemID = GetPlaylistItemID( itemIndex );
			itemsToMove.push_back( itemID );
			itemIndex = ListView_GetNextItem( m_hWnd, itemIndex, LVNI_SELECTED );
		}
		if ( m_Playlist->MoveItems( insertionIndex, itemsToMove ) ) {
			RECT rect = {};
			ListView_GetItemRect( m_hWnd, 0, &rect, LVIR_BOUNDS );
			const int itemHeight = rect.bottom - rect.top;
			GetClientRect( m_hWnd, &rect );
			LVHITTESTINFO hittest = {};
			hittest.pt = { 0 /*x*/, rect.bottom - itemHeight /*y*/ };
			const int lastVisibleItem = ListView_HitTest( m_hWnd, &hittest );

			SetPlaylist( m_Playlist, false /*initSelection*/ );
			const int itemCount = ListView_GetItemCount( m_hWnd );
			for ( itemIndex = 0; itemIndex < itemCount; itemIndex++ ) {
				const long itemID = GetPlaylistItemID( itemIndex );
				const bool movedItem = ( itemsToMove.end() != std::find( itemsToMove.begin(), itemsToMove.end(), itemID ) );
				if ( movedItem ) {
					ListView_SetItemState( m_hWnd, itemIndex, LVIS_SELECTED, LVIS_SELECTED );
				}
			}
			const int firstSelectedItem = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );
			if ( firstSelectedItem >= 0 ) {
				ListView_SetItemState( m_hWnd, firstSelectedItem, LVIS_FOCUSED, LVIS_FOCUSED );
				ListView_EnsureVisible( m_hWnd, ( lastVisibleItem >= 0 ) ? lastVisibleItem : firstSelectedItem, FALSE /*partialOK*/ );
			}
		}
	}
}

void WndList::SelectPreviousItem()
{
	if ( m_Playlist ) {
		const Playlist::Item currentItem = GetCurrentSelectedItem();
		Playlist::Item previousItem = {};
		if ( m_Playlist->GetPreviousItem( currentItem, previousItem ) ) {
			// Deselect all items.
			int selectedItem = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );
			while ( selectedItem >= 0 ) {
				ListView_SetItemState( m_hWnd, selectedItem, 0, LVIS_SELECTED | LVIS_FOCUSED );
				selectedItem = ListView_GetNextItem( m_hWnd, selectedItem, LVNI_SELECTED );
			}
			// Select previous item.
			if ( const int itemIndex = FindItemIndex( previousItem.ID ); itemIndex >= 0 ) {
				ListView_SetItemState( m_hWnd, itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
				ListView_EnsureVisible( m_hWnd, itemIndex, FALSE /*partialOK*/ );
			}
		}
	}
}

void WndList::SelectNextItem()
{
	if ( m_Playlist ) {
		const Playlist::Item currentItem = GetCurrentSelectedItem();
		Playlist::Item nextItem = {};
		if ( m_Playlist->GetNextItem( currentItem, nextItem ) ) {
			// Deselect all items.
			int selectedItem = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );
			while ( selectedItem >= 0 ) {
				ListView_SetItemState( m_hWnd, selectedItem, 0, LVIS_SELECTED | LVIS_FOCUSED );
				selectedItem = ListView_GetNextItem( m_hWnd, selectedItem, LVNI_SELECTED );
			}
			// Select next item.
			if ( const int itemIndex = FindItemIndex( nextItem.ID ); itemIndex >= 0 ) {
				ListView_SetItemState( m_hWnd, itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
				ListView_EnsureVisible( m_hWnd, itemIndex, FALSE /*partialOK*/ );
			}
		}
	}
}

void WndList::EnsureVisible( const Playlist::Item& item, const bool select )
{
	if ( const int itemIndex = FindItemIndex( item.ID ); itemIndex >= 0 ) {
		ListView_EnsureVisible( m_hWnd, itemIndex, FALSE /*partialOK*/ );
		if ( select ) {
			int selectedItem = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );
			while ( selectedItem >= 0 ) {
				ListView_SetItemState( m_hWnd, selectedItem, 0, LVIS_SELECTED | LVIS_FOCUSED );
				selectedItem = ListView_GetNextItem( m_hWnd, selectedItem, LVNI_SELECTED );
			}
			ListView_SetItemState( m_hWnd, itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

Playlist::ItemList WndList::GetSelectedPlaylistItems()
{
	Playlist::ItemList items;
	int itemIndex = ListView_GetNextItem( m_hWnd, -1, LVNI_SELECTED );
	while ( itemIndex >= 0 ) {
		const long itemID = GetPlaylistItemID( itemIndex );
		Playlist::Item item = { itemID, {} };
		if ( m_Playlist->GetItem( item ) ) {
			items.push_back( item );
		}
		itemIndex = ListView_GetNextItem( m_hWnd, itemIndex, LVNI_SELECTED );
	}
	return items;
}

int WndList::GetSelectedCount() const
{
	const int count = ListView_GetSelectedCount( m_hWnd );
	return count;
}

int WndList::GetCount() const
{
	const int count = ListView_GetItemCount( m_hWnd );
	return count;
}

void WndList::OnSelectAll()
{
	if ( const HWND editControl = ListView_GetEditControl( m_hWnd ); nullptr != editControl ) {
		Edit_SetSel( editControl, 0, -1 );
	} else {
		const int itemCount = ListView_GetItemCount( m_hWnd );
		SendMessage( m_hWnd, WM_SETREDRAW, FALSE, 0 );
		for ( int itemIndex = 0; itemIndex < itemCount; itemIndex++ ) {
			ListView_SetItemState( m_hWnd, itemIndex, LVIS_SELECTED, LVIS_SELECTED );
		}
		SendMessage( m_hWnd, WM_SETREDRAW, TRUE, 0 );
	}
}

void WndList::OnCommandAddFolder()
{
	WCHAR title[ MAX_PATH ] = {};
	LoadString( m_hInst, IDS_ADDFOLDERTOPLAYLIST_TITLE, title, MAX_PATH );

	WCHAR buffer[ MAX_PATH ] = {};
	BROWSEINFO bi = {};
	bi.hwndOwner = m_hWnd;
	bi.lpszTitle = title;
	bi.pszDisplayName = buffer;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
	LPITEMIDLIST idlist = SHBrowseForFolder( &bi );
	if ( nullptr != idlist ) {
		const DWORD pathSize = 1024;
		WCHAR path[ pathSize ] = {};
		if ( TRUE == SHGetPathFromIDListEx( idlist, path, pathSize, GPFIDL_DEFAULT ) ) {
			const bool addToExisting = ( m_Playlist && ( ( Playlist::Type::User == m_Playlist->GetType() ) || ( Playlist::Type::All == m_Playlist->GetType() ) || ( Playlist::Type::Favourites == m_Playlist->GetType() ) ) );
			if ( !addToExisting ) {
				VUPlayer* vuplayer = VUPlayer::Get();
				if ( nullptr != vuplayer ) {
					m_Playlist = vuplayer->NewPlaylist();
				}
			}
			AddFolderToPlaylist( path );
		}
		CoTaskMemFree( idlist );
	}
}

void WndList::OnCommandAddFiles()
{
	WCHAR title[ MAX_PATH ] = {};
	LoadString( m_hInst, IDS_ADDFILES_TITLE, title, MAX_PATH );

	WCHAR filter[ MAX_PATH ] = {};
	LoadString( m_hInst, IDS_ADDFILES_FILTERAUDIO, filter, MAX_PATH );
	const std::wstring filter1( filter );
	std::set<std::wstring> fileTypes = m_Output.GetAllSupportedFileExtensions();

	std::set<std::wstring> playlistTypes = Playlist::GetSupportedPlaylistExtensions();
	fileTypes.merge( playlistTypes );

	std::wstring filter2;
	for ( const auto& iter : fileTypes ) {
		filter2 += L"*." + iter + L";";
	}
	if ( !filter2.empty() ) {
		filter2.pop_back();
	}
	LoadString( m_hInst, IDS_CHOOSE_FILTERALL, filter, MAX_PATH );
	const std::wstring filter3( filter );
	const std::wstring filter4( L"*.*" );
	std::vector<WCHAR> filterStr;
	filterStr.reserve( MAX_PATH );
	filterStr.insert( filterStr.end(), filter1.begin(), filter1.end() );
	filterStr.push_back( 0 );
	filterStr.insert( filterStr.end(), filter2.begin(), filter2.end() );
	filterStr.push_back( 0 );
	filterStr.insert( filterStr.end(), filter3.begin(), filter3.end() );
	filterStr.push_back( 0 );
	filterStr.insert( filterStr.end(), filter4.begin(), filter4.end() );
	filterStr.push_back( 0 );
	filterStr.push_back( 0 );

	const std::string initialFolderSetting = "AddFiles";
	const std::wstring initialFolder = m_Settings.GetLastFolder( initialFolderSetting );

	const DWORD bufferSize = 32768;
	WCHAR buffer[ bufferSize ] = {};
	OPENFILENAME ofn = {};
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrTitle = title;
	ofn.lpstrFilter = &filterStr[ 0 ];
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = bufferSize;
	ofn.lpstrInitialDir = initialFolder.empty() ? nullptr : initialFolder.c_str();
	if ( ( FALSE != GetOpenFileName( &ofn ) ) && ( ofn.nFileOffset > 0 ) ) {
		std::wstring foldername( ofn.lpstrFile, ofn.nFileOffset - 1 );
		foldername += L"\\";
		WCHAR* filenameOffset = ofn.lpstrFile + ofn.nFileOffset;
		while ( 0 != *filenameOffset ) {
			const bool addToExisting = ( m_Playlist && ( ( Playlist::Type::User == m_Playlist->GetType() ) || ( Playlist::Type::All == m_Playlist->GetType() ) || ( Playlist::Type::Favourites == m_Playlist->GetType() ) ) );
			if ( !addToExisting ) {
				VUPlayer* vuplayer = VUPlayer::Get();
				if ( nullptr != vuplayer ) {
					m_Playlist = vuplayer->NewPlaylist();
				}
			}
			const std::wstring filename = foldername + filenameOffset;
			AddFileToPlaylist( filename );
			filenameOffset += wcslen( filenameOffset ) + 1;
		}
		m_Settings.SetLastFolder( initialFolderSetting, std::wstring( ofn.lpstrFile ).substr( 0, ofn.nFileOffset ) );
	}
}

void WndList::OnCommandAddStream()
{
	const DlgAddStream dlg( m_hInst, m_hWnd );
	const std::wstring& url = dlg.GetURL();
	if ( !url.empty() ) {
		const auto decoder = IsURL( url ) ? m_Output.GetHandlers().OpenDecoder( url ) : nullptr;
		if ( decoder ) {
			const bool addToExisting = ( m_Playlist && ( ( Playlist::Type::Streams == m_Playlist->GetType() ) || ( Playlist::Type::User == m_Playlist->GetType() ) || ( Playlist::Type::All == m_Playlist->GetType() ) || ( Playlist::Type::Favourites == m_Playlist->GetType() ) ) );
			if ( !addToExisting ) {
				VUPlayer* vuplayer = VUPlayer::Get();
				if ( nullptr != vuplayer ) {
					m_Playlist = vuplayer->SelectStreamsPlaylist();
				}
			}
			if ( m_Playlist ) {
				m_Playlist->AddPending( url );
			}
		} else {
			const int bufferSize = 256;
			WCHAR buffer[ bufferSize ] = {};
			LoadString( m_hInst, IDS_ADDSTREAM_ERROR_CAPTION, buffer, bufferSize );
			const std::wstring caption = buffer;
			LoadString( m_hInst, IDS_ADDSTREAM_ERROR_TEXT, buffer, bufferSize );
			std::wstring text = buffer + url;
			MessageBox( m_hWnd, text.c_str(), caption.c_str(), MB_OK | MB_ICONWARNING );
		}
	}
}

void WndList::OnCutCopy( const bool cut )
{
	std::wstring clipboardText;
	bool haveClipboardText = false;
	if ( const HWND editControl = ListView_GetEditControl( m_hWnd ); nullptr != editControl ) {
		const int textLength = Edit_GetTextLength( editControl );
		if ( textLength > 0 ) {
			std::vector<WCHAR> text( 1 + textLength, 0 );
			if ( Edit_GetText( editControl, text.data(), 1 + textLength ) >= textLength ) {
				DWORD startSel = 0;
				DWORD endSel = 0;
				if ( ( SendMessage( editControl, EM_GETSEL, WPARAM( &startSel ), LPARAM ( &endSel ) ) > 0 ) && ( endSel > startSel ) && ( startSel < text.size() ) && ( endSel < text.size() ) ) {
					clipboardText = std::wstring( text.begin() + startSel, text.begin() + endSel );
					haveClipboardText = true;
					if ( cut ) {
						Edit_ReplaceSel( editControl, L"" );
					}
				}
			}
		}
	} else {
		const Playlist::ItemList items = GetSelectedPlaylistItems();
		if ( !items.empty() ) {
			std::wstringstream stream;
			for ( const auto& item : items ) {
				stream << item.Info.GetFilename() << L"\r\n";
			}
			clipboardText = stream.str();
			haveClipboardText = true;
			if ( cut ) {
				DeleteSelectedItems();
			}
		}
	}

	if ( haveClipboardText ) {
		if ( OpenClipboard( m_hWnd ) ) {
			if ( const HGLOBAL mem = GlobalAlloc( GMEM_MOVEABLE, ( clipboardText.size() + 1 ) * sizeof( WCHAR ) ); nullptr != mem ) {
				if ( LPWSTR destString = static_cast<LPWSTR>( GlobalLock( mem ) ); nullptr != destString ) {
					wcscpy_s( destString, clipboardText.size() + 1, clipboardText.c_str() );			
					GlobalUnlock( mem );
					EmptyClipboard();
					SetClipboardData( CF_UNICODETEXT, mem );
				} else {
					GlobalFree( mem );
				}
			}
			CloseClipboard();
		}
	}

}

void WndList::OnPaste()
{
	if ( OpenClipboard( m_hWnd ) ) {
		bool clipboardHasText = false;
		std::wstring clipboardText;
		HANDLE handle = GetClipboardData( CF_UNICODETEXT );
		if ( nullptr != handle ) {
			LPCWSTR str = static_cast<LPCWSTR>( GlobalLock( handle ) );
			if ( nullptr != str ) {
				clipboardText = str;
				GlobalUnlock( handle );
				clipboardHasText = true;
			}	
		} else {
			handle = GetClipboardData( CF_TEXT );
			if ( nullptr != handle ) {
				LPCSTR str = static_cast<LPCSTR>( GlobalLock( handle ) );
				if ( nullptr != str ) {
					clipboardText = AnsiCodePageToWideString( str );
					GlobalUnlock( handle );
					clipboardHasText = true;
				}	
			} else {
				handle = GetClipboardData( CF_HDROP );
				if ( nullptr != handle ) {
					if ( const HWND editControl = ListView_GetEditControl( m_hWnd ); nullptr == editControl ) {
						OnDropFiles( HDROP( handle ) );
					}
				}
			}
		}
		CloseClipboard();

		if ( clipboardHasText ) {
			if ( const HWND editControl = ListView_GetEditControl( m_hWnd ); nullptr != editControl ) {
				Edit_ReplaceSel( editControl, clipboardText.c_str() );
			} else {
				try {
					std::wstringstream stream( clipboardText );
					do {
						std::wstring filename;
						std::getline( stream, filename );
						if ( !filename.empty() ) {
							if ( filename.back() == '\r' ) {
								filename.pop_back();
							}
							AddFileToPlaylist( filename );				
						}
					} while ( !stream.eof() );
				} catch ( ... ) {
				}
			}
		}
	}
}

void WndList::OnShowColumn( const UINT command )
{
	if ( ID_SHOWCOLUMNS_STATUS == command ) {
		m_EnableStatusIcon = !m_EnableStatusIcon;
		ShowStatusIconColumn();
	} else {
		for ( const auto& iter : s_ColumnFormats ) {
			const ColumnFormat& columnFormat = iter.second;
			if ( columnFormat.ShowID == command ) {
				const Playlist::Column columnID = iter.first;
				ShowColumn( columnID, columnFormat.Width, !IsColumnShown( columnID ) );
				break;
			}
		}
	}
}

void WndList::OnSortPlaylist( const UINT command )
{
	for ( const auto& iter : s_ColumnFormats ) {
		const ColumnFormat& columnFormat = iter.second;
		if ( columnFormat.SortID == command ) {
			const Playlist::Column columnID = iter.first;
			SortPlaylist( columnID );
			break;
		}
	}
}

void WndList::GetColumnVisibility( std::set<UINT>& shown, std::set<UINT>& hidden )
{
	shown.clear();
	hidden.clear();
	for ( const auto& columnIter : s_ColumnFormats ) {
		hidden.insert( columnIter.second.ShowID );
	}
	const HWND headerWnd = ListView_GetHeader( m_hWnd );
	if ( NULL != headerWnd ) {
		const int columnCount = Header_GetItemCount( headerWnd );
		for ( int columnIndex = 1; columnIndex < columnCount; columnIndex++ ) {
			LVCOLUMN lvc = {};
			lvc.mask = LVCF_SUBITEM;
			if ( FALSE != ListView_GetColumn( m_hWnd, columnIndex, &lvc ) ) {
				const Playlist::Column columnID = static_cast<Playlist::Column>( lvc.iSubItem );
				const auto columnFormatIter = s_ColumnFormats.find( columnID );
				if ( columnFormatIter != s_ColumnFormats.end() ) {
					const UINT cmdShowID = columnFormatIter->second.ShowID;
					hidden.erase( cmdShowID );
					shown.insert( cmdShowID );
				}		
			}
		}
	}
}

void WndList::SelectPlaylistItem( const long itemID )
{
	if ( const int selectedIndex = FindItemIndex( itemID ); selectedIndex >= 0 ) {
		const int itemCount = ListView_GetItemCount( m_hWnd );
		for ( int itemIndex = 0; itemIndex < itemCount; itemIndex++ ) {
			if ( itemIndex == selectedIndex ) {
				ListView_SetItemState( m_hWnd, itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
				ListView_EnsureVisible( m_hWnd, itemIndex, FALSE /*partialOK*/ );
			} else {
				ListView_SetItemState( m_hWnd, itemIndex, 0, LVIS_SELECTED | LVIS_FOCUSED );
			}
		}
	}
}

void WndList::CreateImageList()
{
	const int iconSize = GetStatusIconSize();
	const int imageCount = 1;
	if ( HIMAGELIST imageList = ImageList_Create( iconSize, iconSize, ILC_COLOR32, 0 /*initial*/, imageCount /*grow*/ ); nullptr != imageList ) {
		const COLORREF colour = m_IsHighContrast ? GetSysColor( COLOR_HIGHLIGHT ) : GetStatusIconColour();
		if ( const HBITMAP hBitmap = CreateColourBitmap( m_hInst, IDI_VOLUME, iconSize, colour ); nullptr != hBitmap ) {
			m_IconMap.insert( IconMap::value_type( Output::State::Playing, ImageList_Add( imageList, hBitmap, nullptr /*mask*/ ) ) );
			DeleteObject( hBitmap );
		}
		if ( const HBITMAP hBitmap = CreateColourBitmap( m_hInst, IDI_PAUSE, iconSize, colour ); nullptr != hBitmap ) {
			m_IconMap.insert( IconMap::value_type( Output::State::Paused, ImageList_Add( imageList, hBitmap, nullptr /*mask*/ ) ) );
			DeleteObject( hBitmap );
		}
		if ( const HIMAGELIST previousImageList = ListView_SetImageList( m_hWnd, imageList, LVSIL_SMALL ); nullptr != previousImageList ) {
			ImageList_Destroy( previousImageList );
		}
		ShowStatusIconColumn();
	}
}

void WndList::UpdateStatusIcon()
{
	const auto [ outputItemID, outputItemState ] = std::make_pair( m_Output.GetCurrentPlaying().PlaylistItem.ID, m_Output.GetState() );
	auto& [ iconItemID, iconItemState ] = m_IconStatus;
	if ( ( iconItemState != outputItemState ) || ( iconItemID != outputItemID ) ) {
		if ( iconItemID != outputItemID ) {
			if ( const int itemIndex = FindItemIndex( iconItemID ); itemIndex >= 0 ) {
				LVITEM item = {};
				item.mask = LVIF_IMAGE;
				item.iItem = itemIndex;
				item.iImage = -1;
				ListView_SetItem( m_hWnd, &item );
			}
		}

		const auto iconIter = m_IconMap.find( outputItemState );
		const int imageIndex = ( m_IconMap.end() != iconIter ) ? iconIter->second : -1;
		if ( const int itemIndex = FindItemIndex( outputItemID ); itemIndex >= 0 ) {
			LVITEM item = {};
			item.mask = LVIF_IMAGE;
			item.iItem = itemIndex;
			item.iImage = imageIndex;
			ListView_SetItem( m_hWnd, &item );
		}

		iconItemID = outputItemID;
		iconItemState = outputItemState;
	}
}

void WndList::ShowStatusIconColumn()
{
	const int iconBorder = static_cast<int>( 4 * GetDPIScaling() );
	const int width = GetStatusIconEnabled() ? ( GetStatusIconSize() + iconBorder ) : 0;
	const int currentWidth = ListView_GetColumnWidth( m_hWnd, 0 );
	if ( currentWidth != width ) {
		ListView_SetColumnWidth( m_hWnd, 0, width );
	}
}

int WndList::GetStatusIconSize() const
{
	int iconSize = static_cast<int>( 16 * GetDPIScaling() );
	constexpr int iconModulus = 4;
	if ( HDC dc = GetDC( m_hWnd ); nullptr != dc ) {
		const Gdiplus::Font font( dc, m_ChosenFont );
		const Gdiplus::Graphics graphics( dc );
		const Gdiplus::PointF origin;
		Gdiplus::RectF bounds;
		if ( Gdiplus::Ok == graphics.MeasureString( L"Ay96", -1, &font, origin, &bounds ) ) {
			iconSize = std::max<int>( 16, iconModulus * ( static_cast<int>( bounds.Height ) / iconModulus ) );
		}
		ReleaseDC( m_hWnd, dc );
	}
	return iconSize;
}

int WndList::FindItemIndex( const long itemID )
{
	int itemIndex = -1;
	const int itemCount = ListView_GetItemCount( m_hWnd );
	for ( int i = 0; i < itemCount; i++ ) {
		if ( itemID == GetPlaylistItemID( i ) ) {
			itemIndex = i;
			break;
		}
	}
	return itemIndex;
}

void WndList::SetColours()
{
	if ( m_IsHighContrast ) {
		ListView_SetTextColor( m_hWnd, GetSysColor( COLOR_WINDOWTEXT ) );
		ListView_SetTextBkColor( m_hWnd, GetSysColor( COLOR_WINDOW ) );
		ListView_SetBkColor( m_hWnd, GetSysColor( COLOR_WINDOW ) );	
	} else {
		ListView_SetTextColor( m_hWnd, m_ColourFont );
		ListView_SetTextBkColor( m_hWnd, m_ColourBackground );
		ListView_SetBkColor( m_hWnd, m_ColourBackground );
	}
}

void WndList::OnSysColorChange( const bool isHighContrast )
{
	m_IsHighContrast = isHighContrast;
	CreateImageList();
	SetColours();
}
