import json
import shutil
import sys
import os
import configure
import template

def gen_config( dst, src, zone_name, template ):
	zone_name = str(zone_name);
	srcpath = src + zone_name + ".json";
	if not os.path.exists(srcpath):
		print "%s.json not found" % (zone_name);
		return -1;

	print "start out config:" + zone_name;
	dstpath=dst + "/" + zone_name + "/";
	if os.path.exists(dstpath) == False:
		os.mkdir(dstpath);

	cfg = configure.Configure(srcpath);
	cfg.prepare();
	data = cfg.get_data();
	template.write_config( data, dstpath );

	# gen db install.bat
	db_install = dstpath + "db_install.bat";
	db = open(db_install, 'wb');
	db.write(
'''@echo off
rem copy this file to sql directory
onekey %s %s %s %d %d %d
''' % (data["db_user"], data["db_pwd"], data["db_host"], data["db_port"], data["GameZoneID"], data["GameZoneID"]) );
	db.close();
	return 0;


if __name__ == "__main__":
	outdir="./out";
	configdir="./cfg/";
	if os.path.exists(outdir) == False:
		os.mkdir(outdir);

	template = template.ConfigTemplate("templates.json");

	if len(sys.argv) > 1:
		zone_name = sys.argv[1];
		gen_config(outdir, configdir, zone_name, template);
	else:
		zone_list = os.listdir(configdir);
		for zone_name in zone_list:
			gen_config(outdir, configdir, os.path.splitext(zone_name)[0], template);