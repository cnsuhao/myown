DROP TABLE IF EXISTS `game_config`;
CREATE TABLE `game_config` (
  `s_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `s_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_type` int(11) DEFAULT '0',
  `s_item_type` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_state` varchar(1) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL DEFAULT '1',
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  KEY `idx_game_config_s_id` (`s_id`) USING BTREE,
  KEY `idx_game_config_n_type` (`n_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
