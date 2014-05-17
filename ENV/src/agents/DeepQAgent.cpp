#include "DeepQAgent.hpp"
#include "random_tools.h"
#include <iostream>
#include <zlib.h>
#include <iterator>
#include <cstring>
#include <random>
#include <vector>

void DeepQAgent::WriteEpsilon(const char* file_name) {
  ofstream file_out(file_name);
  file_out << epsilon_;
  cout << "write: epsilon " << epsilon_ << endl;
  file_out.close();
}

void DeepQAgent::ReadEpsilon(const char* file_name) {
  ifstream file_in(file_name);
  file_in >> epsilon_;
  cout << "Read: epsilon " << epsilon_ << endl;
  file_in.close();
}

DeepQAgent::DeepQAgent(OSystem* _osystem, RomSettings* _settings) : PlayerAgent(_osystem, _settings) {
  InitNN();
  
  pre_act_= 0;
  pre_sampling_array_ =NULL;
  pre_act_value_ = 0;
  process_count_ = 0;
  pre_lives_ = 5;
  epsilon_ = 1;
  sampling_array_ = new float[4*SAMPLE_WIDTH*SAMPLE_HEIGHT];
  pre_sampling_array_ = new float[4*SAMPLE_WIDTH*SAMPLE_HEIGHT];

  cout << "start of agent" << endl;
}

DeepQAgent::~DeepQAgent() {
  if (nn) {
    delete nn;
    nn = nullptr;
  }
  if (sampling_array_) {
    delete[] sampling_array_;
    sampling_array_ = nullptr;
  }
  if (pre_sampling_array_) {
    delete[] pre_sampling_array_;
    pre_sampling_array_ = nullptr;
  }
  if (nn_para) {
    delete nn_para;
    nn_para = nullptr;
  }
}

Action DeepQAgent::act() {
  Action a = Action(Action::PLAYER_A_FIRE);//legal_actions[rand() % legal_actions.size()];
  return a;
}

void DeepQAgent::SavePNG(IntMatrix* screen_matrix) {
  p_osystem->p_export_screen->export_any_matrix(screen_matrix,"check.png");
  return;
}

