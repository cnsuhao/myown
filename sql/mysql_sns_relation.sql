CREATE TABLE nx_sns_relation (
	serial_no varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	target_uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	range_mask int NULL,
	create_time datetime NULL
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_sns_relation ADD CONSTRAINT PK_nx_sns_relation PRIMARY KEY CLUSTERED (serial_no);
CREATE INDEX IX_nx_sns_relation_1 ON nx_sns_relation(uid);
CREATE INDEX IX_nx_sns_relation_2 ON nx_sns_relation(target_uid);

CREATE TABLE nx_sns_feed (
	feed_id varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	target_uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	range_mask int NULL,
	level int NULL,
	category int NULL,
	desc_info varchar(1024) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	comment_at varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	good_count int NULL,
	bad_count int NULL,
	create_time datetime NULL
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_sns_feed ADD CONSTRAINT PK_nx_sns_feed PRIMARY KEY CLUSTERED (feed_id);
CREATE INDEX IX_nx_sns_feed_1 ON nx_sns_feed(level);
CREATE INDEX IX_nx_sns_feed_2 ON nx_sns_feed(create_time);
