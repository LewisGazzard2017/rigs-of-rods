.. RigEditor documentation master file, created by
   sphinx-quickstart on Wed Sep 23 11:16:49 2015.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

RigEditor - Python interface
============================

Intro
-----

This is Python interface of RigEditor, a built-in content editor 
from `Rigs of Rods <http://rigsofrods.github.io>`_ project. Rigs of Rods (called "RoR")
is an open-source hardcore vehicle simulator fully driven by soft-body physics 
calculations. Creating the soft-body structures, called "rigs", is
the job of RigEditor.

RigEditor is very early in development (hence version "0" displayed here)
and strives to become flexible and easily extensible for maximum productivity
of both it's users and developers.

The idea is to implement as much functionality as possible directly in
Python, exposing only a necessary minimum of functionality from the
hosting application.

Background
----------

To understand the current state and future outlook of this project, it's good
to look into it's (brief!) history.

RoR has been around for a decade now and until very recently it never had
a built-in editor. Content creators had to rely on external editors,
Blender mesh converters or plain text file editing.

The RigEditor was initiated around a year ago as a monolithic C++ application.
The idea was to quickly create a prototype editor suite to perform
basic modding tasks. The development progressed quickly in the beginning,
RigEditor was capable of loading, displaying and interactively modifying
existing rigs. However, support for adding new elements was either very 
experimental or completely missing. 
For example, RigEditor learned to modify wheels 
(very well, actually), but it never got the ability to add them.

It was only recently the author realized the only way to ever make
RigEditor a compelling tool is to open it to wider developer base and ease
the dev. process by making it script-driven. The language of choice
was Python because of it's popularity and also a very strong
success-story: `Blender <http://www.blender.org>`_
 
 
Python modules
--------------

.. toctree::
   :maxdepth: 2
   
   application
   inputs
   camera
   euclid3


Built-in modules
----------------

.. toctree::
   :maxdepth: 2
   
   ror_truckfile
   ror_drawing
   ror_system
   



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

