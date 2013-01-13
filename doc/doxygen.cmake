# doxygen helper

set(ENV{SNSTRM_ROOT} ${PDIR})
message(${PDIR})
execute_process(
	COMMAND doxygen "${PDIR}/doc/doxygen")
