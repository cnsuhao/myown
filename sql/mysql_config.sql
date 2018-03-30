CREATE TABLE nx_config (
	id varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	type int NULL,
	property longtext CHARACTER SET utf8 COLLATE utf8_bin NULL,
	status int NULL,
	op_name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	op_type int NULL,
	op_property longtext CHARACTER SET utf8 COLLATE utf8_bin NULL,
	op_status int NULL,
	op_version int NULL
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_config ADD CONSTRAINT PK_nx_config PRIMARY KEY CLUSTERED (id);
CREATE INDEX IX_nx_config_1 ON nx_config(name);
CREATE INDEX IX_nx_config_2 ON nx_config(op_name);

