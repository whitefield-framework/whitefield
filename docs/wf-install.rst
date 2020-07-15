############
Installation
############

One Time Setup
--------------
::

    git clone https://github.com/whitefield-framework/whitefield
    cd whitefield
    ./scripts/setup.sh #This may take a while...

`setup.sh` does following:

1. Checks and installs dependencies
2. Enables specific stacklines and NS3 configuration. This step creates `config.inc`.
3. `make` all the enabled modules

