#include "V8Handler.h"

bool V8Handler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
                        CefRefPtr<CefV8Value>& retval, CefString& exception) {
  auto msgName = arguments[0]->GetStringValue();

  CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(msgName);  // �������̼���Ϣ

  CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
  context.get()->GetFrame()->SendProcessMessage(PID_BROWSER, msg);  // ����Ⱦ���̷������������

  return true;  // ��ʾ�Ѵ����������
};
