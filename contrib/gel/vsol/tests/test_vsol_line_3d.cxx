//*****************************************************************************
// File name: test_vsol_line_3d.cxx
// Description: Test the vsol_line_3d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/08| Fran�ois BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsol/vsol_line_3d.h>
#include <vsol/vsol_point_3d.h>
//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  vcl_cout<<"Constructor from extremities"<<vcl_endl;
  vsol_point_3d_sptr p=new vsol_point_3d(10,4,1);
  vsol_point_3d_sptr q=new vsol_point_3d(5,1,-23);
  vsol_line_3d_sptr a=new vsol_line_3d(p,q);

  assert(*(a->p0())==*p);
  assert(*(a->p1())==*q);

  vcl_cout<<"Copy constructor"<<vcl_endl;
  vsol_line_3d_sptr b=new vsol_line_3d(*a);

  vcl_cout<<"== operator"<<vcl_endl;
  assert(*a==*b);

  vcl_cout<<"Constructor from direction and middle point"<<vcl_endl;
  vgl_vector_3d<double> v(10,-2,11);
  // choose the above numbers to obtain a "round" length: 15.

  vsol_line_3d_sptr c=new vsol_line_3d(v,p);

  vcl_cout<<"middle()"<<vcl_endl;
  assert(*(c->middle())==*p);
  vcl_cout<<"direction()"<<vcl_endl;
  assert(c->direction()==v);

  vcl_cout<<"length() = "<<c->length()<<vcl_endl;
  assert(c->length()==15);

  vcl_cout<<"set_length()"<<vcl_endl;
  c->set_length(90);
  assert(c->length()==90); // this could fail because of rounding errors
  // set_length() doesn't change the middle point
  assert(*(c->middle())==*p);

  vcl_cout<<"set_p0()"<<vcl_endl;
  c->set_p0(p);
  assert(*(c->p0())==*p);

  vcl_cout<<"set_p1()"<<vcl_endl;
  c->set_p1(q);
  assert(*(c->p1())==*q);

  return 0;
}
