#include "AudioTrack.h"
#include <iostream>
#include <cstring>
#include <random>

AudioTrack::AudioTrack(const std::string& title, const std::vector<std::string>& artists, 
                      int duration, int bpm, size_t waveform_samples)
    : title(title), artists(artists), duration_seconds(duration), bpm(bpm), 
    waveform_data(nullptr), waveform_size(waveform_samples) {



    // Allocate memory for waveform analysis
    waveform_data = new double[waveform_size];

    // Generate some dummy waveform data for testing
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-1.0, 1.0);

    for (size_t i = 0; i < waveform_size; ++i) {
        waveform_data[i] = dis(gen);
    }
    #ifdef DEBUG
    std::cout << "AudioTrack created: " << title << " by " << std::endl;
    for (const auto& artist : artists) {
        std::cout << artist << " ";
    }
    std::cout << std::endl;
    #endif
}

// ========== TODO: STUDENTS IMPLEMENT RULE OF 5 ==========

AudioTrack::~AudioTrack() {
    // TODO: Implement the destructor
    #ifdef DEBUG
    std::cout << "AudioTrack destructor called for: " << title << std::endl;
    #endif
    delete[] waveform_data;
}

// Copy constructor
AudioTrack::AudioTrack(const AudioTrack& other) 
// initialize members
: title(other.title), artists(other.artists), 
  duration_seconds(other.duration_seconds), bpm(other.bpm), 
  waveform_data(new double[other.waveform_size]), waveform_size(other.waveform_size)
{
    #ifdef DEBUG
    std::cout << "AudioTrack copy constructor called for: " << other.title << std::endl;
    #endif
    for (size_t i = 0; i < waveform_size; ++i) {
        this->waveform_data[i] = other.waveform_data[i];
    }
}

// Copy assignment operator
AudioTrack& AudioTrack::operator=(const AudioTrack& other) {
    #ifdef DEBUG
    std::cout << "AudioTrack copy assignment called for: " << other.title << std::endl;
    #endif
    if (this != &other) {
        delete[] this->waveform_data;

        this->title = other.title;
        this->artists = other.artists;
        this->duration_seconds = other.duration_seconds;
        this->bpm = other.bpm;
        this->waveform_data = new double[waveform_size];

        this->waveform_size = other.waveform_size;
        for (size_t i = 0; i < waveform_size; ++i) {
            this->waveform_data[i] = other.waveform_data[i];
        }
    }
    return *this;
}

// Move constructor
AudioTrack::AudioTrack(AudioTrack&& other) noexcept 
// initialize members by stealing from other
: title(other.title), artists(other.artists), 
    duration_seconds(other.duration_seconds), bpm(other.bpm), 
    waveform_data(other.waveform_data), waveform_size(other.waveform_size)
{
    #ifdef DEBUG
    std::cout << "AudioTrack move constructor called for: " << other.title << std::endl;
    #endif

    // null out other's pointers so it doesn't own the data anymore
    other.waveform_data = nullptr;
    other.waveform_size = 0;
}

// Move assignment operator
AudioTrack& AudioTrack::operator=(AudioTrack&& other) noexcept {

    #ifdef DEBUG
    std::cout << "AudioTrack move assignment called for: " << other.title << std::endl;
    #endif
    if (this != &other) {
        // Clean existing data
        delete[] this->waveform_data;

        // Steal resources
        this->title = other.title;
        this->artists = other.artists;
        this->duration_seconds = other.duration_seconds;
        this->bpm = other.bpm;
        this->waveform_size = other.waveform_size;
        this->waveform_data = other.waveform_data;  

        // null out other's pointers so it doesn't own the data anymore
        other.waveform_data = nullptr;
        other.waveform_size = 0;
    }
    return *this;
}

void AudioTrack::get_waveform_copy(double* buffer, size_t buffer_size) const {
    if (buffer && waveform_data && buffer_size <= waveform_size) {
        std::memcpy(buffer, waveform_data, buffer_size * sizeof(double));
    }
}