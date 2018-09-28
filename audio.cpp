#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
#include <random>

#include "portaudio.h"

#include "audio.hpp"

using namespace Audio;

using namespace std::chrono;
static uint64_t get_now_ms() {
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int mix_and_play (const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData ) {
	Player *player = (Player*) userData;
	float *buffer = (float *) outputBuffer;
	Sample *s;
	s = player->get_data();
	if (s != NULL && player->is_playing()) {
		std::vector<float> data = s->get_data();
		unsigned int pos = s->get_position();

		// Fill the buffer with samples!
		for (int i=0; (i<framesPerBuffer); i++) {
			if (pos < data.size()) {
				buffer[i] = data[pos];
			} else {
				player->pause();
				buffer[i] = 0;
			}
			i++;
			pos+=2;
		}
		s->set_position(pos);
	} else {
		for (int i=0; (i<framesPerBuffer); i++) {
			buffer[i] = 0;
		}
	}
	return 0;
}

Sample::Sample() {

}

Sample::~Sample() {

}

bool Sample::finished() {
	if ( (this->position) >= (this->data.size())) return true;
	else return false;
}

void Sample::load(const char *filename) {
	std::string buffer;
	float fdata;
	std::ifstream input_file;
	unsigned int count = 0;

	input_file.open(filename, std::ios_base::in);
	if (!input_file) {
		std::cerr << "Arquivo " << filename << " nao encontrado" << std::endl;
		return;
	}

	while (std::getline(input_file, buffer) ) {
		std::stringstream(buffer) >> fdata;
		(this->data).push_back(fdata);
		count ++;
	}
	this->position = 0;

	std::cerr << "Total: " << count << " samples" << std::endl;
}

void Sample::reverse() {
	std::reverse((this->data).begin(),(this->data).end());
}

unsigned int Sample::get_position() {
	return this->position;
}

void Sample::set_position(unsigned int pos) {
	this->position = pos;
}

std::vector<float> Sample::get_data() {
	return this->data;
}

Player::Player() {
	this->playing = false;
	this->audio_sample = NULL;
}

void Player::pause() {
	this->playing = false;
}

bool Player::is_playing() {
	return this->playing;
}

Player::~Player() {
	this->stop();
}

Sample *Player::get_data() {
	return this->audio_sample;
}



void Player::play() {
	if (this->audio_sample != NULL) {
		this->playing = true;
	}
}

void Player::play(Sample *audiosample) {
	this->playing = true;
	this->audio_sample = audiosample;
}

void Player::init() {
	PaError err;

	err = Pa_Initialize();
	if( err != paNoError ) {
		std::cerr << "Error on Pa_Initialize()" << std::endl;
		return;
	}

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
	if (outputParameters.device == paNoDevice) {
		std::cerr << "Error: No default output device on Pa_GetDefaultOutputDevice()" << std::endl;
		return;
	}

	outputParameters.channelCount = 1;                     /* Mono output. */
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	err = Pa_OpenStream( &stream,
			NULL,      /* No input. */
			&outputParameters,
			44100,
			64,       /* Frames per buffer. */
			paClipOff, /* We won't output out of range samples so don't bother clipping them. */
			mix_and_play,
			this );

	if( err != paNoError ) {
		std::cerr << "Error on Pa_OpenStream()" << std::endl;
		return;
	}
	
	err = Pa_StartStream( stream );
	if( err != paNoError ) {
		std::cerr << "Error on Pa_StartStream()" << std::endl;
		return;
	}

	// Espera
	uint64_t t0=get_now_ms(), t1;
	while (1) {
		std::this_thread::sleep_for (std::chrono::milliseconds(1));
		t1 = get_now_ms();
		if (t1-t0 > 500) break;
	}
}

void Player::stop() {
	PaError err;
	err = Pa_StopStream( stream );
	if( err != paNoError ) {
		std::cerr << "Error on Pa_StopStream()" << std::endl;
		return;
	}

	err = Pa_CloseStream( stream );
	if( err != paNoError ) {
		std::cerr << "Error on Pa_StopStream()" << std::endl;
		return;
	}

	Pa_Terminate();
}