void DeepQAgent::SaveGrayPNG(double** gray_matrix, int width, int height, const char * file_name) {
  uInt8* buffer  = (uInt8*) NULL;
  uInt8* compmem = (uInt8*) NULL;
  ofstream out;
  try {
      // Get actual image dimensions. which are not always the same
      // as the framebuffer dimensions
    out.open(file_name, ios_base::binary);
    if(!out)
        throw "Couldn't open PNG file";
    // PNG file header
    uInt8 header[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    out.write((const char*)header, 8);
    // PNG IHDR
    uInt8 ihdr[13];
    ihdr[0]  = width >> 24;   // i_screen_width
    ihdr[1]  = width >> 16;
    ihdr[2]  = width >> 8;
    ihdr[3]  = width & 0xFF;
    ihdr[4]  = height >> 24;  // i_screen_height
    ihdr[5]  = height >> 16;
    ihdr[6]  = height >> 8;
    ihdr[7]  = height & 0xFF;
    ihdr[8]  = 8;  // 8 bits per sample (24 bits per pixel)
    ihdr[9]  = 2;  // PNG_COLOR_TYPE_RGB
    ihdr[10] = 0;  // PNG_COMPRESSION_TYPE_DEFAULT
    ihdr[11] = 0;  // PNG_FILTER_TYPE_DEFAULT
    ihdr[12] = 0;  // PNG_INTERLACE_NONE
    p_osystem->p_export_screen->writePNGChunk(out, "IHDR", ihdr, 13);
    // Fill the buffer with scanline data
    int rowbytes = width * 3;
    buffer = new uInt8[(rowbytes + 1) * height];
    uInt8* buf_ptr = buffer;
    for (int i = 0; i < height; i++) {
      *buf_ptr++ = 0;                  // first byte of row is filter type
      for (int j = 0; j < width; j++) {
        int gray_value = gray_matrix[i][j];
        buf_ptr[j * 3 + 0] = gray_value;
        buf_ptr[j * 3 + 1] = gray_value;
        buf_ptr[j * 3 + 2] = gray_value;
      }
      buf_ptr += rowbytes;                 // add pitch
    }
    // Compress the data with zlib
    uLongf compmemsize = (uLongf)((height * (width + 1) * 3 * 1.001 + 1) + 12);
    compmem = new uInt8[compmemsize];
    if (compmem == NULL ||
        (compress(compmem, &compmemsize, buffer, height *
                      (width * 3 + 1)) != Z_OK)) {
      throw "Error: Couldn't compress PNG";
    }
      
    // Write the compressed framebuffer data
    p_osystem->p_export_screen->writePNGChunk(out, "IDAT", compmem, compmemsize);
    // Add some info about this snapshot
    p_osystem->p_export_screen->writePNGText(out, "ROM Name", p_osystem->p_export_screen->p_props->get(Cartridge_Name));
    p_osystem->p_export_screen->writePNGText(out, "ROM MD5", p_osystem->p_export_screen->p_props->get(Cartridge_MD5));
    p_osystem->p_export_screen->writePNGText(out, "Display Format", p_osystem->p_export_screen->p_props->get(Display_Format));
    // Finish up
    p_osystem->p_export_screen->writePNGChunk(out, "IEND", 0, 0);
    // Clean up
    if (buffer)  delete[] buffer;
    if (compmem) delete[] compmem;
    out.close();
  } catch(const char *msg) {
    if(buffer)  delete[] buffer;
    if(compmem) delete[] compmem;
    out.close();
    cerr << msg << endl;
  }
}

void DeepQAgent::DownSampling(double **gray_array,double **sample_array,int width_size, 
                              int height_size, int sample_width, int sample_height) {
    // TODO be careful, here for breakout game, we would like to accelarate the process of traing, as a consequence,
    // we cut the top frame to cut off the irrelavent pictures.
    // all numbers here are magic, we just would like to test whether the program works or not.

    /*!!!!********************************************/
  height_size = 6/7.0 * height_size;
 /************************************************/
 // height_size =  height_size;

  double scale_height = (double)sample_height / (double)height_size;
  double scale_width =  (double)sample_width / (double)width_size;

  for (int cy = 0; cy < sample_height; ++cy) {
    for (int cx = 0; cx < sample_width; ++cx) {
      sample_array[cy][cx] = gray_array[(int)(1/7.0*height_size) +(int)(cy / scale_height)][(int)(cx/scale_width)];
         /************************************************/
         //sample_array[cy][cx] = gray_array[(int)(cy / scale_height)][(int)(cx/scale_width)];
    }
  }
}

void DeepQAgent::RGB2Gray(double** gray_array, uInt8 *buffer, int width, int height) {
  uInt8* buf_ptr = buffer;
  int rowbytes = width * 3;
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      gray_array[i][j] = 0.2989 * buf_ptr[j * 3 + 0] + 0.5879 * buf_ptr[j * 3 + 1] + 0.1140 * buf_ptr[j * 3 + 2];
    }      
    buf_ptr += rowbytes+1; //added one so that the gray picture is not distorted.
  }
}

