namespace csharp world
namespace java jamboy.charge.common.protocol.thrift

/**
 * 操作异常，　
 *　whatOp 表操作枚举值
 *　why 表原因，字符描述
 */
exception InvalidOperationException {
  1: i32 whatOp,
  2: string why
}


