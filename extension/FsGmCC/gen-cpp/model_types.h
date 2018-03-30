/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef model_TYPES_H
#define model_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>




struct Operation {
  enum type {
    CLOSE = 1,
    START = 2,
    RESTART = 3,
    LOAD_DATA = 4
  };
};

extern const std::map<int, const char*> _Operation_VALUES_TO_NAMES;

struct Status {
  enum type {
    CLOSING = 1,
    CLOSED = 2,
    OPENING = 3,
    OPENED = 4,
    DEAD = 5
  };
};

extern const std::map<int, const char*> _Status_VALUES_TO_NAMES;

struct OperationStatus {
  enum type {
    NORMAL = 1,
    IDLE = 2,
    BUSY = 3
  };
};

extern const std::map<int, const char*> _OperationStatus_VALUES_TO_NAMES;

struct GameStatus {
  enum type {
    READY = 1,
    OPEN = 2,
    CLOSING = 3,
    CLOSED = 4
  };
};

extern const std::map<int, const char*> _GameStatus_VALUES_TO_NAMES;

struct GameStep {
  enum type {
    INNER_TEST = 1,
    OUTER_TEST = 2,
    NOMAL = 3
  };
};

extern const std::map<int, const char*> _GameStep_VALUES_TO_NAMES;

struct SystemNoticeType {
  enum type {
    NOTICE = 1,
    ACTIVITY = 2,
    ITEMORDER = 3,
    SIGNIN = 4,
    STOREITEM = 5,
    CONFIG = 6
  };
};

extern const std::map<int, const char*> _SystemNoticeType_VALUES_TO_NAMES;

struct OperateType {
  enum type {
    ADD = 0,
    DEL = 1,
    EDIT = 2,
    HANDLED = 9
  };
};

extern const std::map<int, const char*> _OperateType_VALUES_TO_NAMES;

struct OperateStatus {
  enum type {
    UNSTART = 0,
    DOING = 1,
    COMPLETED = 9
  };
};

extern const std::map<int, const char*> _OperateStatus_VALUES_TO_NAMES;

struct ChangeStatus {
  enum type {
    UNCHANGE = 1,
    CHANGED = 9
  };
};

extern const std::map<int, const char*> _ChangeStatus_VALUES_TO_NAMES;

struct MailType {
  enum type {
    NOMAL = 1,
    NOTICE = 2
  };
};

extern const std::map<int, const char*> _MailType_VALUES_TO_NAMES;

struct ConfigType {
  enum type {
    ACTIVITY = 1
  };
};

extern const std::map<int, const char*> _ConfigType_VALUES_TO_NAMES;

class HandleResult;

class SystemNotice;

class MachineConfig;

class GameServerInfo;

class MailInfo;

class CommandInfo;

class RoleInfo;

class AreaInfo;

class Order;

typedef struct _HandleResult__isset {
  _HandleResult__isset() : operateStatus(false), changeStatus(false), mess(false), code(false) {}
  bool operateStatus :1;
  bool changeStatus :1;
  bool mess :1;
  bool code :1;
} _HandleResult__isset;

class HandleResult {
 public:

  HandleResult(const HandleResult&);
  HandleResult& operator=(const HandleResult&);
  HandleResult() : operateStatus((OperateStatus::type)0), changeStatus((ChangeStatus::type)0), mess(), code(0) {
  }

  virtual ~HandleResult() throw();
  OperateStatus::type operateStatus;
  ChangeStatus::type changeStatus;
  std::string mess;
  int32_t code;

  _HandleResult__isset __isset;

  void __set_operateStatus(const OperateStatus::type val);

  void __set_changeStatus(const ChangeStatus::type val);

  void __set_mess(const std::string& val);

  void __set_code(const int32_t val);

