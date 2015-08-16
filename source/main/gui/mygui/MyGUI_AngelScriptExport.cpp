
#include "MyGUI_AngelScriptExport.h"
#include "AngelScriptCallbackSocket.h"

using namespace AngelScript;

// ============================================================================
// Inherited classes
// ============================================================================

class AS_MyGuiCommon_Widget: public AngelScriptNoRefCount
{
public:
	
	void             AS_SetPosition_IntInt(int x, int y)   { this->ToWidget()->setPosition(x,y); }
	MyGUI::IntSize   AS_GetSize()                          { return this->ToWidget()->getSize(); }
	MyGUI::IntSize   AS_GetParentSize()                    { return this->ToWidget()->getParentSize(); }
	MyGUI::Widget*   AS_FindWidget(std::string name)       { return static_cast<MyGUI::Widget*>(this->ToWidget()->findWidget(name)); }
	MyGUI::Widget*   AS_CastType_Widget()                  { return this->ToWidget(); }

protected:
	MyGUI::Widget* ToWidget() { return (MyGUI::Widget*) this; }
};

class AS_MyGuiCommon_EditBox: public AS_MyGuiCommon_Widget
{
public:
	void AS_SetCaption_StdString(std::string text) { this->ToEditBox()->setCaption(text); }
	//void AS_SetVScrollPosition(size_t pos)         { this->ToEditBox()->setVScrollPosition(pos); }
protected:
	MyGUI::EditBox* ToEditBox() { return (MyGUI::EditBox*) this; }
};

class AS_MyGUI_EditBox: public MyGUI::EditBox, public AS_MyGuiCommon_EditBox
{
};

class AS_MyGUI_Button: public MyGUI::Button, public AS_MyGuiCommon_EditBox
{
};

