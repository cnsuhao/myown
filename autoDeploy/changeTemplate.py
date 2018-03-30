#-*- encoding: gb2312 -*-
import json
import codecs
import ConfigParser
import sys
reload(sys)
sys.setdefaultencoding('gb2312')
configTeamplateName ="ChangeTemplate.json"

class MyConfig(ConfigParser.ConfigParser):
	def __ini__(self,defaults=None):
		ConfigParser.ConfigParser.__init__(self,defaults)
	def optionxform(self,optionstr):
		return optionstr


class ConfigTemplate:
	def LoadConf(self):
		with open(configTeamplateName) as file:
			file=json.load(file)
		return file

	def Init(self):
		self.configTemplateInfo = self.LoadConf()
	def SetValue(self,fd,section,key,value):
		try:
			fd.set(section,key,value)
		except:
			print "error not found section=%s,key=%s" ,section ,key


	def ValueParse(self,value,serverInfo):
		if  value[0]=='@':
			value=value[1:]
			value=serverInfo[value]
		return value


	def ChangeFile(self,serverInfo):
		fileList = self.configTemplateInfo["ChangeFile"]
		for fileInfo in fileList:
			fileName = fileInfo["FileName"]
			config = MyConfig(allow_no_value=True)
			fd = codecs.open(fileName,encoding="gb2312")
			config.readfp(fd)
			content = fileInfo["Content"]
			for info in content :
				section = info["SectionName"]
				kvGroup = info["KeyValueGroup"]
				for kv in kvGroup:
					key = kv["KeyName"]
					value=kv["ValueName"]
					value = self.ValueParse(value,serverInfo)	
					self.SetValue(config,section,key,value)
					#config.set(section,key,value)
			config.write(open(fileName, "wb")) 


	
