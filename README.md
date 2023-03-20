# DirectoryMonitoring

DirectoryMonitoring watch what is inside directory which selected with inotifywait utility. 
Project uses signals, pipes and named pipes. inotifywait monitor changes in directory which selected.
It is implemented as a server client service.

Compile command: make all

Run executable: ./sniffer or ./sniffer -p pathofdirectory

User give input at runtime when using program ./sniffer 
