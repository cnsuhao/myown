import json
import changeTemplate
import shutil
import sys
import os
import LoadConfig
global gameConfig
def GetGameZoneIPByID(zoneID):
	for zoneInfo in gameZoneList :
		if zoneInfo['GameZoneID'] == zoneID:
			print zoneInfo
			return  zoneInfo['ServerIP']


def GetGameZoneIDByIP(zoneIP):
	for zoneInfo in gameZoneList :
		if zoneInfo['ServerIP'] == zoneIP:
			return  zoneInfo['GameZoneID']

			
			
def GetGameConfigByID(zoneID):
	for zoneInfo in gameZoneList :
		if zoneInfo['GameZoneID'] == zoneID:
			return  zoneInfo


if __name__ == "__main__":
	
	if len(sys.argv) <3 :
	    print "error need 3 args"
	os.chdir(sys.argv[1])
	gameConfig=LoadConfig.initConfig()
	gameZoneList={}
	gameZoneList=gameConfig["GameZoneList"]
	template = changeTemplate.ConfigTemplate()
	template.Init()
	gameConfig = GetGameConfigByID(int(sys.argv[2]))
	template.ChangeFile(gameConfig)


