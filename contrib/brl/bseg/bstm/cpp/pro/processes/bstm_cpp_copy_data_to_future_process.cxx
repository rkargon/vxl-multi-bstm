// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_copy_data_to_future_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for copying the data in the input time step into all future time nodes
//
// \author Ali Osman Ulusoy
// \date October 20, 2013

#include <vcl_fstream.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <bstm/cpp/algo/bstm_copy_data_to_future_function.h>

#include <bstm/bstm_util.h>

namespace bstm_cpp_copy_data_to_future_process_globals
{
  const unsigned n_inputs_ =  3;
  const unsigned n_outputs_ = 0;
}

bool bstm_cpp_copy_data_to_future_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_copy_data_to_future_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //time


  // process has 0 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_copy_data_to_future_process(bprb_func_process& pro)
{

  using namespace bstm_cpp_copy_data_to_future_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  float time =pro.get_input<float>(i++);


  //bstm app query
  vcl_string app_data_type;
  int apptypesize;
  vcl_vector<vcl_string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, app_data_type, apptypesize ) ) {
    vcl_cout<<"bstm_cpp_copy_data_to_future_process ERROR: scene doesn't have BSTM_MOG6_VIEW_COMPACT or BSTM_MOG3_GREY or BSTM_GAUSS_RGB data type"<<vcl_endl;
    return false;
  }

  vcl_string nobs_data_type;
  int nobstypesize;
  valid_types.empty();
  valid_types.push_back(bstm_data_traits<BSTM_NUM_OBS>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_NUM_OBS_SINGLE>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, nobs_data_type, nobstypesize ) ) {
    vcl_cout<<"bstm_cpp_copy_data_to_future_process ERROR: scene doesn't have BSTM_NUM_OBS or BSTM_NUM_OBS_VIEW_COMPACT or BSTM_NUM_OBS_SINGLE data type"<<vcl_endl;
    return false;
  }


  vcl_cout<<"Copying data to future..."<<vcl_endl;


  vcl_map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  vcl_map<bstm_block_id, bstm_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    bstm_block_id id = blk_iter->first;

    //skip block if it doesn't contain curr time.
    bstm_block_metadata mdata =  blk_iter->second;
    double local_time;
    if(!mdata.contains_t(time,local_time))
      continue;
    vcl_cout<<"Processsing Block: "<<id<<vcl_endl;

    bstm_block     * blk     = cache->get_block(id);
    bstm_time_block* blk_t   = cache->get_time_block(id);
    bstm_data_base * alph    = cache->get_data_base(id,bstm_data_traits<BSTM_ALPHA>::prefix());
    int num_el = alph->buffer_length() / bstm_data_traits<BSTM_ALPHA>::datasize();
    bstm_data_base * mog     = cache->get_data_base(id, app_data_type, apptypesize * num_el);
    bstm_data_base * num_obs = cache->get_data_base(id, nobs_data_type,nobstypesize * num_el );

    vcl_vector<bstm_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);

    //refine block and datas
    if(app_data_type == bstm_data_traits<BSTM_MOG3_GREY>::prefix() &&  nobs_data_type == bstm_data_traits<BSTM_NUM_OBS>::prefix()  )
      bstm_copy_data_to_future_function<BSTM_MOG3_GREY, BSTM_NUM_OBS> ( blk_t, blk, datas, local_time);
    else if (app_data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix() &&  nobs_data_type == bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix()  )
      bstm_copy_data_to_future_function<BSTM_MOG6_VIEW_COMPACT, BSTM_NUM_OBS_VIEW_COMPACT> ( blk_t, blk, datas, local_time);
    else if (app_data_type == bstm_data_traits<BSTM_GAUSS_RGB>::prefix() &&  nobs_data_type == bstm_data_traits<BSTM_NUM_OBS_SINGLE>::prefix()  )
      bstm_copy_data_to_future_function<BSTM_GAUSS_RGB, BSTM_NUM_OBS_SINGLE> ( blk_t, blk, datas, local_time);
    else {
      vcl_cerr << "bstm_cpp_copy_data_to_future_process ERROR! Types don't match...." << vcl_endl;
      vcl_cerr << "App type: " << app_data_type << " and nobs: " << nobs_data_type << vcl_endl;
    }
  }

  vcl_cout << "Finished copying data scene..." << vcl_endl;
  return true;
}