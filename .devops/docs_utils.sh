#!/bin/bash

parseVersion(){
    local MAJOR=$(cat include/EvoAI/Config.hpp | grep EVOAI_VERSION_MAJOR | cut -d " " -f 3)
    local MINOR=$(cat include/EvoAI/Config.hpp | grep EVOAI_VERSION_MINOR | cut -d " " -f 3)
    echo "$MAJOR.$MINOR"
}
