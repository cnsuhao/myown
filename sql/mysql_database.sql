CREATE TABLE nx_chunks (
	serial_no varchar(64) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	server_id int NULL,
	scene_id int NULL,
	create_time datetime NULL,
	deleted int NULL,
	save_time datetime NULL,
	save_data longblob NULL 
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_chunks ADD CONSTRAINT PK_nx_chunks PRIMARY KEY CLUSTERED (serial_no);
CREATE INDEX IX_nx_chunks_1 ON nx_chunks(name);
CREATE INDEX IX_nx_chunks_2 ON nx_chunks(scene_id);
CREATE INDEX IX_nx_chunks_3 ON nx_chunks(server_id);

CREATE TABLE nx_letter (
	serial_no varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	msg_send_time datetime NULL,
	msg_time varchar(64) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	msg_name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	msg_uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	msg_source varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	msg_source_uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	msg_type int NULL,
	msg_content varchar(256) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	msg_appendix varchar(4096) CHARACTER SET utf8 COLLATE utf8_bin NULL 
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_letter ADD CONSTRAINT PK_nx_letter PRIMARY KEY CLUSTERED (serial_no);
CREATE INDEX IX_nx_letter_1 ON nx_letter(msg_send_time);
CREATE INDEX IX_nx_letter_2 ON nx_letter(msg_name);
CREATE INDEX IX_nx_letter_3 ON nx_letter(msg_uid);
CREATE INDEX IX_nx_letter_4 ON nx_letter(msg_source);
CREATE INDEX IX_nx_letter_5 ON nx_letter(msg_source_uid);
CREATE INDEX IX_nx_letter_6 ON nx_letter(msg_type);

CREATE TABLE nx_pubdata (
	name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	create_time datetime NULL,
	deleted int NULL,
	save_time datetime NULL,
	save_data longblob NULL 
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_pubdata ADD CONSTRAINT PK_nx_pubdata PRIMARY KEY CLUSTERED (name);
ALTER TABLE nx_pubdata ADD CONSTRAINT IX_nx_pubdata UNIQUE (uid);

CREATE TABLE `nx_roles` (
  `name` varchar(128) NOT NULL,
  `uid` varchar(32) NOT NULL,
  `account` varchar(128) NOT NULL,
  `create_time` datetime DEFAULT NULL,
  `delete_time` datetime DEFAULT NULL,
  `open_time` datetime DEFAULT NULL,
  `server_id` int(11) DEFAULT NULL,
  `total_sec` int(11) DEFAULT NULL,
  `deleted` int(11) DEFAULT NULL,
  `letter_max` int(11) DEFAULT NULL,
  `sys_flags` int(11) DEFAULT NULL,
  `illegals` int(11) DEFAULT NULL,
  `online` int(11) DEFAULT NULL,
  `role_index` int(11) DEFAULT NULL,
  `scene_id` int(11) DEFAULT NULL,
  `location` varchar(64) DEFAULT NULL,
  `resume_time` datetime DEFAULT NULL,
  `resume_info` varchar(128) DEFAULT NULL,
  `charge_info` varchar(128) DEFAULT NULL,
  `save_para1` varchar(2048) DEFAULT NULL,
  `store_serial` int(8) DEFAULT NULL,
  `money_info` varchar(128) DEFAULT NULL,
  `save_time` datetime DEFAULT NULL,
  `save_data` longblob,
  `lazy_data` longblob,
  `save_para2` varchar(2048) DEFAULT NULL,
  PRIMARY KEY (`name`),
  UNIQUE KEY `IX_nx_roles` (`uid`),
  KEY `IX_nx_roles_1` (`account`),
  KEY `IX_nx_roles_2` (`save_time`),
  KEY `IX_nx_roles_3` (`resume_time`),
  KEY `IX_nx_roles_4` (`server_id`),
  KEY `IX_nx_roles_5` (`online`),
  KEY `IX_nx_roles_6` (`create_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


CREATE TABLE nx_changename (
	name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	new_name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	save_time datetime NULL
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_changename ADD CONSTRAINT PK_nx_changename PRIMARY KEY CLUSTERED (name);

CREATE TABLE nx_roles_crossing (
	name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	status int NOT NULL,
	role_name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	server_id int NOT NULL,
	scene_id int NULL,
	save_time datetime NULL
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_roles_crossing ADD CONSTRAINT PK_nx_roles_crossing PRIMARY KEY CLUSTERED (name);
ALTER TABLE nx_roles_crossing ADD CONSTRAINT IX_roles_crossing UNIQUE (uid);
ALTER TABLE nx_roles_crossing ADD UNIQUE INDEX IX_index_name_server(role_name, server_id);

