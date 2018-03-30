CREATE TABLE nx_domains (
	name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	create_time datetime NULL,
	deleted int NULL,
	save_time datetime NULL,
	save_data longblob NULL 
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_domains ADD CONSTRAINT PK_nx_domains PRIMARY KEY CLUSTERED (name);
ALTER TABLE nx_domains ADD CONSTRAINT IX_nx_domains UNIQUE (uid);

CREATE TABLE nx_guilds (
	name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	create_time datetime NULL,
	deleted int NULL,
	save_time datetime NULL,
	save_data longblob NULL 
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_guilds ADD CONSTRAINT PK_nx_guilds PRIMARY KEY CLUSTERED (name);
ALTER TABLE nx_guilds ADD CONSTRAINT IX_nx_guilds UNIQUE (uid);

