
#include "MyGUI_AngelScriptExport.h"

#include "AngelScriptCallbackSocket.h"

using namespace AngelScript;

// ============================================================================
// Inherited classes
// ============================================================================

#define DEFINE_CAST_METHOD(TARGET_WIDGET_NAME)            \
AS_MyGUI_##TARGET_WIDGET_NAME* AS_CastType_##TARGET_WIDGET_NAME()					  \
{														  \
	return static_cast<AS_MyGUI_##TARGET_WIDGET_NAME*>(this->castType<MyGUI::TARGET_WIDGET_NAME>());			  \
}

/// The main widget class
class AS_MyGUI_Widget: public MyGUI::Widget, public AngelScriptNoRefCount
{
public:
	DEFINE_CAST_METHOD(Window);
	DEFINE_CAST_METHOD(EditBox);
	DEFINE_CAST_METHOD(Button);
};

class AS_MyGUI_EditBox: public MyGUI::EditBox, public AngelScriptNoRefCount
{};

class AS_MyGUI_Button: public MyGUI::Button, public AngelScriptNoRefCount
{}

class AS_MyGUI_Window: public MyGUI::Window, public AngelScriptNoRefCount
{
public:
	AS_MyGUI_Window()
	{
		this->eventWindowButtonPressed += MyGUI::newDelegate(this, CallbackWindowButtonClicked);
	}

	void AS_BindCallback_EventWindowButtonClick(asIScriptObject* obj, std::string func_name)
	{
		m_event_window_button_click.RegisterCallback(obj, func_name);
	}
private:
	void CallbackWindowButtonClicked(MyGUI::Window* win, std::string const & btn_name_ref)
	{
		if (! m_event_window_button_click.IsBound())
		{
			return;
		}
		std::string btn_name = btn_name_ref; // Pirate copy :D

		auto ctx = m_event_window_button_click.PrepareContext();
		m_event_window_button_click.SetArgObject(ctx, 0, static_cast<void*>(win));
		m_event_window_button_click.SetArgObject(ctx, 1, static_cast<void*>(&btn_name));
		m_event_window_button_click.ExecuteContext(ctx);
	}

	RoR::AngelScriptCallbackSocket m_event_window_button_click;
};

/// Vector of widget-pointers
/// Essential to export LayoutManager::loadLayout(XML_LAYOUT_FILE) function
INHERIT_STD_VECTOR(AS_MyGUI_VectorWidgetPtr, AS_MyGUI_Widget*);

/// Delegate template system - used for binding event callbacks to widgets
class AS_MyGUI_Delegate
{
public:
	AS_MyGUI_Delegate(asIScriptFunction* func, asIScriptObject* obj, asIScriptContext* ctx):
		m_as_function(func),
		m_as_context(ctx),
		m_as_object(obj)
		{}

	virtual ~AS_MyGUI_Delegate()
	{
		if (m_as_context)
		{
			//delete m_as_context; // I failed to find the API to destroy context...
		}
	}

	virtual bool compare(AS_MyGUI_Delegate* other)
	{
		return (other->m_as_function == this->m_as_function) && (other->m_as_object == this->m_as_object);
	}

	virtual void invoke(MyGUI::Widget* widget)
	{
		m_as_context->Prepare(m_as_function);
	}

	virtual bool compare(MyGUI::delegates::IDelegateUnlink* _unlink) const
	{
		return false;
	}
protected:
	asIScriptFunction* m_as_function;
	asIScriptObject*   m_as_object;
	asIScriptContext*  m_as_context; //<! Sub-optimal; temporary solution
};

