#ifndef vil_copy_h_
#define vil_copy_h_
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vil/vil_copy.h

//:
// \file
// \brief Image copy function
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00


// Modifications
//     000216 AWF Initial version.
//     000217 JS  components*planes because get_section returns RGBRGB

#include <vil/vil_fwd.h>

//: Copy SRC to DST.
// Images must be exactly the same dimensions, but datatype isn't matched
void vil_copy(vil_image const& src, vil_image& DST);

#endif // vil_copy_h_
