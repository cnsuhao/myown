#-*- encoding: gb2312 -*-
import json
import codecs
import ConfigParser
import sys
import os;
reload(sys)
sys.setdefaultencoding('gb2312')

class MyConfig(ConfigParser.ConfigParser):
	def __ini__(self,defaults=None):
		ConfigParser.ConfigParser.__init__(self,defaults)
	def optionxform(self,optionstr):
		return optionstr


class ConfigTemplate:
	def __init__(self, template_name):
		with open(template_name) as file:
			self.templates = json.load(file);

	def __set_value(self,fd,section,key,value):
		try:
			if fd.has_option(section, key):
				fd.set(section,key,value)
		except:
			print "error not found section=%s,key=%s" ,section ,key


	def __get_value(self,value,serverInfo):
		if  value[0]=='@':
			value=value[1:]
			value=serverInfo[value]
		return value


	def write_config(self,serverInfo, outdir):
		templist = self.templates["Files"]
		for template in templist:
			file_name = template["FileName"]
			print "handle " + file_name + " begin";
			config = MyConfig(allow_no_value=True)
			hfile = codecs.open(file_name, encoding="gb2312")
			config.readfp(hfile)

			content = template["Content"]
			for info in content :
				section = info["SectionName"]
				kvGroup = info["KeyValueGroup"]
				for kv in kvGroup:
					key = kv["KeyName"]
					value=kv["ValueName"]
					value = self.__get_value(value,serverInfo);
					self.__set_value(config,section,key,value);
					#config.set(section,key,value)

			path = outdir + os.path.basename(file_name);
			config.write(open(path, "wb")) ;
			print "handle " + file_name + " completed";