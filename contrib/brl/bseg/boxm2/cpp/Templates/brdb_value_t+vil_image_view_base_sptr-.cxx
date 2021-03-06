#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_image_view.h>
#include <brdb/brdb_value.hxx>

//stubs for binary IO, since we don't need to use it
void vsl_b_read(vsl_b_istream&, vil_image_view_base_sptr&) {}
void vsl_b_write(vsl_b_ostream&, vil_image_view_base_sptr const&) {}

BRDB_VALUE_INSTANTIATE(vil_image_view_base_sptr,"vil_image_view_base_sptr");
