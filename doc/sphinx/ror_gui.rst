:mod:`ror_gui` --- Graphical user interface
===========================================

.. module:: ror_gui
   :synopsis: Graphical user interface.

.. moduleauthor:: Petr Ohlidal <_myname_@_mysurname_.cz>

This module wraps RoR's GUI library, `MyGUI <http://www.mygui.info>`_ and tries
to follow it's design and idioms as closely as possible. All MyGUI classes
and functions are prefixed "MyGUI", extensions are without prefix.

MyGUI comes with an editor, dubbed LayoutEditor, available for download in
"demos and samples" package. The editor produces XML layout files which
can be easily loaded and processed by RigEditor.

A lot of useful MyGUI resources can be found 
at OGRE wiki: http://www.ogre3d.org/tikiwiki/tiki-index.php?page=MyGUI

Layout workflow
---------------

MyGUI defines an XML layout format which can define any GUI window or panel.
It also comes with an editor, dubbed LayoutEditor, available for download in
"demos and samples" package.

For some examples, check layout files of MyGUI's LayoutEditor itself: 
https://github.com/MyGUI/mygui/tree/master/Media/Tools/LayoutEditor

.. function:: load_layout(file_path, widget_names_prefix = "", parent_widget = None)

   Loads an XML layout file and returns a list 
   of root widgets - widgets with no parent.
   
   Wrapper around MyGUI::LayoutManager::loadLayout()
   
Input handling
--------------

The GUI is updated automatically 
when :func:`ror_system.capture_input_and_update_gui` is called.

Widget events are exposed through "EventHandle" objects. EventHandles are
available as attributes of Widget objects and act like containers of
callback function + object bindings. These bindings are called "delegates"
in MyGUI.

.. class :: MyGUI_EventHandle

   Container for callback bindings.

   .. method :: add_delegate(object, function)
   
      Add callback binding.
      
      :param object object: Will be passed as first parameter if not None.
         If None, it won't be passed at all.
      
      :param function function: must conform to interface specified per-event. 
         See :class:`MyGUI_Widget` for detailed reference.  

Widgets
-------

MyGUI defines a hierarchy of widget types.

.. class :: MyGUI_Widget

   Base class for all widget types.
   
   Managed by WidgetManager (not exported yet.)
   
   .. method :: get_name()
   
      :returns: str; arbitrary name assigned to this widget.
      
   .. method :: find_widget(name)
   
      Finds child widget by name.
       
      :rtype: :class:`MyGUI_Widget`
   
   .. method :: is_root_widget()
   
      :rtype: bool
   
   .. method :: get_parent()
   
      :rtype: :class:`MyGUI_Widget`
   
   .. method :: is_visible()
   
      :rtype: bool
      
   .. method :: set_visible(visible)
   
   .. method :: is_enabled()
   
      :rtype: bool
   
   .. method :: set_enabled(enabled)         
   
   **Event handles:**
   
   Instances of :class:`MyGUI_EventHandle` designated to specific events.      
   
   .. attribute :: event_mouse_set_focus
   
      Mouse cursor was placed above the widget.
      
      Callback arguments: (sender_widget, previous_focused_widget)
   
   .. attribute :: event_mouse_lost_focus
   
      Mouse cursor moved away from the widget.
      
      Callback arguments: (sender_widget, new_focused_widget)
   
   .. attribute :: event_mouse_button_click
   
      Callback arguments: (sender_widget)