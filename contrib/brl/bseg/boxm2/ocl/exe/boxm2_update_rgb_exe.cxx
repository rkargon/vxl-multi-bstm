#include <bocl/bocl_cl.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_flatten.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_nn_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/pro/boxm2_opencl_render_rgb_process.h>
#include <boxm2/ocl/pro/boxm2_opencl_update_rgb_process.h>

//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"Boxm2 Single Frame RGB Update and Render"<<vcl_endl;
  vul_arg<vcl_string> camfile("-cam", "camera filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> img("-img", "input image", "");
  vul_arg<unsigned> ni("-ni", "Width of output image", 1280);
  vul_arg<unsigned> nj("-nj", "Height of output image", 720);

  // need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

  //load camera from file
  vcl_ifstream ifs(camfile().c_str());
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << camfile() << '\n';
    return -1;
  }
  else  {
    ifs >> *pcam;
  }
  vpgl_camera_double_sptr cam = pcam;
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);

  //load image from file and format it into RGBA
  vil_image_view_base_sptr loaded_image = vil_convert_to_n_planes(4, vil_load(img().c_str()));
  vil_image_view_base_sptr comp_image = vil_convert_to_component_order(loaded_image); 
  vil_image_view<vil_rgba<vxl_byte> >* rgb_view = new vil_image_view<vil_rgba<vxl_byte> >(comp_image); 
  vil_image_view<vil_rgba<vxl_byte> >::iterator iter; 
  for(iter = rgb_view->begin(); iter != rgb_view->end(); ++iter) 
    (*iter) = vil_rgba<vxl_byte>(iter->R(), iter->G(), iter->B(), 255); 
  vil_image_view_base_sptr rgba_sptr(rgb_view);// = new vil_image_view<unsigned int>(ni(), nj());
  vcl_cout<<rgb_view->pixel_format()<<vcl_endl;
  brdb_value_sptr brdb_inimg = new brdb_value_t<vil_image_view_base_sptr>(rgba_sptr);

  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  //start out rendering with the CPU
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);

  //initialize a block and data cache
  boxm2_nn_cache cache( scene.ptr() );

  ////initialize gpu pro / manager
  boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
  gpu_pro->set_scene(scene.ptr());
  gpu_pro->set_cpu_cache(&cache);
  gpu_pro->init();

  //////set inputs
  vcl_vector<brdb_value_sptr> input;
  input.push_back(brdb_scene);
  input.push_back(brdb_cam);
  input.push_back(brdb_inimg);

  ////initoutput vector
  vcl_vector<brdb_value_sptr> output;

  //////initialize GPU update process
  boxm2_opencl_update_rgb_process gpu_update;
  gpu_update.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0]); 
  
  for(int i=0; i<1; i++) 
    gpu_pro->run(&gpu_update, input, output); 

  ////////////////////////////////////////////////////////////////////////////////
  //// RENDER SCENE FOR DEBUGGING
  ////////////////////////////////////////////////////////////////////////////////
  vil_image_view<vil_rgba<vxl_byte> >* expimg = new vil_image_view<vil_rgba<vxl_byte> >(ni(), nj());
  expimg->fill(0);
  vil_image_view_base_sptr expimg_sptr(expimg);
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg_sptr);
 
  //create vis image buffer
  vil_image_view<float>* vis_img = new vil_image_view<float>(ni(), nj());
  vis_img->fill(1.0f);
  brdb_value_sptr brdb_vis = new brdb_value_t<vil_image_view_base_sptr>(vis_img);

  ////set inputs
  vcl_vector<brdb_value_sptr> r_input;
  r_input.push_back(brdb_scene);
  r_input.push_back(brdb_cam);
  r_input.push_back(brdb_expimg);
  r_input.push_back(brdb_vis);

  ////initialize the GPU render process
  boxm2_opencl_render_rgb_process gpu_render;
  gpu_render.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0]);
  gpu_pro->run(&gpu_render, r_input, output);
  
  //vcl_cout<<cache<<vcl_endl;
  
  ////save blocks and data to disk for debugging
  //vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  //vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  //for(iter = blocks.begin(); iter != blocks.end(); ++iter)
  //{ 
    //boxm2_block_id id = iter->first; 
    //boxm2_sio_mgr::save_block(scene->data_path(), cache.get_block(id)); 
    //boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_ALPHA>(id) );
    //boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_MOG3_GREY>(id) );
    //boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_NUM_OBS>(id) );
  //}

  //----------------------------------------------------------------------------
  //------- END API EXAMPLE ----------------------------------------------------
  //----------------------------------------------------------------------------
  //save to disk
  //vil_image_view<vil_rgb<vxl_byte> >* expimg_view = static_cast<vil_image_view<vil_rgb<vxl_byte> >* >(expimg_sptr.ptr());
  for(iter = expimg->begin(); iter != expimg->end(); ++iter) 
    (*iter) = vil_rgba<vxl_byte>(iter->R(), iter->G(), iter->B(), 255); 
  vil_save( *expimg, "exp_image.png"); 

  //vil_image_view<vxl_byte> byte_img(ni(), nj());
  //for (unsigned int i=0; i<ni(); ++i)
    //for (unsigned int j=0; j<nj(); ++j)
      //byte_img(i,j) =  static_cast<vxl_byte>( (*expimg_view)(i,j) );   //just grab the first byte (all foura r the same)
  //vil_save( byte_img, "exp_img.png");

  return 0;
}
