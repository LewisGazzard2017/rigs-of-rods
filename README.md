# Rigs of Rods 
## Editor-Python-Dirty branch

Alpha prototype of new RigEditor, fully powered by Python 3.4 with bindings to any necessary functionality.

The idea is to open RigEditor to wider developer base and give it a powerful plugin system, such as the one of Blender (www.blender.org)

The 'dirty' word in the name refers to code-quality: Python bindings are made 
from former AngelScript bindings (also only experimental) and obsoleted C++ 
implementations are left lying all over the place. When the code matures, 
it will be carefully cherry-picked to master branch.

#### Status

Working technical preview:
 1. sets up scene/camera (dark purple background);
 2. draws one LinesMesh with 3 axis-lines. 
 3. stays idle until you pres ESC key to exit to main menu.
 4. user may enter RigEditor any number of times.

#### Documentation

RigEditor uses Sphinx (www.sphinx-doc.org) to generate documentation.

The source text is located in doc/sphinx. 
Build scripts (Makefile + make.bat) are included.