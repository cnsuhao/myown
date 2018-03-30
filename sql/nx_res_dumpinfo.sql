DROP TABLE IF EXISTS `nx_res_dumpinfo`;
CREATE TABLE `nx_res_dumpinfo` (
  `id` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `name` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `property` longtext CHARACTER SET utf8 COLLATE utf8_bin,
  `status` int(11) DEFAULT NULL,
  `op_name` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `op_type` int(11) DEFAULT NULL,
  `op_property` longtext CHARACTER SET utf8 COLLATE utf8_bin,
  `op_status` int(11) DEFAULT NULL,
  `op_version` int(11) DEFAULT NULL,
  `query_id` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `key_id` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`key_id`),
  KEY `IX_nx_res_dumpinfo_4` (`query_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;