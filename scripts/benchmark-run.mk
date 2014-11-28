PENCIL_RUNTIME=${PENCIL_UTIL_HOME}/runtime

run: ${UPLOAD_FILES}
	../../scripts/run-on-board.sh $^

run-orig: ${UPLOAD_FILES_ORIG}
	../../scripts/run-on-board.sh $^

package: ${UPLOAD_FILES}
	mkdir -p package
	cp $^ package

package-orig: ${UPLOAD_FILES_ORIG}
	mkdir -p package-orig
	cp $^ package-orig

prepare: ${DATA_FILES}
	ssh ${BOARD} "mkdir -p ${BOARD_LIB_DIR}"
	scp ${PENCIL_RUNTIME}/libocl_pencil.so ${BOARD}:${BOARD_LIB_DIR}
ifdef DATA_FILES
	ssh ${BOARD} "mkdir -p ${BOARD_DATA_DIR}"
	scp $^ ${BOARD}:${BOARD_DATA_DIR}/
endif