class AS_MyGUI_Window: public MyGUI::Window, public AS_MyGuiCommon_Widget
{
public:
	AS_MyGUI_Window()
	{
		this->eventWindowButtonPressed += MyGUI::newDelegate(this, &AS_MyGUI_Window::CallbackWindowButtonClicked);
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

/// The main widget class
class AS_MyGUI_Widget: public MyGUI::Widget, public AS_MyGuiCommon_Widget
{
public:
	AS_MyGUI_Window* AS_CastType_Window()
	{
		return static_cast<AS_MyGUI_Window*>(this->castType<MyGUI::Window>());
	}
	AS_MyGUI_EditBox* AS_CastType_EditBox()
	{
		return static_cast<AS_MyGUI_EditBox*>(this->castType<MyGUI::EditBox>());
	}
	AS_MyGUI_Button* AS_CastType_Button()
	{
		return static_cast<AS_MyGUI_Button*>(this->castType<MyGUI::Button>());
	}
	
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

AS_MyGUI_VectorWidgetPtr* AS_MyGUI_LayoutManager_LoadLayout(std::string file_path, std::string widget_names_prefix, AS_MyGUI_Widget* parent)
{
	auto widgets = MyGUI::LayoutManager::getInstance().loadLayout(file_path, widget_names_prefix, parent);
	AS_MyGUI_VectorWidgetPtr* out_widgets = new AS_MyGUI_VectorWidgetPtr();
	auto itor = widgets.begin();
	auto iend = widgets.end();
	for(; itor != iend; ++itor)
	{
		out_widgets->push_back(static_cast<AS_MyGUI_Widget*>(*itor));
	}
	out_widgets->AS_RefCountIncrease();
	return out_widgets;
}

void AS_MyGUI_IntSize_Construct(void *memory)
{
	// Initialize the pre-allocated memory by calling the
	// object constructor with the placement-new operator
	new(memory) MyGUI::IntSize();
}

// ============================================================================
// The export
// ============================================================================

#define OBJPROXY_ADD_METHOD(  OBJPROXY, DECL, TYPE, FUNC)  OBJPROXY.AddMethod  (DECL, asMETHOD(TYPE, FUNC));
#define OBJPROXY_ADD_PROPERTY(OBJPROXY, DECL, TYPE, FIELD) OBJPROXY.AddProperty(DECL, asOFFSET(TYPE, FIELD));

void MyGUI_AngelScriptExport::Export(RoR::AngelScriptSetupHelper* A)
{
	// Dummy interface
	// Purpose: to enable creating 'funcdef' for binding AS callbacks to MyGUI events
	// See below: eventWindowButtonClick_BindCallback
	A->RegisterInterface("MyGUI_IEventListener");

	// TYPES

	auto intsize_proxy = A->RegisterObjectWithProxy("MyGUI_IntSize", sizeof(MyGUI::IntSize), asOBJ_VALUE | asOBJ_APP_CLASS | asOBJ_POD);
	intsize_proxy.AddBehavior(asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(AS_MyGUI_IntSize_Construct), asCALL_CDECL_OBJLAST);
	intsize_proxy.AddProperty("int width",  asOFFSET(MyGUI::IntSize, width));
	intsize_proxy.AddProperty("int height", asOFFSET(MyGUI::IntSize, height));

	auto  widget_proxy = RegisterRefObject<AS_MyGUI_Widget> (A, "MyGUI_Widget");
	auto editbox_proxy = RegisterRefObject<AS_MyGUI_EditBox>(A, "MyGUI_EditBox");
	auto  button_proxy = RegisterRefObject<AS_MyGUI_Button> (A, "MyGUI_Button");
	auto  window_proxy = RegisterRefObject<AS_MyGUI_Window> (A, "MyGUI_Window");

	RegisterRefObject<AS_MyGUI_EventHandle_WidgetWidget> (A, "MyGUI_EventHandle_WidgetWidget");
	RegisterRefObject<AS_MyGUI_EventHandle_WidgetVoid>   (A, "MyGUI_EventHandle_WidgetVoid");

	A->RegisterInheritedStdVector<AS_MyGUI_VectorWidgetPtr>("MyGUI_VectorWidgetPtr", "MyGUI_Widget@");

	// CLASS WIDGET

	#define WIDGET_ADD_METHOD(  DECL, TYPE, FUNC)  OBJPROXY_ADD_METHOD(  widget_proxy, DECL, TYPE, FUNC)
	#define WIDGET_ADD_PROPERTY(DECL, TYPE, FIELD) OBJPROXY_ADD_PROPERTY(widget_proxy, DECL, TYPE, FIELD)

	// Please maintain alphabetical order
	WIDGET_ADD_METHOD( "MyGUI_EditBox@ CastType_EditBox()",          AS_MyGUI_Widget,       AS_CastType_EditBox);
	WIDGET_ADD_METHOD( "MyGUI_Button@  CastType_Button()",           AS_MyGUI_Widget,       AS_CastType_Button);
	WIDGET_ADD_METHOD( "MyGUI_Window@  CastType_Window()",           AS_MyGUI_Widget,       AS_CastType_Window);
	WIDGET_ADD_METHOD( "MyGUI_Widget@  FindWidget(string name)",     AS_MyGuiCommon_Widget, AS_FindWidget);
	WIDGET_ADD_METHOD( "MyGUI_IntSize  GetSize()",                   AS_MyGuiCommon_Widget, AS_GetSize);
	WIDGET_ADD_METHOD( "MyGUI_IntSize  GetParentSize()",             AS_MyGuiCommon_Widget, AS_GetParentSize);
	WIDGET_ADD_METHOD( "void           SetPosition(int x, int y)",   AS_MyGuiCommon_Widget, AS_SetPosition_IntInt);
	WIDGET_ADD_METHOD( "void           SetVisible(bool visible)",    MyGUI::Widget,         setVisible);

	// Input
	WIDGET_ADD_PROPERTY( "MyGUI_EventHandle_WidgetWidget@ eventMouseSetFocus",    MyGUI::Widget, eventMouseSetFocus);
	WIDGET_ADD_PROPERTY( "MyGUI_EventHandle_WidgetWidget@ eventMouseLostFocus",   MyGUI::Widget, eventMouseLostFocus);
	WIDGET_ADD_PROPERTY( "MyGUI_EventHandle_WidgetVoid@   eventMouseButtonClick", MyGUI::Widget, eventMouseButtonClick);
	
	// CLASS WINDOW

	#define WINDOW_ADD_METHOD(  DECL, TYPE, FUNC)  OBJPROXY_ADD_METHOD(  window_proxy, DECL, TYPE, FUNC)
	#define WINDOW_ADD_PROPERTY(DECL, TYPE, FIELD) OBJPROXY_ADD_PROPERTY(window_proxy, DECL, TYPE, FIELD)

	// Please maintain alphabetical order
	WINDOW_ADD_METHOD( "MyGUI_Widget@  CastType_Widget()",              AS_MyGuiCommon_Widget, AS_CastType_Widget);
	WINDOW_ADD_METHOD( "void           eventWindowButtonClick_BindCallback(MyGUI_IEventListener@ object, string method_name)", AS_MyGUI_Window, AS_BindCallback_EventWindowButtonClick);
	WINDOW_ADD_METHOD( "MyGUI_Widget@  FindWidget(string name)",        AS_MyGuiCommon_Widget, AS_FindWidget);
	WINDOW_ADD_METHOD( "MyGUI_IntSize  GetParentSize()",                AS_MyGuiCommon_Widget, AS_GetParentSize);
	WINDOW_ADD_METHOD( "MyGUI_IntSize  GetSize()",                      AS_MyGuiCommon_Widget, AS_GetSize);
	WINDOW_ADD_METHOD( "void           SetPosition(int x, int y)",      AS_MyGuiCommon_Widget, AS_SetPosition_IntInt);
	WINDOW_ADD_METHOD( "void           SetVisible(bool visible)",       MyGUI::Widget, setVisible);

	// CLASS EDITBOX

	#define EDITBOX_ADD_METHOD(  DECL, TYPE, FUNC)  OBJPROXY_ADD_METHOD(  editbox_proxy, DECL, TYPE, FUNC)
	#define EDITBOX_ADD_PROPERTY(DECL, TYPE, FIELD) OBJPROXY_ADD_PROPERTY(editbox_proxy, DECL, TYPE, FIELD)

	// Please maintain alphabetical order
	EDITBOX_ADD_METHOD( "MyGUI_Widget@  CastType_Widget()",                AS_MyGuiCommon_Widget,  AS_CastType_Widget);
	EDITBOX_ADD_METHOD( "MyGUI_Widget@  FindWidget(string name)",          AS_MyGuiCommon_Widget,  AS_FindWidget);
	EDITBOX_ADD_METHOD( "MyGUI_IntSize  GetParentSize()",                  AS_MyGuiCommon_Widget,  AS_GetParentSize);
	EDITBOX_ADD_METHOD( "MyGUI_IntSize  GetSize()",                        AS_MyGuiCommon_Widget,  AS_GetSize);
	EDITBOX_ADD_METHOD( "void           SetCaption(string caption)",       AS_MyGuiCommon_EditBox, AS_SetCaption_StdString);
	EDITBOX_ADD_METHOD( "void           SetHScrollPosition(uint32 index)", MyGUI::EditBox,         setHScrollPosition);
	EDITBOX_ADD_METHOD( "void           SetPosition(int x, int y)",        AS_MyGuiCommon_Widget,  AS_SetPosition_IntInt);
	EDITBOX_ADD_METHOD( "void           SetVisible(bool visible)",         MyGUI::Widget,          setVisible);
	EDITBOX_ADD_METHOD( "void           SetVScrollPosition(uint32 index)", MyGUI::EditBox,         setVScrollPosition);

	// CLASS BUTTON

	#define BUTTON_ADD_METHOD(  DECL, FUNC)  button_proxy.AddMethod  (DECL, asMETHOD(AS_MyGUI_Button, FUNC));
	#define BUTTON_ADD_PROPERTY(DECL, FIELD) button_proxy.AddProperty(DECL, asOFFSET(AS_MyGUI_Button, FIELD));

	//BUTTON_ADD_METHOD( "MyGUI_Widget@  CastType_Widget()",                AS_CastType_Widget);
	//BUTTON_ADD_METHOD( "MyGUI_Widget@  FindWidget(string name)",          AS_FindWidget);
	//BUTTON_ADD_METHOD( "MyGUI_IntSize  GetSize()",                        AS_GetSize);
	//BUTTON_ADD_METHOD( "void           SetCaption(string caption)",       AS_SetCaption_StdString);
	//BUTTON_ADD_METHOD( "MyGUI_IntSize  GetParentSize()",                  AS_GetParentSize);
	//BUTTON_ADD_METHOD( "void           SetPosition(int x, int y)",        AS_SetPosition_IntInt);
	//BUTTON_ADD_METHOD( "void           SetVisible(bool visible)",         setVisible);

	// LAYOUT HANDLING

	A->RegisterGlobalFunction(
		"MyGUI_VectorWidgetPtr@ MyGUI_LayoutManager_LoadLayout(string file_path, string widget_names_prefix, MyGUI_Widget@ parent_widget)",
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
