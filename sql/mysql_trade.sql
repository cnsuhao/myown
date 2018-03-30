
CREATE TABLE nx_trade (
	serial_no varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
	seller_name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	seller_uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	buyer_name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	buyer_uid varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	buyer_count int NULL,
	item_type int NULL,
	item_name varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	item_info varchar(2048) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	item_cat1 varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	item_cat2 varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	item_cat3 varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NULL,
	status int NULL,
	price_min int NULL,
	price_max int NULL,
	price_deal int NULL,
	time_beg datetime NULL,
	time_end datetime NULL,
	time_deal datetime NULL
) CHARACTER SET utf8 ENGINE = MyISAM;

ALTER TABLE nx_trade ADD CONSTRAINT PK_nx_trade PRIMARY KEY CLUSTERED (serial_no);

