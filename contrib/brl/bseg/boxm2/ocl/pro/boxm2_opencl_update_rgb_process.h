#ifndef boxm2_opencl_update_rgb_process_h
#define boxm2_opencl_update_rgb_process_h
//:
// \file
#include <boxm2/ocl/pro/boxm2_opencl_process_base.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vcl_vector.h>
#include <boxm2/boxm2_data_traits.h>

//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>



class boxm2_opencl_update_rgb_process : public boxm2_opencl_process_base
{
  public:
    //PASS ENUM
    enum {
      UPDATE_SEGLEN = 0,
      UPDATE_COMPRESS_RGB = 1,
      UPDATE_PREINF = 2,
      UPDATE_PROC   = 3,
      UPDATE_BAYES  = 4, 
      UPDATE_CELL   = 5, 
    }; 
      
    boxm2_opencl_update_rgb_process() : image_(0), 
                                        vis_image_(0), 
                                        pre_image_(0), 
                                        alpha_int_image_(0), 
                                        norm_image_(0) {}
    
    //: process init and execute
    bool init() { return true; }
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    bool clean(); 

    //: opencl specific init - compiles kernels associated with this process
    virtual bool init_kernel(cl_context* context, cl_device_id* device, vcl_string opts="");

  private:

    //: render kernel (other processes may have many kernels
    vcl_vector<bocl_kernel*> update_kernels_;

    //: workspace 
    vcl_size_t lThreads_[2]; 
    vcl_size_t gThreads_[2]; 
    vcl_size_t img_size_[2]; 

    //: INPUT IMAGE: 
    bocl_mem* image_;             //will be a uchar4 image, RBG
    bocl_mem* vis_image_;         //will be a float image, maintains visibility between blocks
    bocl_mem* pre_image_;         //pre_image
    bocl_mem* alpha_int_image_;   //alpha_int_image
    bocl_mem* norm_image_;        //result of proc_norm_image 

    //: block stuff
    bocl_mem* blk_info_;          
    bocl_mem* blk_;               
    bocl_mem* alpha_;
    bocl_mem* mog_;               //mixture of TWO RGB guassians 
    bocl_mem* num_obs_; 
    bocl_mem* aux_;
    
    //: cam
    bocl_mem* persp_cam_; 
    bocl_mem* img_dim_;
    bocl_mem* cl_output_;
    bocl_mem* lookup_; 
    
    //: app density used for proc_norm_image
    bocl_mem* app_density_; 
    
    //----- Update Helper Methods ----------------------------------------------
    //: set args
    bool set_args(unsigned pass); 
    
    //: Set workspace helper method
    bool set_workspace(unsigned pass);
    
    //: write input image to buffer
    bool write_input_image(vil_image_view<vil_rgba<vxl_byte> >* input_image); 

};



#endif
