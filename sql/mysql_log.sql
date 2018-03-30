# Host: 127.0.0.1  (Version: 5.5.20)
# Date: 2015-08-24 16:03:20
# Generator: MySQL-Front 5.3  (Build 2.42)

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE */;
/*!40101 SET SQL_MODE='STRICT_TRANS_TABLES,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES */;
/*!40103 SET SQL_NOTES='ON' */;

DROP DATABASE IF EXISTS `nx_log`;
CREATE DATABASE `nx_log` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `nx_log`;

#
# Source for table "boss_log"
#

DROP TABLE IF EXISTS `boss_log`;
CREATE TABLE `boss_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_scene_id` int(11) DEFAULT NULL,
  `c_boss_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_boss_log_d_create` (`d_create`),
  KEY `idx_boss_log_s_role_name` (`s_role_name`),
  KEY `idx_boss_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_boss_log_n_scene_id` (`n_scene_id`) USING BTREE,
  KEY `idx_boss_log_c_boss_id` (`c_boss_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "equip_log"
#

DROP TABLE IF EXISTS `equip_log`;
CREATE TABLE `equip_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_operate_type` int(11) DEFAULT NULL,
  `n_equip_pos` int(11) DEFAULT NULL,
  `n_equip_slot` int(11) DEFAULT NULL,
  `n_before_grade` int(11) DEFAULT NULL,
  `n_after_grade` int(11) DEFAULT NULL,
  `c_source_item_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_target_item_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_deduct_item_ids` varchar(200) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_deduct_item_nums` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_capital_type` int(11) DEFAULT NULL,
  `n_capital_num` int(11) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_equip_log_d_create` (`d_create`),
  KEY `idx_equip_log_s_role_name` (`s_role_name`),
  KEY `idx_equip_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_equip_log_n_operate_type` (`n_operate_type`) USING BTREE,
  KEY `idx_equip_log_c_source_item_id` (`c_source_item_id`) USING BTREE,
  KEY `idx_equip_log_c_target_item_id` (`c_target_item_id`) USING BTREE,
  KEY `idx_equip_log_c_deduct_item_ids` (`c_deduct_item_ids`) USING BTREE,
  KEY `idx_equip_log_n_capital_type` (`n_capital_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "extract_award_log"
#

DROP TABLE IF EXISTS `extract_award_log`;
CREATE TABLE `extract_award_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_battle_ability` int(11) DEFAULT NULL,
  `n_extract_type` int(11) DEFAULT NULL,
  `c_params` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_award_level` int(11) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_extract_award_log_d_create` (`d_create`),
  KEY `idx_extract_award_log_s_role_name` (`s_role_name`),
  KEY `idx_extract_award_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_extract_award_log_n_extract_type` (`n_extract_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "friend_log"
#

DROP TABLE IF EXISTS `friend_log`;
CREATE TABLE `friend_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `s_friend_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_type` int(11) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_friend_log_d_create` (`d_create`),
  KEY `idx_friend_log_s_role_name` (`s_role_name`),
  KEY `idx_friend_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_friend_log_n_type` (`n_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "game_config"
#

DROP TABLE IF EXISTS `game_config`;
CREATE TABLE `game_config` (
  `s_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `s_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_type` int(11) DEFAULT '0',
  `s_item_type` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_state` varchar(1) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL DEFAULT '1',
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "game_config_type"
#

DROP TABLE IF EXISTS `game_config_type`;
CREATE TABLE `game_config_type` (
  `n_type` int(11) NOT NULL DEFAULT '0',
  `s_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`n_type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "game_player_action_log"
#

DROP TABLE IF EXISTS `game_player_action_log`;
CREATE TABLE `game_player_action_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_nation` int(11) DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_action_type` int(11) DEFAULT NULL,
  `n_scene_id` int(11) DEFAULT NULL,
  `n_action_state` int(4) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_game_player_action_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_game_player_action_log_s_role_name` (`s_role_name`) USING BTREE,
  KEY `idx_game_player_action_log_d_create` (`d_create`) USING BTREE,
  KEY `idx_game_player_action_log_n_action_type` (`n_action_type`) USING BTREE,
  KEY `idx_game_player_action_log_n_action_state` (`n_action_state`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "guild_log"
#

DROP TABLE IF EXISTS `guild_log`;
CREATE TABLE `guild_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `s_guild_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_grade` int(4) DEFAULT NULL,
  `s_by_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_operate_type` int(11) DEFAULT NULL,
  `n_capital_type` int(11) DEFAULT NULL,
  `n_capital_num` int(11) DEFAULT NULL,
  `n_donate_num` int(11) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_guild_log_d_create` (`d_create`),
  KEY `idx_guild_log_s_role_name` (`s_role_name`),
  KEY `idx_guild_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_guild_log_s_guild_name` (`s_guild_name`) USING BTREE,
  KEY `idx_guild_log_n_operate_type` (`n_operate_type`) USING BTREE,
  KEY `idx_guild_log_n_capital_type` (`n_capital_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "item_log"
#

DROP TABLE IF EXISTS `item_log`;
CREATE TABLE `item_log` (
  `auto_int_serial` int(8) NOT NULL AUTO_INCREMENT,
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `time` datetime DEFAULT NULL,
  `store_id` int(4) DEFAULT NULL,
  `recover_id` int(4) DEFAULT NULL,
  `self_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `self_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `self_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `event_id` int(4) DEFAULT NULL,
  `event_type` int(4) DEFAULT NULL,
  `event_serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `item_id` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `item_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `item_lifetime` int(4) DEFAULT NULL,
  `before` int(4) DEFAULT NULL,
  `num` int(4) DEFAULT NULL,
  `after` int(4) DEFAULT NULL,
  `n_color_level` int(4) DEFAULT NULL,
  `sender_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `sender_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `scene_id` int(11) DEFAULT NULL,
  `guild_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `guild_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_state` int(4) DEFAULT NULL,
  `comment` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`auto_int_serial`),
  KEY `idx_item_log_self_name` (`self_name`) USING BTREE,
  KEY `idx_item_log_item_id` (`item_id`) USING BTREE,
  KEY `idx_item_log_self_account` (`self_account`) USING BTREE,
  KEY `idx_item_log_time` (`time`) USING BTREE,
  KEY `idx_item_log_event_id` (`event_id`) USING BTREE,
  KEY `idx_item_log_self_uid` (`self_uid`) USING BTREE,
  KEY `idx_item_log_scene_id` (`scene_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "mail_log"
#

DROP TABLE IF EXISTS `mail_log`;
CREATE TABLE `mail_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `c_template_type` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_capital_params` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_item_params` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_mail_log_d_create` (`d_create`),
  KEY `idx_mail_log_s_role_name` (`s_role_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "money_log"
#

DROP TABLE IF EXISTS `money_log`;
CREATE TABLE `money_log` (
  `auto_int_serial` int(8) NOT NULL AUTO_INCREMENT,
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `time` datetime DEFAULT NULL,
  `store_id` int(4) DEFAULT NULL,
  `recover_id` int(4) DEFAULT NULL,
  `self_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `self_name` varchar(64) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `self_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `event_id` int(4) DEFAULT NULL,
  `event_type` int(11) DEFAULT NULL,
  `s_item_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_count` int(11) DEFAULT NULL,
  `event_serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `money_type` int(4) DEFAULT NULL,
  `before` int(11) DEFAULT NULL,
  `num` int(11) DEFAULT NULL,
  `after` int(11) DEFAULT NULL,
  `sender_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `sender_uid` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `scene_id` int(11) DEFAULT NULL,
  `guild_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `guild_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_state` int(4) DEFAULT NULL,
  `comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`auto_int_serial`),
  KEY `idx_money_log_self_name` (`self_name`) USING BTREE,
  KEY `idx_money_log_self_account` (`self_account`) USING BTREE,
  KEY `idx_money_log_time` (`time`) USING BTREE,
  KEY `idx_money_log_event_id` (`event_id`) USING BTREE,
  KEY `idx_money_log_self_uid` (`self_uid`) USING BTREE,
  KEY `idx_money_log_scene_id` (`scene_id`) USING BTREE,
  KEY `idx_money_log_money_type` (`money_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;



#
# Source for table "nation_log"
#

DROP TABLE IF EXISTS `nation_log`;
CREATE TABLE `nation_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_operate_type` int(11) DEFAULT NULL,
  `c_params` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_nation_log_d_create` (`d_create`),
  KEY `idx_nation_log_s_role_name` (`s_role_name`),
  KEY `idx_nation_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_nation_log_n_operate_type` (`n_operate_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "nx_chatlog"
#

DROP TABLE IF EXISTS `nx_chatlog`;
CREATE TABLE `nx_chatlog` (
  `serial_no` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `chat_time` datetime DEFAULT NULL,
  `chat_from` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `chat_to` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `chat_type` int(11) DEFAULT NULL,
  `chat_content` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `chat_comment` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial_no`),
  KEY `IX_nx_chatlog_1` (`chat_time`),
  KEY `IX_nx_chatlog_2` (`chat_from`),
  KEY `IX_nx_chatlog_3` (`chat_to`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "nx_gmlog"
#

DROP TABLE IF EXISTS `nx_gmlog`;
CREATE TABLE `nx_gmlog` (
  `serial_no` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `log_time` datetime DEFAULT NULL,
  `log_name` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `log_source` int(11) DEFAULT NULL,
  `log_type` int(11) DEFAULT NULL,
  `log_content` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `log_comment` varchar(2048) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial_no`),
  KEY `IX_nx_gmlog_1` (`log_time`),
  KEY `IX_nx_gmlog_2` (`log_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "nx_logdata"
#

DROP TABLE IF EXISTS `nx_logdata`;
CREATE TABLE `nx_logdata20151026` (
  `serial_no` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `log_time` datetime DEFAULT NULL,
  `log_name` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `log_source` int(11) DEFAULT NULL,
  `log_type` int(11) DEFAULT NULL,
  `log_content` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `log_comment` varchar(256) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial_no`),
  KEY `idx_nx_logdata_log_time` (`log_time`) USING BTREE,
  KEY `idx_nx_logdata_log_name` (`log_name`) USING BTREE,
  KEY `idx_nx_logdata_log_type` (`log_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "pet_ride_log"
#

DROP TABLE IF EXISTS `pet_ride_log`;
CREATE TABLE `pet_ride_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `c_pet_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_operate_type` int(11) DEFAULT NULL,
  `n_level_before` int(11) DEFAULT NULL,
  `n_level_after` int(11) DEFAULT NULL,
  `c_skill_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_pet_ride_log_d_create` (`d_create`),
  KEY `idx_pet_ride_log_s_role_name` (`s_role_name`),
  KEY `idx_pet_ride_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_pet_ride_log_n_operate_type` (`n_operate_type`) USING BTREE,
  KEY `idx_pet_ride_log_c_pet_id` (`c_pet_id`) USING BTREE,
  KEY `idx_pet_ride_log_c_skill_id` (`c_skill_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "punish_log"
#

DROP TABLE IF EXISTS `punish_log`;
CREATE TABLE `punish_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_gm_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_type` int(11) DEFAULT NULL,
  `n_minutes` int(11) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_punish_log_d_create` (`d_create`),
  KEY `idx_punish_log_s_role_name` (`s_role_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "pvp_log"
#

DROP TABLE IF EXISTS `pvp_log`;
CREATE TABLE `pvp_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `d_end` datetime DEFAULT NULL,
  `s_be_challenge` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_rank_before` int(11) DEFAULT NULL,
  `n_rank_after` int(11) DEFAULT NULL,
  `n_result` int(4) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_pvp_log_d_create` (`d_create`),
  KEY `idx_pvp_log_s_role_name` (`s_role_name`),
  KEY `idx_pvp_log_c_account` (`c_account`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#
# Source for table "role_log"
#

DROP TABLE IF EXISTS `role_log`;
CREATE TABLE `role_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_ip` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_login_time` datetime DEFAULT NULL,
  `d_logout_time` datetime DEFAULT NULL,
  `c_mac` varchar(200) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_os_type` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_role_log_d_login_time` (`d_login_time`),
  KEY `idx_role_log_s_role_name` (`s_role_name`),
  KEY `idx_role_log_c_account` (`c_account`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "role_upgrade_log"
#

DROP TABLE IF EXISTS `role_upgrade_log`;
CREATE TABLE `role_upgrade_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_upgrade_type` int(11) DEFAULT NULL,
  `n_level_before` int(11) DEFAULT NULL,
  `n_level_after` int(11) DEFAULT NULL,
  `c_skill_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_role_upgrade_log_d_create` (`d_create`),
  KEY `idx_role_upgrade_log_s_role_name` (`s_role_name`),
  KEY `idx_role_upgrade_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_role_upgrade_log_n_upgrade_type` (`n_upgrade_type`) USING BTREE,
  KEY `idx_role_upgrade_log_c_skill_id` (`c_skill_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "scene_log"
#

DROP TABLE IF EXISTS `scene_log`;
CREATE TABLE `scene_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_scene_id` int(11) DEFAULT NULL,
  `d_login_time` datetime DEFAULT NULL,
  `d_logout_time` datetime DEFAULT NULL,
  `n_total_time` int(11) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_scene_log_d_login_time` (`d_login_time`),
  KEY `idx_scene_log_s_role_name` (`s_role_name`),
  KEY `idx_scene_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_scene_log_n_scene_id` (`n_scene_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "sceret_log"
#

DROP TABLE IF EXISTS `sceret_log`;
CREATE TABLE `sceret_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `d_end` datetime DEFAULT NULL,
  `n_sceret_id` int(11) DEFAULT NULL,
  `n_dead` int(11) DEFAULT NULL,
  `n_result` int(4) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_sceret_log_d_create` (`d_create`),
  KEY `idx_sceret_log_s_role_name` (`s_role_name`),
  KEY `idx_sceret_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_sceret_log_n_sceret_id` (`n_sceret_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "task_log"
#

DROP TABLE IF EXISTS `task_log`;
CREATE TABLE `task_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `c_task_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_type` int(4) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_task_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_task_log_s_role_name` (`s_role_name`) USING BTREE,
  KEY `idx_task_log_d_create` (`d_create`) USING BTREE,
  KEY `idx_task_log_n_type` (`n_type`) USING BTREE,
  KEY `idx_task_log_c_task_id` (`c_task_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "title_log"
#

DROP TABLE IF EXISTS `title_log`;
CREATE TABLE `title_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_title_id` int(11) DEFAULT NULL,
  `n_operate_type` int(11) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_title_log_d_create` (`d_create`),
  KEY `idx_title_log_s_role_name` (`s_role_name`),
  KEY `idx_title_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_title_log_n_operate_type` (`n_operate_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


#
# Source for table "traceorder_log"
#

DROP TABLE IF EXISTS `traceorder_log`;
CREATE TABLE `traceorder_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_custom_type` int(11) DEFAULT NULL,
  `c_buy_index` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_trace_order` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_money` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_traceorder_log_d_create` (`d_create`),
  KEY `idx_traceorder_log_s_role_name` (`s_role_name`),
  KEY `idx_traceorder_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_traceorder_log_n_custom_type` (`n_custom_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `wing_log`;
CREATE TABLE `wing_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_account` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_operate_type` int(11) DEFAULT NULL,
  `n_wing_pos` int(4) DEFAULT NULL,
  `n_before_grade` int(11) DEFAULT NULL,
  `n_after_grade` int(11) DEFAULT NULL,
  `n_before_exp` int(11) DEFAULT NULL,
  `n_after_exp` int(11) DEFAULT NULL,
  `c_deduct_item_ids` varchar(200) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `c_deduct_item_nums` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `n_capital_type` int(4) DEFAULT NULL,
  `n_capital_num` int(11) DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_wing_log_d_create` (`d_create`) USING BTREE,
  KEY `idx_wing_log_s_role_name` (`s_role_name`) USING BTREE,
  KEY `idx_wing_log_c_account` (`c_account`) USING BTREE,
  KEY `idx_wing_log_n_operate_type` (`n_operate_type`) USING BTREE,
  KEY `idx_wing_log_c_deduct_item_ids` (`c_deduct_item_ids`) USING BTREE,
  KEY `idx_wing_log_n_capital_type` (`n_capital_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `title_new_log`;
CREATE TABLE `title_new_log` (
  `serial` varchar(32) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `c_uid` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `s_role_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  `d_create` datetime DEFAULT NULL,
  `n_title_id` int(11) DEFAULT NULL,
  `n_operate_type` int(11) DEFAULT NULL,
  `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`serial`),
  KEY `idx_title_new_log_d_create` (`d_create`),
  KEY `idx_title_new_log_s_role_name` (`s_role_name`),
  KEY `idx_title_new_log_c_account` (`c_uid`) USING BTREE,
  KEY `idx_title_new_log_n_operate_type` (`n_operate_type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
