#pragma once

#include "Database.h"
#include "Handlers.h"
#include "MediaInfo.h"

#include <vector>

// Media library
class Library
{
public:
	// 'database' - application database.
	// 'handlers' - available handlers.
	Library( Database& database, const Handlers& handlers );

	virtual ~Library();

	// Media library column type.
	enum class Column {
		Filename = 1,
		Filetime,
		Filesize,
		Duration,
		SampleRate,
		BitsPerSample,
		Channels,
		Artist,
		Title,
		Album,
		Genre,
		Year,
		Comment,
		Track,
		Version,
		GainTrack,
		GainAlbum,
//		PeakTrack = 18,	*DEPRECATED*
//		PeakAlbum = 19,	*DEPRECATED*
		Artwork = 20,
		CDDB = 21,
		Bitrate = 22,

		_Undefined
	};

	// Gets media information.
	// 'mediaInfo' - in/out, media information containing the filename to query.
	// 'checkFileAttributes' - whether to check if the time/size of the file matches any existing entry.
	// 'scanMedia' - whether to scan the file specified in 'mediaInfo' if no matching database entry is found.
	// 'sendNotification' - whether to notify the main app if 'mediaInfo' has changed.
	// 'removeMissing' - whether to remove media information from the library if the file specified in 'mediaInfo' cannot be opened.
	// Returns true if media information was returned.
	bool GetMediaInfo( MediaInfo& mediaInfo, const bool checkFileAttributes = true, const bool scanMedia = true, const bool sendNotification = true, const bool removeMissing = false );

	// Updates media information and writes out tag information to file.
	// 'previousMediaInfo' - previous media information.
	// 'updatedMediaInfo' - updated media information.
	void UpdateMediaTags( const MediaInfo& previousMediaInfo, const MediaInfo& updatedMediaInfo );

	// Gets media artwork.
	// 'mediaInfo' - media information.
	// Returns the artwork image, or an empty array if there is no artwork.
	std::vector<BYTE> GetMediaArtwork( const MediaInfo& mediaInfo );

	// Adds media artwork to the database if it does not already exist.
	// 'image' - artwork image.
	// Returns the artwork ID.
	std::wstring AddArtwork( const std::vector<BYTE>& image );

	// Returns the artists contained in the media library.
	std::set<std::wstring> GetArtists();

	// Returns the albums contained in the media library.
	std::set<std::wstring> GetAlbums();
	
	// Returns the albums by 'artist' contained in the media library.
	std::set<std::wstring> GetAlbums( const std::wstring artist );

	// Returns the genres contained in the media library.
	std::set<std::wstring> GetGenres();

	// Returns the years contained in the media library.
	std::set<long> GetYears();

	// Returns the media information by 'artist' contained in the media library.
	MediaInfo::List GetMediaByArtist( const std::wstring& artist );

	// Returns the media information by 'album' contained in the media library.
	MediaInfo::List GetMediaByAlbum( const std::wstring& album );

	// Returns the media information by 'artist' & 'album' contained in the media library.
	MediaInfo::List GetMediaByArtistAndAlbum( const std::wstring& artist, const std::wstring& album );

	// Returns the media information by 'genre' contained in the media library.
	MediaInfo::List GetMediaByGenre( const std::wstring& genre );

	// Returns the media information by 'year' contained in the media library.
	MediaInfo::List GetMediaByYear( const long year );

	// Returns all media information contained in the media library.
	MediaInfo::List GetAllMedia();

	// Returns all network streams contained in the media library.
	MediaInfo::List GetStreams();

	// Returns whether the 'artist' exists in the media library.
	bool GetArtistExists( const std::wstring& artist );

	// Returns whether the 'album' exists in the media library.
	bool GetAlbumExists( const std::wstring& album );

	// Returns whether the 'artist' & 'album' exists in the media library.
	bool GetArtistAndAlbumExists( const std::wstring& artist, const std::wstring& album );

	// Returns whether the 'genre' exists in the media library.
	bool GetGenreExists( const std::wstring& genre );

	// Returns whether the 'year' exists in the media library.
	bool GetYearExists( const long year );

	// Removes 'mediaInfo' from the library.
	// Returns true if the library was updated.
	bool RemoveFromLibrary( const MediaInfo& mediaInfo );

