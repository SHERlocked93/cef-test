#pragma once
#include "include/cef_v8.h"

class V8Handler : public CefV8Handler {
 public:
  V8Handler() = default;

  //! ȫ��ע�᷽��ִ��ʱ��Ļص�
  //! \param name ��������ex:nativeCall
  //! \param object JS�����������ʱ��this
  //! \param arguments JS�����������ʱ���ݵĲ���
  //! \param retval [out] ����ִ�����CPP���ظ�JS�ķ���ֵ
  //! \param exception [out] ����ִ�й����г��ֵ��쳣����JS����Ϊerror�׳�
  virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval, CefString& exception) override;

 private:
  IMPLEMENT_REFCOUNTING(V8Handler);
};
