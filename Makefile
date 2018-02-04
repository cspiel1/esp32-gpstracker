#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := gpstracker

include user.mk
include $(IDF_PATH)/make/project.mk

deploy:	$(APP_BIN)
	scp build/gpstracker.bin $(SERVERPATH)