	// Returns all the file extensions supported by the handlers, as a set of lowercase strings.
	std::set<std::wstring> GetAllSupportedFileExtensions() const;

	// Returns the 'mediaInfo' as tags.
	Tags GetTags( const MediaInfo& mediaInfo );

	// Updates the track gain, if necessary.
	// 'previousInfo' - previous media information.
	// 'updatedInfo' - updated media information.
	// 'sendNotification' - whether to notify the main application if the library has been updated.
	// Returns whether the library was updated.
	bool UpdateTrackGain( const MediaInfo& previousInfo, const MediaInfo& updatedInfo, const bool sendNotification = true );

	// Updates 'mediaInfo' with 'decoder' information.
	// 'sendNotification' - whether to notify the main application if the library has been updated.
	void UpdateMediaInfoFromDecoder( MediaInfo& mediaInfo, const Decoder& decoder, const bool sendNotification = true );

	// Returns whether there has been a recent attempt to write the tags for the 'filename'.
	bool HasRecentlyWrittenTag( const std::wstring& filename ) const;

private:
	// Media library columns.
	typedef std::map<std::string,Column> Columns;

	// Maps a filename to tag information.
	typedef std::map<std::wstring,Tags> FileTags;

	// Updates the database to the current version if necessary.
	void UpdateDatabase();

	// Updates the media table if necessary.
	void UpdateMediaTable();

	// Updates the CDDA table if necessary.
	void UpdateCDDATable();

	// Updates the artwork table if necessary.
	void UpdateArtworkTable();

	// Creates indices if necessary.
	void CreateIndices();

	// Gets the 'lastModified' time and 'fileSize' of 'filename', returning true if the file could be opened.
	bool GetFileInfo( const std::wstring& filename, long long& lastModified, long long& fileSize ) const;

	// Queries the decoders for media information.
	// 'mediaInfo' - in/out, media information containing the filename to query.
	// Returns true if the file was successfully opened by a decoder.
	bool GetDecoderInfo( MediaInfo& mediaInfo );

	// Updates the media library.
	// 'mediaInfo' - media information.
	// Returns true if the library was updated.
	bool UpdateMediaLibrary( const MediaInfo& mediaInfo );

	// Writes out tag information to file.
	// 'mediaInfo' - in/out, media information which will be modified if tags are successfully written.
	// 'tags' - tags to write.
	void WriteFileTags( MediaInfo& mediaInfo, const Tags& tags );

	// Adds an artwork to the media library.
	// 'id' - artwork ID.
	// 'artwork' - artwork image.
	bool AddArtwork( const std::wstring& id, const std::vector<BYTE>& image );

	// Searches the artwork table for a matching 'image'.
	// Returns the image ID if an image was found, or an empty string if there was no match.
	std::wstring FindArtwork( const std::vector<BYTE>& image );

	// Sets 'mediaInfo' from a SQLite 'stmt'.
	void ExtractMediaInfo( sqlite3_stmt* stmt, MediaInfo& mediaInfo );

	// Returns the library columns corresponding to 'source'.
	const Columns& GetColumns( const MediaInfo::Source source ) const;

	// Updates 'mediaInfo' with the 'tags'.
	void UpdateMediaInfoFromTags( MediaInfo& mediaInfo, const Tags& tags );

	// Adds any pending 'tags' for the 'filename', to be written out at the next opportunity.
	void AddPendingTags( const std::wstring& filename, const Tags& tags );

	// Gets any pending 'tags' for the 'filename'.
	// Returns whether there are any pending tags.
	bool GetPendingTags( const std::wstring& filename, Tags& tags ) const;

	// Updates the time at which the last attempt was made to write the tags for the 'filename'.
	void SetRecentlyWrittenTag( const std::wstring& filename );

	// Database.
	Database& m_Database;

	// The available handlers.
	const Handlers& m_Handlers;

	// Tag information waiting to be written.
	FileTags m_PendingTags;

	// The time that the last attempt was made to write tags.
	long long m_LastTagWriteTime;

	// File names for which an attempt has been made to write tags, mapped to the time that the attempt was last made.
	mutable std::map<std::wstring, long long> m_TagsWritten;

	// Mutex for the map of attempted tag writes.
	mutable std::mutex m_TagsWrittenMutex;

	// Media library columns.
	Columns m_MediaColumns;

	// CD audio columns.
	Columns m_CDDAColumns;
};