class AS_MyGUI_Delegate_WidgetVoid: 
	public MyGUI::delegates::IDelegate1<MyGUI::Widget*>,
	public AS_MyGUI_Delegate
{
public:
	AS_MyGUI_Delegate_WidgetVoid(asIScriptFunction* func, asIScriptObject* obj, asIScriptContext* ctx):
		AS_MyGUI_Delegate(func, obj, ctx)
		{}

	// <typeid> should be disabled... forever, if possible... >:(
	virtual bool isType( const std::type_info& _type)
	{
		return _type == typeid(AS_MyGUI_Delegate_WidgetVoid);
	}

	virtual bool compare(MyGUI::delegates::IDelegate1<MyGUI::Widget*>* other) const
	{
		return this->compare(other);
	}

	virtual void invoke(MyGUI::Widget* widget)
	{
		// Ugly error handling
		int result = m_as_context->Prepare(m_as_function);
		if (result != 0)
		{
			throw std::runtime_error("AngelScript/MyGUI: failed to invoke Delegate function");
		}
		result = m_as_context->SetArgAddress(0, widget);
		if (result != 0)
		{
			throw std::runtime_error("AngelScript/MyGUI: failed to invoke Delegate function");
		}
		result = m_as_context->Execute();
		if (result != 0)
		{
			throw std::runtime_error("AngelScript/MyGUI: failed to Execute() Delegate function");
		}
	}
};

class AS_MyGUI_Delegate_WidgetWidget: 
	public MyGUI::delegates::IDelegate2<MyGUI::Widget*, MyGUI::Widget*>,
	public AS_MyGUI_Delegate
{
public:
	AS_MyGUI_Delegate_WidgetWidget(asIScriptFunction* func, asIScriptObject* obj, asIScriptContext* ctx):
		AS_MyGUI_Delegate(func, obj, ctx)
		{}

	// <typeid> should be disabled... forever, if possible... >:(
	virtual bool isType( const std::type_info& _type)
	{
		return _type == typeid(AS_MyGUI_Delegate_WidgetWidget);
	}

	virtual bool compare(MyGUI::delegates::IDelegate2<MyGUI::Widget*, MyGUI::Widget*>* other) const
	{
		return this->compare(other);
	}

	virtual void invoke(MyGUI::Widget* widget_a, MyGUI::Widget* widget_b)
	{
		// Ugly error handling
		int result = m_as_context->Prepare(m_as_function);
		if (result != 0)
		{
			throw std::runtime_error("AngelScript/MyGUI: failed to invoke Delegate function");
		}
		result = m_as_context->SetArgAddress(0, widget_a);
		if (result != 0)
		{
			throw std::runtime_error("AngelScript/MyGUI: failed to invoke Delegate function");
		}
		result = m_as_context->SetArgAddress(1, widget_b);
		if (result != 0)
		{
			throw std::runtime_error("AngelScript/MyGUI: failed to invoke Delegate function");
		}
		result = m_as_context->Execute();
		if (result != 0)
		{
			throw std::runtime_error("AngelScript/MyGUI: failed to Execute() Delegate function");
		}
	}
};


class AS_MyGUI_EventHandle_WidgetWidget: 
	public MyGUI::EventHandle_WidgetWidget,
	public AngelScriptNoRefCount
{
public:
	void AS_AddDelegate(asIScriptObject* object, asIScriptFunction* function)
	{
		asIScriptEngine* engine = function->GetEngine();
		asIScriptContext* ctx = engine->CreateContext();
		if (ctx == nullptr)
		{
			throw std::runtime_error("AngelScript/MyGUI: Failed to bind function to widget");
		}
		AS_MyGUI_Delegate_WidgetWidget* _delegate = new AS_MyGUI_Delegate_WidgetWidget(function, object, ctx);

		this->operator+=(_delegate);
	}
};

class AS_MyGUI_EventHandle_WidgetVoid: 
	public MyGUI::EventHandle_WidgetVoid,
	public AngelScriptNoRefCount
{
	void AS_AddDelegate(asIScriptObject* object, asIScriptFunction* function)
	{
		asIScriptEngine* engine = function->GetEngine();
		asIScriptContext* ctx = engine->CreateContext();
		if (ctx == nullptr)
		{
			throw std::runtime_error("AngelScript/MyGUI: Failed to bind function to widget");
		}
		AS_MyGUI_Delegate_WidgetVoid* _delegate = new AS_MyGUI_Delegate_WidgetVoid(function, object, ctx);

		this->operator+=(_delegate); // Valid according to C++ standard, amusing according to human programmer =)
	}
};

// ============================================================================
// Interface wrappers
// ============================================================================

