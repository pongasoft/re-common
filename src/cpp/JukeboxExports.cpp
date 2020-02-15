#include "Jukebox.h"
#include <cstring>
#include "CommonDevice.h"
#include "JukeboxExports.h"

/**
* Note from forum: https://www.propellerheads.se/forum/showpost.php?p=1585993&postcount=15
*
* Luckily you have full debugging with Xcode, which should eliminate much of the need for caveman debugging.
* While compiled for the IDE, you can use sprintf() as much as you like (on Mac, the Xcode printf() will be sent to
* the debug console) and you can use the presence of preprocessor macro __phdsp__ to turn all that stuff
* off for the Jukebox LLVM builds
*/

/**
* @brief	This is our entry point when we're calling jbox.make_native_object_rw from realtime_controller.lua.
* @details	Note that we pass a parameter - the instance id. The code below demonstrates how to it up even though we
*			currently don't use it.
*/
void *JBox_Export_CreateNativeObject(const char iOperation[], const TJBox_Value iParams[], TJBox_UInt32 iCount) {

  if (std::strcmp(iOperation, "Instance") == 0) {
    JBOX_TRACE("CreateNativeObject / Instance");
    return createDevice(iParams, iCount);
  }

  JBOX_ASSERT_MESSAGE(false, "Unknown operation passed to CreateNativeObject");
  return NULL;
}

/**
* @brief	This is the callback we get from Reason/Recon to do our audio rendering
*/
void JBox_Export_RenderRealtime(void *privateState, const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount) {

  // For the CommonDevice, we always return non-null native objects in JBox_Export_CreateNativeObject and we
  // never call make_empty_native_object() from lua

  if (privateState == NULL) {
    return;
  }
  //JBOX_ASSERT(privateState != NULL);

  CommonDevice *pi = reinterpret_cast<CommonDevice *>(privateState);
  pi->renderBatch(iPropertyDiffs, iDiffCount);
}
