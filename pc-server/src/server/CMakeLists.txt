ADD_EXECUTABLE (server server.c ../../../src/bt-scan/commands.c)
TARGET_LINK_LIBRARIES (server pthread) 