void DeepQAgent::InitNN() {
  nn_para = new NNParameter;
  nn_para->im_size_ = 84;
  nn_para->channel_num_ = 4;
  nn_para->action_num_ = 3;
  nn_para->learn_rate_ = 0.001;
  nn_para->mb_size_ = 32;    
  // layer parameters
  LayerParameter* conv1 = new LayerParameter();
  LayerParameter* conv2 = new LayerParameter();
  LayerParameter* full1 = new LayerParameter();
  LayerParameter* full2 = new LayerParameter();

  conv1->type_ = "conv";
  conv1->input_im_size_ = nn_para->im_size_;
  conv1->input_size_ = (conv1->input_im_size_)*(conv1->input_im_size_);
  conv1->input_num_ = 4;
  conv1->filter_size_ = 8;
  conv1->filter_num_ = 16;
  conv1->stride_ = 4;
  conv2->type_ = "conv";
  conv2->filter_size_ = 4;
  conv2->filter_num_ = 32;
  conv2->stride_ = 2;
  full1->type_ = "full_rect";
  full1->input_im_size_ = nn_para->im_size_;
  full1->input_size_ = (nn_para->im_size_)*(nn_para->im_size_);
  full1->output_size_ = 256;
  full2->type_ = "full_linear";
  full2->output_size_ = nn_para->action_num_;

  nn_para->layer_para_.push_back(conv1);
  nn_para->layer_para_.push_back(conv2);
  nn_para->layer_para_.push_back(full1);
  nn_para->layer_para_.push_back(full2);

  nn = new NN(nn_para);
  nn->Init();
}

// Never use before assign sample size
void DeepQAgent::TestNNInput() {
  std::stringstream o;
  string name = "test_input_";
  std::cout << o.str();
  double ** sample_buffer = CreateArray(84,84);

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 84; ++j) {
      for (int k = 0; k < 84; ++k) {
        sample_buffer[j][k] =  sampling_array_[84*84*i + j*84 + k];
      }
    }
    o << i;
    string file_name = name + o.str() + ".png";
    SaveGrayPNG(sample_buffer,84,84, file_name.c_str());
  } 

  DestroyImgBuf(sample_buffer,84);
}

