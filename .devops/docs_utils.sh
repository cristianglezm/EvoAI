#!/bin/bash

parseVersion(){
    local MAJOR=$(cat include/EvoAI/Config.hpp | grep EVOAI_VERSION_MAJOR | cut -d " " -f 3)
    local MINOR=$(cat include/EvoAI/Config.hpp | grep EVOAI_VERSION_MINOR | cut -d " " -f 3)
    echo "$MAJOR.$MINOR"
}

syncVersionsJson(){
    local latestVersions="latest/_static/versions.json"
    if [ ! -f "$latestVersions" ]; then
        echo "syncVersionsJson: $latestVersions not found, skipping" >&2
        return 1
    fi
    for dir in v*/; do
        dir="${dir%/}"
        local target="$dir/_static/versions.json"
        if [ -f "$target" ] && ! cmp -s "$latestVersions" "$target"; then
            cp "$latestVersions" "$target"
            git add "$target"
        fi
    done
}
