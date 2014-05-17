#ifndef DEEP_Q_AGENT_H
#define DEEP_Q_AGENT_H

#include "../common/Constants.h"
#include "PlayerAgent.hpp"
#include <cfloat>
#include "../emucore/OSystem.hxx"
#include "../dnq_gpu/nn.h"
#include "../dnq_gpu/layer.h"
#include "../dnq_gpu/parameter.h"
#include "../dnq_gpu/common.h"
#include "../dnq_gpu/math_functions.h"

#include "../games/RomUtils.hpp"

#include <vector>

#include <cuda.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <curand.h>


inline double** CreateArray(int width, int height)
{
    double** array   = new double*[height];
    for(int i = 0; i < height; ++i)
        array[i] = new double[width];
    return array;
}
inline void DestroyImgBuf(double ** &array,int height)
{
    for(int i = 0; i < height; ++i)
        delete[] array[i];
    delete[] array;
    array = NULL;
}
class DeepQAgent : public PlayerAgent
{
public:

    struct Memory
    {
        float prev_im[84*84*4];
        float im[84*84*4];
        int action;
        float reward;  
    }; 

    DeepQAgent(OSystem* _osystem,RomSettings* _settings);
    ~DeepQAgent();


protected:
    /* *********************************************************************
            Returns the best action from the set of possible actions
         ******************************************************************** */
    virtual Action act();
    virtual Action agent_step();
private:
    // The program is going to collect four images and process them together.
    // This variable detectes the number of time of running.
    static const int SAMPLE_WIDTH = 84;
    static const int SAMPLE_HEIGHT = 84;
    int process_count_;
    NN* nn; 
    NNParameter* nn_para;

    int pre_act_;
    float* sampling_array_;
    float* pre_sampling_array_;
    int pre_act_value_;
    int pre_lives_;
    double epsilon_;


    void InitNN();
    void UnitTest();
    void TestNNInput();
    void TestNNOutput();

    void WriteEpsilon(const char*);
    void ReadEpsilon(const char*);

    void DownSampling(double **gray_array, double ** sample_arrayint, int width_size, int height_size, int sample_width, int sample_height);
    void SavePNG(IntMatrix* const screen_matrix);
    void SaveGrayPNG(double** gray_buffer, int sample_width, int sample_height, const char * file_name);
    void RGB2Gray(double** gray_array, uInt8 *buffer, int width, int height);

};

#endif // DEEP_Q_AGENT_H
