
#pragma once

// Assumes boost::python is included and namespace is used.
#define PYTHON_REGISTER_VECTOR_SUITE(VEC_CLASS, PYNAME)  \
	class_< VEC_CLASS >(PYNAME)                          \
		.def(vector_indexing_suite< VEC_CLASS >());      \

// Assumes boost::python is included and namespace is used.
#define PYTHON_REGISTER_STD_VECTOR(CLASS, PYNAME)            \
	PYTHON_REGISTER_VECTOR_SUITE(std::vector<CLASS>, PYNAME) \

// Assumes boost::python is included and namespace is used.
#define PYTHON_REGISTER_SHARED_PTR(CLASS)                 \
    register_ptr_to_python< boost::shared_ptr<CLASS> >(); \

