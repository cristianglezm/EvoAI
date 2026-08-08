# Add Tools
if(EvoAI_SFML_AVAILABLE)
    add_subdirectory(NeuralNetworkVisualizer)
    add_subdirectory(GenomeVisualizer)
    add_subdirectory(ImageEvolver)
    add_subdirectory(ImageGenerator)
    add_subdirectory(ImageMixer)
    add_subdirectory(SoundGenerator)
else()
    message(STATUS "Skipping NeuralNetworkVisualizer, GenomeVisualizer, ImageEvolver, "
                    "ImageGenerator, ImageMixer, SoundGenerator - all require SFML")
endif()
