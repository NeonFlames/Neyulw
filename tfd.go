package main

/*
#include <stdlib.h>
#include "extern/tfd/tinyfiledialogs.h"
#include "extern/tfd/tinyfiledialogs.c"
*/
import "C"
import (
	"unsafe"
	"strings"
)

func fileDialog(title string, allow_multiple int) []string {
	t := C.CString(title)
	defer C.free(unsafe.Pointer(t))
	file := C.tinyfd_openFileDialog(t, nil, 0, nil, nil, C.int(allow_multiple))
	defer C.free(unsafe.Pointer(file))
	if allow_multiple != 0 {
		data := C.GoString(file)
		files := strings.Split(data, "|")
		return files
	}
	return []string{C.GoString(file)}
}

// TODO: Uses ancient folder selection on windows for some reason
func folderDialog(title string) string {
	t := C.CString(title)
	defer C.free(unsafe.Pointer(t))
	folder := C.tinyfd_selectFolderDialog(t, nil)
	return C.GoString(folder)
}
