#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    // Check if track with the same title exists in cache
    if (cache.contains(track.get_title())) {
        // HIT case: refresh MRU status and return 1
        cache.get(track.get_title()); 
        return 1;
    }
    // MISS case: proceed with insertion
    
    // Create a polymorphic clone and unwrap to get raw pointer
    PointerWrapper<AudioTrack> cloned_wrapper = track.clone();
    AudioTrack* cloned_track = cloned_wrapper.release();  // Unwrap to get raw pointer
    
    // If clone is nullptr, log error and return failure code
    if (!cloned_track) {
        std::cout << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone" << std::endl;
        return 0;
    }
    
    // load the track data
    cloned_track->load();
    // Do beatgrid analysis
    cloned_track->analyze_beatgrid();
    // Wrap the prepared clone in a new PointerWrapper
    PointerWrapper<AudioTrack> prepared_track(cloned_track);
    // Insert track clone into cache using put()
    bool eviction_occurred = cache.put(std::move(prepared_track));
    
    // Return based on eviction result
    if (eviction_occurred) {
        return -1;  // MISS with eviction (track evicted)
    } else {
        return 0;   // MISS without eviction (track inserted)
    }
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    if (cache.contains(track_title)) {
        return cache.get(track_title);
    } else {
        return nullptr;
    }
}
