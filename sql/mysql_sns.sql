CREATE TABLE `nx_sns` (
  `uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL COMMENT '角色uid',
  `create_time` datetime default NULL COMMENT '创建时间',
  `deleted` int(11) default NULL COMMENT '是否已删除标志',
  `save_time` datetime default NULL COMMENT '保存时间',
  `active_count` int(11) default NULL COMMENT '活跃值',
  `save_data` longblob COMMENT '保存的sns数据'
) CHARACTER SET utf8 ENGINE = MyISAM COMMENT='sns数据表-保存角色sns数据-DESIGNER:tangyz';

ALTER TABLE `nx_sns` ADD CONSTRAINT PK_nx_sns PRIMARY KEY CLUSTERED (uid);
ALTER TABLE `nx_sns` ADD CONSTRAINT IX_nx_sns UNIQUE (uid);

CREATE TABLE `nx_snslog` (
`serial_no`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
`log_time`  datetime NULL DEFAULT NULL ,
`log_uid`  varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL ,
`log_source`  int(11) NULL DEFAULT NULL ,
`log_type`  int(11) NULL DEFAULT NULL ,
`log_content`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL ,
`log_comment`  varchar(256) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL ,
PRIMARY KEY (`serial_no`)
)
ENGINE=MyISAM
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci;
