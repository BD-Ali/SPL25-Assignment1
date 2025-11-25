#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist) {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */


DJLibraryService::~DJLibraryService() {
    for (size_t i = 0; i < library.size(); ++i) {
        delete library[i];
    }
    library.clear();
    std::cout << "[DJLibraryService] Library resources cleaned up." << std::endl;
}

void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for (size_t i = 0; i < library_tracks.size(); ++i) {
        AudioTrack* track = nullptr;
        // Create track based on type
        if (library_tracks[i].type == "MP3") {
            track = new MP3Track(library_tracks[i].title, library_tracks[i].artists, 
                                 library_tracks[i].duration_seconds, library_tracks[i].bpm, 
                                 library_tracks[i].extra_param1);
            std::cout << "MP3Track created: " << library_tracks[i].extra_param1 << " kbps" << std::endl;
        } else if (library_tracks[i].type == "WAV") {
            track = new WAVTrack(library_tracks[i].title, library_tracks[i].artists, 
                                 library_tracks[i].duration_seconds, library_tracks[i].bpm, 
                                 library_tracks[i].extra_param1, library_tracks[i].extra_param2);
            std::cout << "WAVTrack created: " << library_tracks[i].extra_param1 << "Hz/" 
                      << library_tracks[i].extra_param2 << "bit" << std::endl;
        }
        // Add track to library if created successfully
        if (track){
            library.push_back(track);
        }
    }
    std::cout << "[INFO] Track library built: " 
              << library.size() << " tracks loaded." << std::endl;
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    playlist = Playlist(playlist_name);
    for (size_t i = 0; i < track_indices.size(); ++i) {
        int idx = track_indices[i];
        if (idx <= 0 || idx > (int)(library.size())) {
            std::cout << "[WARNING] Invalid track index: " << idx << std::endl;
            continue;
        } 
        // Create a polymorphic clone and unwrap to get raw pointer
        PointerWrapper<AudioTrack> cloned_wrapper = library[idx - 1]->clone();
        AudioTrack* cloned_track = cloned_wrapper.release();  // Unwrap to get raw pointer
    
        // If clone is nullptr, log error and return failure code
        if (!cloned_track) {
            std::cout << "[ERROR] Track: \"" << library[idx - 1]->get_title() << "\" failed to clone" << std::endl;
            continue;
        }
    
        // load the track data
        cloned_track->load();
        // Do beatgrid analysis
        cloned_track->analyze_beatgrid();
        // Add cloned track to playlist
        playlist.add_track(cloned_track);
        
        std::cout << "[INFO] Added " << library[idx - 1]->get_title() << " to playlist " << playlist.get_name() << std::endl;
    }
    std::cout << "[INFO] Playlist loaded:" << playlist.get_name() << " (" << playlist.get_track_count() << " tracks)." << std::endl;
}

/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
    for (size_t i = 0; i < playlist.get_track_count(); ++i) {
        titles.push_back(playlist.getTracks()[i]->get_title());
    }
    return titles;
}