  bool operator == (const HandleResult & rhs) const
  {
    if (!(operateStatus == rhs.operateStatus))
      return false;
    if (!(changeStatus == rhs.changeStatus))
      return false;
    if (__isset.mess != rhs.__isset.mess)
      return false;
    else if (__isset.mess && !(mess == rhs.mess))
      return false;
    if (__isset.code != rhs.__isset.code)
      return false;
    else if (__isset.code && !(code == rhs.code))
      return false;
    return true;
  }
  bool operator != (const HandleResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const HandleResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(HandleResult &a, HandleResult &b);

inline std::ostream& operator<<(std::ostream& out, const HandleResult& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _SystemNotice__isset {
  _SystemNotice__isset() : content(false), resourceType(false), roleId(false), accId(false), operateType(false) {}
  bool content :1;
  bool resourceType :1;
  bool roleId :1;
  bool accId :1;
  bool operateType :1;
} _SystemNotice__isset;

class SystemNotice {
 public:

  SystemNotice(const SystemNotice&);
  SystemNotice& operator=(const SystemNotice&);
  SystemNotice() : content(), resourceType((SystemNoticeType::type)0), roleId(), accId(0), operateType((OperateType::type)0) {
  }

  virtual ~SystemNotice() throw();
  std::string content;
  SystemNoticeType::type resourceType;
  std::string roleId;
  int64_t accId;
  OperateType::type operateType;

  _SystemNotice__isset __isset;

  void __set_content(const std::string& val);

  void __set_resourceType(const SystemNoticeType::type val);

  void __set_roleId(const std::string& val);

  void __set_accId(const int64_t val);

  void __set_operateType(const OperateType::type val);

  bool operator == (const SystemNotice & rhs) const
  {
    if (__isset.content != rhs.__isset.content)
      return false;
    else if (__isset.content && !(content == rhs.content))
      return false;
    if (!(resourceType == rhs.resourceType))
      return false;
    if (__isset.roleId != rhs.__isset.roleId)
      return false;
    else if (__isset.roleId && !(roleId == rhs.roleId))
      return false;
    if (__isset.accId != rhs.__isset.accId)
      return false;
    else if (__isset.accId && !(accId == rhs.accId))
      return false;
    if (!(operateType == rhs.operateType))
      return false;
    return true;
  }
  bool operator != (const SystemNotice &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SystemNotice & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(SystemNotice &a, SystemNotice &b);

inline std::ostream& operator<<(std::ostream& out, const SystemNotice& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _MachineConfig__isset {
  _MachineConfig__isset() : ip(false), port(false), user(false), pwd(false) {}
  bool ip :1;
  bool port :1;
  bool user :1;
  bool pwd :1;
} _MachineConfig__isset;

class MachineConfig {
 public:

  MachineConfig(const MachineConfig&);
  MachineConfig& operator=(const MachineConfig&);
  MachineConfig() : ip(), port(0), user(), pwd() {
  }

  virtual ~MachineConfig() throw();
  std::string ip;
  int32_t port;
  std::string user;
  std::string pwd;

  _MachineConfig__isset __isset;

  void __set_ip(const std::string& val);

  void __set_port(const int32_t val);

  void __set_user(const std::string& val);

  void __set_pwd(const std::string& val);

  bool operator == (const MachineConfig & rhs) const
  {
    if (!(ip == rhs.ip))
      return false;
    if (!(port == rhs.port))
      return false;
    if (!(user == rhs.user))
      return false;
    if (!(pwd == rhs.pwd))
      return false;
    return true;
  }
  bool operator != (const MachineConfig &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MachineConfig & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(MachineConfig &a, MachineConfig &b);

inline std::ostream& operator<<(std::ostream& out, const MachineConfig& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _GameServerInfo__isset {
  _GameServerInfo__isset() : name(false), serverId(false), operationStatus(false), status(false), currentCnt(false), configure(false) {}
  bool name :1;
  bool serverId :1;
  bool operationStatus :1;
  bool status :1;
  bool currentCnt :1;
  bool configure :1;
} _GameServerInfo__isset;

class GameServerInfo {
 public:

  GameServerInfo(const GameServerInfo&);
  GameServerInfo& operator=(const GameServerInfo&);
  GameServerInfo() : name(), serverId(0), operationStatus((OperationStatus::type)0), status((Status::type)0), currentCnt(0) {
  }

  virtual ~GameServerInfo() throw();
  std::string name;
  int64_t serverId;
  OperationStatus::type operationStatus;
  Status::type status;
  int32_t currentCnt;
  MachineConfig configure;

  _GameServerInfo__isset __isset;

  void __set_name(const std::string& val);

  void __set_serverId(const int64_t val);

  void __set_operationStatus(const OperationStatus::type val);

  void __set_status(const Status::type val);

  void __set_currentCnt(const int32_t val);

  void __set_configure(const MachineConfig& val);

  bool operator == (const GameServerInfo & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(serverId == rhs.serverId))
      return false;
    if (!(operationStatus == rhs.operationStatus))
      return false;
    if (!(status == rhs.status))
      return false;
    if (__isset.currentCnt != rhs.__isset.currentCnt)
      return false;
    else if (__isset.currentCnt && !(currentCnt == rhs.currentCnt))
      return false;
    if (__isset.configure != rhs.__isset.configure)
      return false;
    else if (__isset.configure && !(configure == rhs.configure))
      return false;
    return true;
  }
  bool operator != (const GameServerInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const GameServerInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(GameServerInfo &a, GameServerInfo &b);

inline std::ostream& operator<<(std::ostream& out, const GameServerInfo& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _MailInfo__isset {
  _MailInfo__isset() : title(false), id(false), content(false), createTime(false), isAttach(false), attachInfo(false), type(false), senderId(false), recieverId(false), config(false) {}
  bool title :1;
  bool id :1;
  bool content :1;
  bool createTime :1;
  bool isAttach :1;
  bool attachInfo :1;
  bool type :1;
  bool senderId :1;
  bool recieverId :1;
  bool config :1;
} _MailInfo__isset;

class MailInfo {
 public:

  MailInfo(const MailInfo&);
  MailInfo& operator=(const MailInfo&);
  MailInfo() : title(), id(0), content(), createTime(0), isAttach(0), attachInfo(), type((MailType::type)0), senderId(), config() {
  }

  virtual ~MailInfo() throw();
  std::string title;
  int64_t id;
  std::string content;
  int64_t createTime;
  bool isAttach;
  std::string attachInfo;
  MailType::type type;
  std::string senderId;
  std::vector<std::string>  recieverId;
  std::string config;

  _MailInfo__isset __isset;

  void __set_title(const std::string& val);

  void __set_id(const int64_t val);

  void __set_content(const std::string& val);

  void __set_createTime(const int64_t val);

  void __set_isAttach(const bool val);

  void __set_attachInfo(const std::string& val);

  void __set_type(const MailType::type val);

  void __set_senderId(const std::string& val);

  void __set_recieverId(const std::vector<std::string> & val);

  void __set_config(const std::string& val);

  bool operator == (const MailInfo & rhs) const
  {
    if (!(title == rhs.title))
      return false;
    if (__isset.id != rhs.__isset.id)
      return false;
    else if (__isset.id && !(id == rhs.id))
      return false;
    if (!(content == rhs.content))
      return false;
    if (!(createTime == rhs.createTime))
      return false;
    if (!(isAttach == rhs.isAttach))
      return false;
    if (__isset.attachInfo != rhs.__isset.attachInfo)
      return false;
    else if (__isset.attachInfo && !(attachInfo == rhs.attachInfo))
      return false;
    if (!(type == rhs.type))
      return false;
    if (__isset.senderId != rhs.__isset.senderId)
      return false;
    else if (__isset.senderId && !(senderId == rhs.senderId))
      return false;
    if (__isset.recieverId != rhs.__isset.recieverId)
      return false;
    else if (__isset.recieverId && !(recieverId == rhs.recieverId))
      return false;
    if (__isset.config != rhs.__isset.config)
      return false;
    else if (__isset.config && !(config == rhs.config))
      return false;
    return true;
  }
  bool operator != (const MailInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MailInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(MailInfo &a, MailInfo &b);

inline std::ostream& operator<<(std::ostream& out, const MailInfo& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _CommandInfo__isset {
  _CommandInfo__isset() : roleId(false), command(false), cmdArgs(false), operateRoleId(false), cmdId(false) {}
  bool roleId :1;
  bool command :1;
  bool cmdArgs :1;
  bool operateRoleId :1;
  bool cmdId :1;
} _CommandInfo__isset;

class CommandInfo {
 public:

  CommandInfo(const CommandInfo&);
  CommandInfo& operator=(const CommandInfo&);
  CommandInfo() : roleId(), command(), operateRoleId(), cmdId(0) {
  }

  virtual ~CommandInfo() throw();
  std::string roleId;
  std::string command;
  std::vector<std::string>  cmdArgs;
  std::string operateRoleId;
  int64_t cmdId;

  _CommandInfo__isset __isset;

  void __set_roleId(const std::string& val);

  void __set_command(const std::string& val);

  void __set_cmdArgs(const std::vector<std::string> & val);

  void __set_operateRoleId(const std::string& val);

  void __set_cmdId(const int64_t val);

  bool operator == (const CommandInfo & rhs) const
  {
    if (!(roleId == rhs.roleId))
      return false;
    if (!(command == rhs.command))
      return false;
    if (!(cmdArgs == rhs.cmdArgs))
      return false;
    if (!(operateRoleId == rhs.operateRoleId))
      return false;
    if (!(cmdId == rhs.cmdId))
      return false;
    return true;
  }
  bool operator != (const CommandInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const CommandInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(CommandInfo &a, CommandInfo &b);

inline std::ostream& operator<<(std::ostream& out, const CommandInfo& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _RoleInfo__isset {
  _RoleInfo__isset() : areaId(false), isOnline(false), name(false) {}
  bool areaId :1;
  bool isOnline :1;
  bool name :1;
} _RoleInfo__isset;

class RoleInfo {
 public:

  RoleInfo(const RoleInfo&);
  RoleInfo& operator=(const RoleInfo&);
  RoleInfo() : areaId(0), isOnline(0), name() {
  }

  virtual ~RoleInfo() throw();
  int32_t areaId;
  bool isOnline;
  std::string name;

  _RoleInfo__isset __isset;

  void __set_areaId(const int32_t val);

  void __set_isOnline(const bool val);

  void __set_name(const std::string& val);

  bool operator == (const RoleInfo & rhs) const
  {
    if (!(areaId == rhs.areaId))
      return false;
    if (!(isOnline == rhs.isOnline))
      return false;
    if (!(name == rhs.name))
      return false;
    return true;
  }
  bool operator != (const RoleInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RoleInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(RoleInfo &a, RoleInfo &b);

inline std::ostream& operator<<(std::ostream& out, const RoleInfo& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _AreaInfo__isset {
  _AreaInfo__isset() : areaId(false), name(false), serverList(false), currentCnt(false) {}
  bool areaId :1;
  bool name :1;
  bool serverList :1;
  bool currentCnt :1;
} _AreaInfo__isset;

class AreaInfo {
 public:

  AreaInfo(const AreaInfo&);
  AreaInfo& operator=(const AreaInfo&);
  AreaInfo() : areaId(0), name(), currentCnt(0) {
  }

  virtual ~AreaInfo() throw();
  int32_t areaId;
  std::string name;
  std::vector<GameServerInfo>  serverList;
  int32_t currentCnt;

  _AreaInfo__isset __isset;

  void __set_areaId(const int32_t val);

  void __set_name(const std::string& val);

  void __set_serverList(const std::vector<GameServerInfo> & val);

  void __set_currentCnt(const int32_t val);

  bool operator == (const AreaInfo & rhs) const
  {
    if (!(areaId == rhs.areaId))
      return false;
    if (__isset.name != rhs.__isset.name)
      return false;
    else if (__isset.name && !(name == rhs.name))
      return false;
    if (__isset.serverList != rhs.__isset.serverList)
      return false;
    else if (__isset.serverList && !(serverList == rhs.serverList))
      return false;
    if (!(currentCnt == rhs.currentCnt))
      return false;
    return true;
  }
  bool operator != (const AreaInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const AreaInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(AreaInfo &a, AreaInfo &b);

inline std::ostream& operator<<(std::ostream& out, const AreaInfo& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _Order__isset {
  _Order__isset() : roleId(false), orderId(false), type(false), goodsCode(false), price(false), state(false), createAt(false), payAt(false) {}
  bool roleId :1;
  bool orderId :1;
  bool type :1;
  bool goodsCode :1;
  bool price :1;
  bool state :1;
  bool createAt :1;
  bool payAt :1;
} _Order__isset;

class Order {
 public:

  Order(const Order&);
  Order& operator=(const Order&);
  Order() : roleId(), orderId(), type(), goodsCode(), price(0), state(), createAt(0), payAt(0) {
  }

  virtual ~Order() throw();
  std::string roleId;
  std::string orderId;
  std::string type;
  std::string goodsCode;
  double price;
  std::string state;
  int64_t createAt;
  int64_t payAt;

  _Order__isset __isset;

  void __set_roleId(const std::string& val);

  void __set_orderId(const std::string& val);

  void __set_type(const std::string& val);

  void __set_goodsCode(const std::string& val);

  void __set_price(const double val);

  void __set_state(const std::string& val);

  void __set_createAt(const int64_t val);

  void __set_payAt(const int64_t val);

  bool operator == (const Order & rhs) const
  {
    if (!(roleId == rhs.roleId))
      return false;
    if (!(orderId == rhs.orderId))
      return false;
    if (!(type == rhs.type))
      return false;
    if (!(goodsCode == rhs.goodsCode))
      return false;
    if (!(price == rhs.price))
      return false;
    if (!(state == rhs.state))
      return false;
    if (__isset.createAt != rhs.__isset.createAt)
      return false;
    else if (__isset.createAt && !(createAt == rhs.createAt))
      return false;
    if (__isset.payAt != rhs.__isset.payAt)
      return false;
    else if (__isset.payAt && !(payAt == rhs.payAt))
      return false;
    return true;
  }
  bool operator != (const Order &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Order & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Order &a, Order &b);

inline std::ostream& operator<<(std::ostream& out, const Order& obj)
{
  obj.printTo(out);
  return out;
}



#endif
