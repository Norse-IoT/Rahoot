#!/bin/bash

set -e # stop immediately on errors
set -o pipefail # do not silently ignore errors in pipelines
cd "${0%/*}" # cd into script's current location

if [ -z "${NVM_DIR}" ]; then
  # Set the backup value if the variable is not set
  NVM_DIR="$HOME/.nvm"
fi

[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm
npm run all
