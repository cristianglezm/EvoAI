#include "MidiAudio.hpp"

namespace EvoAI{

    void selectInstrument(int trackNum, int channel,std::vector<double>* outputs,smf::MidiFile* mf, int actionTime) noexcept{
        auto index = 0u;
        double max = 0.0;
        for(int i=Limits::INSTRUMENTS_START;i<Limits::INSTRUMENTS_END;++i){
            if((*outputs)[i] >= max){
                max = (*outputs)[i];
                index = i;
            }
        }
        mf->addPatchChange(trackNum,actionTime,channel,index);
    }

    int selectNote(int trackNum, int channel,std::vector<double>* outputs,smf::MidiFile* mf, int& actionTime) noexcept{
        auto index = 0;
        double max = 0.0;
        int vel = 64;
        for(int i=Limits::NOTES_START;i<Limits::NOTES_END;++i){
            if((*outputs)[i] > max){
                max = (*outputs)[i];
                index = i;
                vel = std::clamp<int>((*outputs)[Limits::VELOCITY] * 127, 5, 127);
            }
        }
        // calculate note 0-127
        int note = (index-128);
        mf->addNoteOn(trackNum,actionTime,channel,note,vel);
        actionTime += (*outputs)[Limits::VELOCITY];
        mf->addNoteOff(trackNum,actionTime,channel,note);
        return note;
    }

    void playNotes(int trackNum, int channel,std::vector<double>* outputs,smf::MidiFile* mf, int actionTime) noexcept{
        auto note = 0;
        for(int i=Limits::NOTES_START;i<Limits::NOTES_END;++i){
            if((*outputs)[i] >= 0.5){
                int vel = std::clamp<int>((*outputs)[Limits::VELOCITY] * 127, 5, 127);
                mf->addNoteOn(trackNum,actionTime,channel,note,vel);
            }else{
                mf->addNoteOff(trackNum,actionTime,channel,note);
            }
            ++note;
        }
    }

    int selectChannel(std::vector<double>* outputs) noexcept{
        int channel = 0;
        double max = 0.0;
        for(int i=Limits::CHANNELS_START;i<Limits::CHANNELS_END;++i){
            if((*outputs)[i] >= max){
                max = (*outputs)[i];
                channel = i;
            }
        }
        return channel;
    }

    void generateMidifile(int tracks, int notes, NeuralNetwork* nn, const std::string& soundOutput){
        if((*nn)[0].size() != 3){
            throw std::runtime_error("The neural Network needs to have 3 inputs");
        }
        if((*nn)[(*nn).size()-1].size() < 278){
            throw std::runtime_error("The neural Network needs to at least have 278 outputs");
        }
        auto mf = std::make_unique<smf::MidiFile>();
        mf->absoluteTicks();
        mf->addTrack(tracks-1);
        auto tpq = 64;
        mf->setTicksPerQuarterNote(tpq);
        auto note1 = randomGen().random(0,127);
        auto note2 = randomGen().random(0,127);
        auto note3 = randomGen().random(0,127);
        for(auto x=0;x<tracks;++x){
            auto actionTime = 0;
            for(auto y=0;y<notes;++y){
                if(note1 == note2 && note2 == note3){
                    note1 = randomGen().random(0,127);
                    note2 = randomGen().random(0,127);
                    note3 = randomGen().random(0,127);
                }
                std::vector<double> inputs;
                inputs.emplace_back(EvoAI::stableNormalize<double>(note1,0,127));
                inputs.emplace_back(EvoAI::stableNormalize<double>(note2,0,127));
                inputs.emplace_back(EvoAI::stableNormalize<double>(note3,0,127));
                nn->setInputs(std::move(inputs));
                auto outputs = nn->run();
                nn->reset();
                auto channel = selectChannel(&outputs);
                if(outputs[Limits::CHANGE_INSTRUMENT] >= 0.5){
                    selectInstrument(x,channel,&outputs,mf.get(),actionTime);
                }
                if(outputs[Limits::PITCH_BEND] >= 0.5){
                    mf->addPitchBend(x,actionTime,channel,outputs[Limits::PITCH_BEND]);
                }
                if(outputs[Limits::TIMBRE] >= 0.5){
                    auto timbre = std::clamp<int>(outputs[Limits::TIMBRE],0,127);
                    mf->addTimbre(x,actionTime,channel,timbre);
                }
                if(outputs[Limits::SUSTAIN_PEDAL] >= 0.5){
                    mf->addSustainPedalOn(x,actionTime,channel);
                }else{
                    mf->addSustainPedalOff(x,actionTime,channel);
                }
                if(outputs[Limits::SUSTAIN] >= 0.5){
                    mf->addSustainOn(x,actionTime,channel);
                }else{
                    mf->addSustainOff(x,actionTime,channel);
                }
                //playNotes(x,channel,&outputs,mf.get(),actionTime);
                std::swap(note1,note2);
                std::swap(note2,note3);
                note3 = selectNote(x,channel,&outputs,mf.get(),actionTime);
                actionTime += tpq;
            }
        }
        mf->sortTracks();
        mf->write(soundOutput);
    }

