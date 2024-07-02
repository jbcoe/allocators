#!/usr/bin/env bash

# Install Marp CLI
# https://formulae.brew.sh/formula/marp-cli


# Generate slides
# ./scripts/slides.sh

cd $(dirname $(dirname "$0"))
set -x
marp TALK.md --allow-local-files --html -o TALK.pdf
marp TALK.md --allow-local-files --html -o TALK.html
