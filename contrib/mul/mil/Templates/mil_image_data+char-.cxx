#include <mil/mil_image_data.txx>
MIL_IMAGE_DATA_INSTANTIATE(char); //- This fails wierdly

#if 0
template class mil_image_data<char >;
typedef mil_image_data<char> * mil_image_data_char_ptr;
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<char >& v);
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<char >* v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data_char_ptr& v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data<char >& v);
#endif