    std::unique_ptr<smf::MidiFile> processSong(Commands&& comm, Melodies&& m, Rhythms&& r, int tpq){
        if(m.size() != r.size() || r.size() != comm.size() || m.size() != comm.size()){
            throw std::runtime_error("The Melodies, Rhythms and Commands should be the same size.");
        }
        auto midi = std::make_unique<smf::MidiFile>();
        midi->absoluteTicks();
        midi->addTrack(m.size()-1);
        midi->setTicksPerQuarterNote(tpq);
        int actionTime = 0;
        auto Default = 64;
        for(auto trackNum=0u;trackNum<m.size();++trackNum){
            actionTime = 0;
            auto channel = randomGen().random(0,15);
            for(auto mel=0u;mel<m[trackNum].size();++mel){
                actionTime += tpq * r[trackNum][mel];
                if(comm[trackNum][mel] == 0x90){ ///@todo add range 0x90 to 0x9F?
                    midi->addNoteOn(trackNum,actionTime,channel,m[trackNum][mel],Default);
                    actionTime += tpq * r[trackNum][mel];
                    midi->addNoteOff(trackNum,actionTime,channel,m[trackNum][mel]);
                }else if(comm[trackNum][mel] == 0xC0){ ///@todo add range 0xC0 to 0xCF?
                    midi->addPatchChange(trackNum,actionTime,channel,m[trackNum][mel]);
                }else{
                    midi->addController(trackNum,actionTime,channel,r[trackNum][mel],m[trackNum][mel]);
                }
            }
        }
        midi->sortTracks();
        return midi;
    }

    std::unique_ptr<smf::MidiFile> makeRandomSong(std::size_t channels = 2u, std::size_t notes = 150u) noexcept{
        Melodies m; // instruments and notes 
        Rhythms r; // good named var
        Commands comms; //commands
        m.reserve(channels);
        r.reserve(channels);
        comms.reserve(channels);
        for(auto i=0u;i<channels;++i){
            std::vector<int> melody;
            std::vector<int> rhythm;
            std::vector<int> commands;

            commands.emplace_back(0xC0); // patch command
            melody.emplace_back(randomGen().random(0,127)); // random instrument
            rhythm.emplace_back(randomGen().random(1,5)); // speed
            for(auto j=0u;j<notes;++j){
                commands.emplace_back(0x90); //note on
                melody.emplace_back(randomGen().random(0,127)); // notes
                rhythm.emplace_back(randomGen().random(1,5));
            }
            m.emplace_back(melody);
            r.emplace_back(rhythm);
            comms.emplace_back(commands);
        }
        return processSong(std::move(comms),std::move(m),std::move(r), 50);
    }

}