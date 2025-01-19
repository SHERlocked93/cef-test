#pragma once
#include "include/cef_v8.h"

class V8Handler : public CefV8Handler {
 public:
  V8Handler() = default;

  //! 全局注册方法执行时候的回调
  //! \param name 方法名，ex:nativeCall
  //! \param object JS调用这个方法时的this
  //! \param arguments JS调用这个方法时传递的参数
  //! \param retval [out] 方法执行完后CPP返回给JS的返回值
  //! \param exception [out] 方法执行过程中出现的异常，在JS中作为error抛出
  virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval, CefString& exception) override;

 private:
  IMPLEMENT_REFCOUNTING(V8Handler);
};