AS_MyGUI_VectorWidgetPtr AS_MyGUI_LayoutManager_LoadLayout(std::string file_path, std::string widget_names_prefix, AS_MyGUI_Widget* parent)
{
	auto widgets = MyGUI::LayoutManager::getInstance().loadLayout(file_path, widget_names_prefix, parent);
	AS_MyGUI_VectorWidgetPtr out_widgets;
	out_widgets.assign(widgets.begin(), widgets.end());
	return out_widgets;
}

// ============================================================================
// The export
// ============================================================================

void MyGUI_AngelScriptExport::Export(RoR::AngelScriptSetupHelper* A)
{
	A->RegisterInterface("MyGUI_IEventListener");

	auto  widget_proxy = RegisterRefObject<AS_MyGUI_Widget> (A, "MyGUI_Widget");
	auto editbox_proxy = RegisterRefObject<AS_MyGUI_EditBox>(A, "MyGUI_EditBox");
	auto  button_proxy = RegisterRefObject<AS_MyGUI_Button> (A, "MyGUI_Button");
	auto  window_proxy = RegisterRefObject<AS_MyGUI_Window> (A, "MyGUI_Window");

	// Cast methods
	widget_proxy.AddMethod("MyGUI_EditBox@ CastType_EditBox()", asMETHOD(AS_MyGUI_Widget, AS_CastType_EditBox));
	widget_proxy.AddMethod(" MyGUI_Button@ CastType_Button()",  asMETHOD(AS_MyGUI_Widget, AS_CastType_Button));
	widget_proxy.AddMethod(" MyGUI_Window@ CastType_Window()",  asMETHOD(AS_MyGUI_Widget, AS_CastType_Window));

	// Input
	RegisterRefObject<MyGUI_EventHandle_WidgetWidget> (A, "MyGUI_EventHandle_WidgetWidget");
	RegisterRefObject<MyGUI_EventHandle_WidgetVoid")>(A, "MyGUI_EventHandle_WidgetVoid");
	
	A->RegisterObjectProperty("MyGUI_Widget", "MyGUI_EventHandle_WidgetWidget@ eventMouseSetFocus",    asOFFSET(AS_MyGUI_Widget, eventMouseSetFocus));
	A->RegisterObjectProperty("MyGUI_Widget", "MyGUI_EventHandle_WidgetWidget@ eventMouseLostFocus",   asOFFSET(AS_MyGUI_Widget, eventMouseLostFocus));
	A->RegisterObjectProperty("MyGUI_Widget", "MyGUI_EventHandle_WidgetVoid@   eventMouseButtonClick", asOFFSET(AS_MyGUI_Widget, eventMouseButtonClick));
	
	window_proxy.AddMethod(
		"void BindCallback_EventWindowButtonClick(MyGUI_IEventListener@ object, string method_name)",
		asMETHOD(AS_MyGUI_Window, AS_BindCallback_EventWindowButtonClick), asCALL_THISCALL);
	
	A->RegisterObjectMethod("MyGUI_Window", 
		
	
	A->RegisterInheritedStdVector<AS_MyGUI_VectorWidgetPtr>("MyGUI_VectorWidgetPtr", "MyGUI_Widget@");
	A->RegisterGlobalFunction(
		"MyGUI_VectorWidgetPtr MyGUI_LayoutManager_LoadLayout(string file_path, string widget_names_prefix, MyGUI_Widget@ parent_widget)",
		asFUNCTION(AS_MyGUI_LayoutManager_LoadLayout), asCALL_CDECL);
}

// ============================================================================
// Helpers
// ============================================================================

template<typename T>
RoR::AngelScriptSetupHelper::ObjectRegistrationProxy MyGUI_AngelScriptExport::RegisterRefObject(RoR::AngelScriptSetupHelper* A, const char* obj_name)
{
	using namespace AngelScript;
	auto proxy = A->RegisterObjectWithProxy(obj_name, 0, asOBJ_REF );
	proxy.AddBehavior(asBEHAVE_ADDREF, "void f()", asMETHOD(T, AS_AddRef), asCALL_THISCALL);
	proxy.AddBehavior(asBEHAVE_RELEASE, "void f()", asMETHOD(T, AS_ReleaseRef), asCALL_THISCALL);
	return proxy;
}
