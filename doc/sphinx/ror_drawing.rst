:mod:`ror_drawing` --- 3D object drawing interface
==================================================

.. module:: ror_drawing
   :synopsis: Simplistic 3D object creating and displaying.

.. moduleauthor:: Petr Ohlidal <_myname_@_mysurname_.cz>

This module contains basic functionality to dynamically create
and manipulate 3D objects, called "meshes".

At the moment, only thin lines and points (adjustable size) can be drawn. 
This should be enough for displaying the edited rig and helper handles.

General usage
-------------

Procedure:

1. Create a mesh object of specific type (provide configuration if needed).
2. Call begin_update(). This clears all previous mesh content.
3. Add elements (like points and lines, depending on mesh type).
4. Call end_update(). The mesh will remain unchanged until you call
   begin_update() again.
5. Eventually, call set_position() (default is 0,0,0)
6. Call attach_to_scene() to make the object visible. To hide it, call
   detach_from_scene().

Lines and points
----------------

.. function:: create_lines_mesh()

   Creates instance of :class:`LinesMesh`
   
   

.. class:: LinesMesh()

   Draws pixel-wide lines (not configurable). Colors are specified for each
   end separately, automatically creating gradients.
   
   Non-instantiable, use function :func:`create_lines_mesh` instead.
   
   .. method:: begin_update()
      
      Clears all mesh content. Must be called before add_line() otherwise
      results are undefined.
      
   .. method:: add_line(position1, color1, position2, color2)
   
      Adds a line. 
      Arguments: "position" objects must have {x, y, z} attributes,
      "color" objects must have {r, g, b} attributes.
      
      .. note:: 
         You must call begin_update() prior to this function,
         Otherwise results are undefined.
         
   .. method:: end_update()
   
      Persists the modifications made with add_line()
      
   .. method:: attach_to_scene()
   
      Attaches the mesh to scene graph, which makes it visible.
      
   .. method:: detach_from_scene()
   
      Detaches the mesh from scene graph, which effectively hides it.      
      
      

.. function:: create_points_mesh(point_size)

   Creates instance of :class:`PointsMesh`
      


.. class:: PointsMesh()

   Draws points with configurable size. Color is specified per-point.
   
   Non-instantiable, use function :func:`create_points_mesh` instead.
   
   .. method:: begin_update()
      
      Clears all mesh content. Must be called before add_point() otherwise
      results are undefined.
      
   .. method:: add_point(position, color)
   
      :attr position: :class:`euclid3.Vector3` object must have {x, y, z} attributes
      :attr color: :class:`datatypes.Color` object must have {r, g, b} attributes.
      
      .. note:: 
         You must call begin_update() prior to this function,
         Otherwise results are undefined.
         
   .. method:: end_update()
   
      Persists the modifications made with add_point()      

   .. method:: attach_to_scene()
   
      Attaches the mesh to scene graph, which makes it visible.
      
   .. method:: detach_from_scene()
   
      Detaches the mesh from scene graph, which effectively hides it.