Action DeepQAgent::agent_step() {
  // Terminate if we have a maximum number of frames
  if (i_max_num_frames > 0 && frame_number >= i_max_num_frames) {
      end_game();
  }

  // Terminate this episode if we have reached the max. number of frames
  if (i_max_num_frames_per_episode > 0 &&
          episode_frame_number >= i_max_num_frames_per_episode) {
      return RESET;
  }
  // Only take an action if manual control is disabled
  Action a;
  a = act();

  int sample_width_ = 84;
  int sample_height_ = 84;

  int screen_width_ = p_osystem->console().mediaSource().width();
  int screen_height_ = p_osystem->console().mediaSource().height();

  IntMatrix screen_matrix;
  for (int i=0; i<screen_height_; ++i) {
      IntVect row;
      for (int j=0; j<screen_width_; ++j)
          row.push_back(-1);
      screen_matrix.push_back(row);
  }
  uInt8* pi_curr_frame_buffer = p_osystem->console().mediaSource().currentFrameBuffer();
  int ind_i, ind_j;
  for (int i = 0; i < screen_width_ * screen_height_; i++) {
      uInt8 v = pi_curr_frame_buffer[i];
      ind_i = i / screen_width_;
      ind_j = i - (ind_i * screen_width_);
      screen_matrix[ind_i][ind_j] = v;
  }

  // Process the Matrix
  uInt8* buffer  = (uInt8*) NULL;
  int rowbytes = screen_width_ * 3;
  buffer = new uInt8[(rowbytes + 1) * screen_height_];
  uInt8* buf_ptr = buffer;
  for(int i = 0; i < screen_height_; i++) {
      *buf_ptr++ = 0;                  // first byte of row is filter type
      for(int j = 0; j < screen_width_; j++) {
          int r, g, b;
          p_osystem->p_export_screen->get_rgb_from_palette((screen_matrix)[i][j], r, g, b);
          buf_ptr[j * 3 + 0] = r;
          buf_ptr[j * 3 + 1] = g;
          buf_ptr[j * 3 + 2] = b;
      }
      buf_ptr += rowbytes;                 // add pitch
  }

  double ** gray_buffer_ = NULL;
  double ** sample_buffer_ = NULL;

  gray_buffer_ = CreateArray(screen_width_,screen_height_);
  sample_buffer_ = CreateArray(sample_width_,sample_height_);

  // // Gray scaling
  this->RGB2Gray(gray_buffer_,buffer,screen_width_,screen_height_);

  // // DownSampling Reduce Size
  this->DownSampling(gray_buffer_,sample_buffer_,screen_width_,screen_height_,\
                     sample_width_,sample_height_);

  // // Copy sample file to the perminant address
  for (int i = 0; i < SAMPLE_WIDTH; i++) {
    for (int j = 0; j < SAMPLE_HEIGHT; j++) {
      sampling_array_[(process_count_%4)*SAMPLE_WIDTH*SAMPLE_HEIGHT+ i*SAMPLE_HEIGHT + j] = (float) sample_buffer_[i][j]/255;
    }
  }

    /*************************************************************/
  if (process_count_ == 0) {
//      ReadEpsilon("epsilon.txt");
//      nn->ReadWeights("weights.txt");
  }

  if (process_count_ % 4 == 3) {
    int DEATH_REWARD = -10;
    double DISCOUNT_FACTOR = 0.1;

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    double number = distribution(generator);

    float reward = 0;
    int lives = p_rom_settings->getBreakoutLives();

    if (pre_lives_ != lives) {
      reward = DEATH_REWARD;
    } 

    cout << "reward: " << reward << endl;     

    vector<Action> usable_action;
    usable_action.push_back(Action::PLAYER_A_LEFT);
    usable_action.push_back(Action::PLAYER_A_RIGHT);
    usable_action.push_back(Action::PLAYER_A_NOOP);    
  
    nn->AddMemIm(sampling_array_);  
    nn->Forward(nn->memory_idx_);
	nn->GetOutput();

    int act_value = 0;
    float max_Q_value = -100;
      
    if (number <= epsilon_) {
      a = choice(&usable_action);
      switch (a) {
      case Action::PLAYER_A_LEFT: act_value = 0;
        break;
      case Action::PLAYER_A_NOOP: act_value = 1;
          break;
      case Action::PLAYER_A_RIGHT: act_value = 2;
          break;
      default:
          break;
      }

      max_Q_value = nn->output_cpu_[act_value];

    } else {
      for (int i = 0; i < nn->action_num_; ++i) {
        float Q_value = nn->output_cpu_[i];                
        if (Q_value > max_Q_value) {
          act_value = i;
          max_Q_value = Q_value;
        }
      }

      switch (act_value) {
        case 0: a = Action::PLAYER_A_LEFT;
          break;
        case 1: a = Action::PLAYER_A_NOOP;
          break;
        case 2: a = Action::PLAYER_A_RIGHT;
          break;
        default:
          break;
      }
    }

    cout << "action: " << act_value << endl;

    nn->AddMemActionReward(act_value, reward);

    int mb_size = 32;
    for (int i = 0; i < mb_size; ++i) {
      nn->Train();	
    }

    nn->GetOutput();
    for (int a = 0; a < nn->action_num_; a++) {  
      float Q_value = nn->output_cpu_[a];
      cout << Q_value << " ";
    }    
        
    pre_act_ = a;
    pre_act_value_ = act_value;
    pre_lives_ = lives;

    float* temp = pre_sampling_array_;
    pre_sampling_array_ = sampling_array_;
    sampling_array_ = temp;
        
    cout << "frame:" << process_count_ << endl;
  }

  DestroyImgBuf(gray_buffer_,screen_height_);
  DestroyImgBuf(sample_buffer_,sample_height_);

  if (buffer) {
    delete[] buffer;
  } 

  if (record_trajectory) {
    record_action(a);
  }

  epsilon_ -= 0.000001;
  if (epsilon_ < 0.1) {
    epsilon_ = 0.1;
  }

//    if (process_count_ % 100 == 0) {
//        WriteEpsilon("epsilon.txt");
//        nn->WriteWeights("weights.txt");
//    }

  process_count_ ++;
  frame_number++;
  episode_frame_number++;

  return a;
}
