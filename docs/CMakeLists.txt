# doxygen
find_package(Doxygen REQUIRED dot)
IF(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf/doxyfile.in 
					${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY)
	SET(DOXYGEN_INPUT "${CMAKE_CURRENT_BINARY_DIR}/doxyfile")
	SET(DOXYGEN_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/doxygen")

	ADD_CUSTOM_COMMAND(
		OUTPUT ${DOXYGEN_OUTPUT}
		COMMAND ${CMAKE_COMMAND} -E echo_append "Building with Doxygen the API documentation..."
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
		COMMAND ${CMAKE_COMMAND} -E echo "Done."
		DEPENDS ${DOXYGEN_INPUT})

	ADD_CUSTOM_TARGET(doxygen_docs ALL DEPENDS ${DOXYGEN_OUTPUT})

	ADD_CUSTOM_TARGET(doxygen_docs_forced
		COMMAND ${CMAKE_COMMAND} -E echo_append "Building with Doxygen the API documentation..."
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
		COMMAND ${CMAKE_COMMAND} -E echo "Done.")
ELSE(DOXYGEN_FOUND)
	message("Doxygen needs to be installed to generate the API documentation")
ENDIF(DOXYGEN_FOUND)
# Sphinx
find_package(Sphinx REQUIRED)
if(SPHINX_FOUND)
	if(NOT DEFINED SPHINX_THEME)
		SET(SPHINX_THEME pydata_sphinx_theme)
	endif()

	if(NOT DEFINED SPHINX_THEME_DIR)
		SET(SPHINX_THEME_DIR)
	endif()

	# configured documentation tools and intermediate build results
	SET(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/sphinx")

	# Sphinx cache with pickled ReST documents
	SET(SPHINX_CACHE_DIR "${BINARY_BUILD_DIR}/doctrees")

	# HTML output directory
	SET(SPHINX_HTML_DIR "${BINARY_BUILD_DIR}/html")

	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conf/conf.py.in"
			"${BINARY_BUILD_DIR}/conf.py" @ONLY)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conf/index.rst.in"
			"${BINARY_BUILD_DIR}/index.rst" @ONLY)
	add_custom_target(copy_rsts
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/sphinx/source
			COMMENT "copying *.rst files..")
	add_custom_target(copy_versions
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/conf/versions.json" "${SPHINX_HTML_DIR}/_static/versions.json"
			COMMENT "copying versions.json files..")
	add_custom_target(docs ALL
			COMMAND ${SPHINX_EXECUTABLE} -b html
#	        -c "${BINARY_BUILD_DIR}"
			-d "${SPHINX_CACHE_DIR}"
			"${BINARY_BUILD_DIR}"
			"${SPHINX_HTML_DIR}"
		COMMENT "Building HTML documentation with Sphinx")
	add_dependencies(docs copy_rsts copy_versions doxygen_docs)
ELSE(SPHINX_FOUND)
	message("Using doxygen html, Sphinx needs to be installed to generate the documentation")
endif(SPHINX_FOUND)