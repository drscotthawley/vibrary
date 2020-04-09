The .gv files are graphviz files which are compiled into dot files. Graphviz is available from homebrew and apt. Or download binaries from here: https://graphviz.gitlab.io/download/

To compile to dot:

   `$ /usr/local/bin/dot AppFlow.gv 1> AppFlow.gv.dot`

Then open that file with the graphviz app and export to the file format you want, or

   `$ /usr/local/bin/dot -Tpng AppFlow.gv 1> AppFlow.png`

