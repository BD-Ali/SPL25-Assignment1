#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : active_deck(0), auto_sync(false), bpm_tolerance(0)
{
    decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks.\n" << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std::cout << "[MixingEngineService] Cleaning up decks...\n";
    for (size_t i = 0; i < 2; ++i) {
        delete decks[i];
        decks[i] = nullptr;
    }
}



/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    // Log section header
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    
    // Clone track polymorphically and wrap in PointerWrapper
    PointerWrapper<AudioTrack> cloned_wrapper = track.clone();
    
    // If clone fails, log error and return -1
    if (!cloned_wrapper) {
        std::cout << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone" << std::endl;
        return -1;
    }
    
    // Identify target deck
    size_t target_deck = 1 - active_deck;  // The one that is NOT currently active
    // Log deck switch
    std::cout << "[Deck Switch] Target deck: " << target_deck << std::endl;
    
    // Unload target deck if occupied
    if (decks[target_deck] != nullptr) {
        delete decks[target_deck];
        decks[target_deck] = nullptr;
    }
    
    // Perform track preparation on cloned track
    cloned_wrapper->load();
    cloned_wrapper->analyze_beatgrid();

    // BPM management
    if (decks[active_deck] != nullptr && auto_sync) {
        if (!can_mix_tracks(cloned_wrapper)) {
            sync_bpm(cloned_wrapper);
        }
    }

    decks[target_deck] = cloned_wrapper.release();  // Transfer ownership to deck
    std::cout << "[Load Complete] Track: \"" << decks[target_deck]->get_title() 
              << "\" loaded to Deck " << target_deck << std::endl;
    
    
    if (decks[active_deck] != nullptr) {
        std::cout << "[Unload] Unloading previous deck " << active_deck 
                  << " (" << decks[active_deck]->get_title() << ")" << std::endl;
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }

    active_deck = target_deck; // Switch active deck
    std::cout << "[Active Deck] Switched to deck " << target_deck << std::endl;

    return (int) target_deck;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    // Ensure both decks are occupied and track is valid
    if (decks[active_deck] == nullptr || !track) {
        return false; 
    }

    // Calculate BPM difference
    int bpm_active = decks[active_deck]->get_bpm();
    int bpm_new = track->get_bpm();
    int bpm_diff = std::abs(bpm_active - bpm_new);

    return bpm_diff <= bpm_tolerance;
}


/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    // Ensure active deck and track are valid
    if (decks[active_deck] == nullptr || !track) {
        std::cout << "[ERROR] Cannot sync BPM - active deck or track is null." << std::endl;
        return;
    }
    // Calculate average BPM and set it to the new track
    int bpm_new = track->get_bpm();
    int bpm_active = decks[active_deck]->get_bpm();
    int average_bpm = (bpm_new + bpm_active) / 2;
    track->set_bpm(average_bpm);
    std::cout << "[Sync BPM] Syncing BPM from " << bpm_new 
          << " to " << average_bpm << std::endl;
}
