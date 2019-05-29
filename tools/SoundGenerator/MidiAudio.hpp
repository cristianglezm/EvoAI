#ifndef EVOAI_MIDI_AUDIO_HPP
#define EVOAI_MIDI_AUDIO_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include <SFML/Graphics/Image.hpp>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Utils.hpp>

#include "midifile/MidiFile.h"

namespace EvoAI{

    using Melodies = std::vector<std::vector<int>>;
    using Rhythms = std::vector<std::vector<int>>;
    using Commands = std::vector<std::vector<int>>;
    /**
     * @brief Limits of outputs of the neural network.
     */
    enum Limits{
        INSTRUMENTS_START = 0,
        INSTRUMENTS_END = 128,
        NOTES_START = 128,
        NOTES_END = 256,
        CHANNELS_START = 256,
        CHANNELS_END = 272,
        CHANGE_INSTRUMENT = 272,
        PITCH_BEND = 273,
        TIMBRE = 274,
        SUSTAIN_PEDAL = 275,
        SUSTAIN = 276,
        VELOCITY = 277
    };
    /**
     * @brief selects the instruments from one of the 128[0-127] outputs of the neural network.
     * @param int trackNum the track number of the smf::MidiFile.
     * @param int channel the channel for the command
     * @param std::vector<double>* outputs the neural network outputs min size = [128]
     * @param smf::Midifile* mf midifile to add the command to
     * @param int actionTime use this to calculate when to put the command
     */
    void selectInstrument(int trackNum, int channel, std::vector<double>* outputs, smf::MidiFile* mf, int actionTime) noexcept;
    /**
     * @brief selects the note(highest number) from one of the 128[128-255] outputs of the neural network.
     * @param int trackNum the track number of the smf::MidiFile.
     * @param int channel the channel for the command
     * @param std::vector<double>* outputs the neural network outputs min size = [256]
     * @param smf::Midifile* mf midifile to add the command to
     * @param int actionTime use this to calculate when to put the command
     * @return int selected note
     */
    int selectNote(int trackNum, int channel,std::vector<double>* outputs,smf::MidiFile* mf, int& actionTime) noexcept;
    /**
     * @brief Adds the noteOn from the outputs that are over 0.5 and those that are under it adds a noteOff. 
     *       128[128-255] outputs of the neural network.
     * @param int trackNum the track number of the smf::MidiFile.
     * @param int channel the channel for the command
     * @param std::vector<double>* outputs the neural network outputs min size = [256]
     * @param smf::Midifile* mf midifile to add the command to
     * @param int actionTime use this to calculate when to put the command
     */
    void playNotes(int trackNum, int channel,std::vector<double>* outputs,smf::MidiFile* mf, int actionTime) noexcept;
    /**
     * @brief selects a channel(highest number) from the outputs of the neural network
     * @param std::vector<double>* outputs
     * @return int return the selected channel 
     */
    int selectChannel(std::vector<double>* outputs) noexcept;
    /**
     * @brief add notes on / notes off and changes of timbre, pitch bend, sustain on / off, sustainPedal on /off to a smf::MidiFile
     * @param const int& tracks how many tracks
     * @param const int& notes how many notes
     * @param NeuralNetwork* nn neural network of 3 inputs and 278 outputs.
     * @param const std::string& soundOutput filename to save the Midifile to.
     */
    void generateMidifile(const int& tracks, const int& notes, NeuralNetwork* nn, const std::string& soundOutput);
    /**
     * @brief processes std::vector<std::vector<int>> and makes a MidiFile
     * @param Commands&& comm commands              [data 0]
     *  note on = 0x90->0x9F
     *  note off = 0x80->0x8F
     *  change instrument = 0xC0-> 0xCF
     *  change controller = 0xB0->0xBF
     * @param Melodies&& m notes values from 0-127  [data 2]
     * @param Rhythms&& r values from 1-5           [data 1]
     * @param const int& tpq ticks per quarter
     * @return std::unique_ptr<smf::MidiFile>
     */
    std::unique_ptr<smf::MidiFile> processSong(Commands&& comm, Melodies&& m, Rhythms&& r, const int& tpq);
    /**
     * @brief makes a random song.
     * @param const std::eize_t& channels how many channels
     * @param const std::size_t% notes how many notes
     * @return std::unique_ptr<smf::MidiFile>
     */
    std::unique_ptr<smf::MidiFile> makeRandomSong(const std::size_t& channels, const std::size_t& notes) noexcept;
}

#endif // EVOAI_MIDI_AUDIO_HPP
