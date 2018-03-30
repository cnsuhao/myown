CREATE TABLE dc_accounts (
	name varchar(128) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
	password varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL ,
	create_time datetime NULL ,
	lastlog_time datetime NULL ,
	lastlog_addr varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL ,
	valid_time datetime NULL ,
	user_type int NULL ,
	status int NULL ,
	logged int NULL ,
	charge_mode int NULL ,
	points int NULL ,
	used_points int NULL ,
	comment varchar(256) CHARACTER SET utf8 COLLATE utf8_bin NULL 
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE dc_accounts ADD CONSTRAINT PK_dc_accounts PRIMARY KEY CLUSTERED (name);

